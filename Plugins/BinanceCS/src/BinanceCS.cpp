/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 27/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "BinanceCS.hpp"
#include "CentaurInterface.hpp"
#include "CentaurPlugin.hpp"
#include "uuid.hpp"

#include <QAction>
#include <QList>
#include <QObject>

/// UUID v5 Hashed String: CentaurProject-BinanceCandleStick-0.1.0

namespace
{
    constexpr char g_ExchangeRateName[]          = "BinanceCandleStick";
    constexpr char g_ExchangeRateVersionString[] = "0.1.0";
    constexpr char g_uuidString[]                = "{8fc9d583-07bc-5deb-b09e-acba58481e30}";
} // namespace

cen::plugin::BinanceCS::BinanceCS(QObject *parent) :
    QObject(parent)
{
    m_supportedExchanges.emplaceBack(PluginInformation { cen::uuid { "{85261bc6-8f92-57ca-802b-f08b819031db}" }, "BinanceSPOT", "0.1.0" });
    // Although Binance Futures WebSocket api has a slightly different approach from the Spot counterpart this plugin will handle both
    m_supportedExchanges.emplaceBack(PluginInformation { cen::uuid { "{f3a8e53e-06f8-54c5-8659-e46ff689e825}" }, "BinanceFutures", "0.1.0" });
}

QObject *cen::plugin::BinanceCS::getPluginObject() noexcept
{
    return qobject_cast<QObject *>(this);
}

QString cen::plugin::BinanceCS::getPluginName() noexcept
{
    return g_ExchangeRateName;
}

QString cen::plugin::BinanceCS::getPluginVersionString() noexcept
{
    return g_ExchangeRateVersionString;
}

void cen::plugin::BinanceCS::setPluginInterfaces(cen::interface::ILogger *logger, cen::interface::IConfiguration *config) noexcept
{
    m_logger = logger;
    m_config = config;
}

cen::uuid cen::plugin::BinanceCS::getPluginUUID() noexcept
{
    return uuid(g_uuidString);
}

BINAPI_NAMESPACE::BinanceTimeIntervals cen::plugin::BinanceCS::mapIntervalFromUIToAPI(cen::plugin::ICandleView::TimeFrame tf)
{
    switch (tf)
    {

        case TimeFrame::nullTime: C_FALLTHROUGH;                                        // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Seconds_1: C_FALLTHROUGH;                                       // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Seconds_5: C_FALLTHROUGH;                                       // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Seconds_10: C_FALLTHROUGH;                                      // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Seconds_30: C_FALLTHROUGH;                                      // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Seconds_45: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1m; // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Minutes_1: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1m;
        case TimeFrame::Minutes_2: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1m; // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Minutes_3: return BINAPI_NAMESPACE::BinanceTimeIntervals::i3m;
        case TimeFrame::Minutes_5: return BINAPI_NAMESPACE::BinanceTimeIntervals::i5m;
        case TimeFrame::Minutes_10: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1m; // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Minutes_15: return BINAPI_NAMESPACE::BinanceTimeIntervals::i15m;
        case TimeFrame::Minutes_30: return BINAPI_NAMESPACE::BinanceTimeIntervals::i30m;
        case TimeFrame::Minutes_45: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1m; // Not valid, however, supportedTimeFrames() must protect us from this case
        case TimeFrame::Hours_1: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1h;
        case TimeFrame::Hours_2: return BINAPI_NAMESPACE::BinanceTimeIntervals::i2h;
        case TimeFrame::Hours_4: return BINAPI_NAMESPACE::BinanceTimeIntervals::i4h;
        case TimeFrame::Hours_6: return BINAPI_NAMESPACE::BinanceTimeIntervals::i6h;
        case TimeFrame::Hours_8: return BINAPI_NAMESPACE::BinanceTimeIntervals::i8h;
        case TimeFrame::Hours_12: return BINAPI_NAMESPACE::BinanceTimeIntervals::i12h;
        case TimeFrame::Days_1: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1d;
        case TimeFrame::Days_3: return BINAPI_NAMESPACE::BinanceTimeIntervals::i3d;
        case TimeFrame::Weeks_1: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1w;
        case TimeFrame::Months_1: return BINAPI_NAMESPACE::BinanceTimeIntervals::i1M;
    }
}

QList<cen::plugin::PluginInformation> cen::plugin::BinanceCS::supportedExchanges() noexcept
{
    return m_supportedExchanges;
}

QList<cen::plugin::ICandleView::TimeFrame> cen::plugin::BinanceCS::supportedTimeFrames() noexcept
{
    // Supported TimeFrame by SPOT and Futures
    return {
        TimeFrame::Minutes_1, TimeFrame::Minutes_3, TimeFrame::Minutes_5, TimeFrame::Minutes_15, TimeFrame::Minutes_30, TimeFrame::nullTime,
        TimeFrame::Hours_1, TimeFrame::Hours_2, TimeFrame::Hours_4, TimeFrame::Hours_6, TimeFrame::Hours_8, TimeFrame::Hours_12, TimeFrame::nullTime,
        TimeFrame::Days_1, TimeFrame::Days_3, TimeFrame::nullTime,
        TimeFrame::Weeks_1, TimeFrame::nullTime,
        TimeFrame::Months_1
    };
}

void cen::plugin::BinanceCS::acquire(const PluginInformation &pi, const QString &symbol, cen::plugin::ICandleView::TimeFrame frame, const uuid &id) noexcept
{
    // Verify PluginInformation
    const uuid exchangeId {};

    auto supported = std::find(m_supportedExchanges.begin(), m_supportedExchanges.end(), pi);
    if (!supported)
    {
        logError("BinanceCS::acquire", "The sender is not supported");
        return;
    }

    if (pi.name == "BinanceSPOT")
    {
        // Handle SPOT
        m_symbolsHandled[id] = { symbol, frame }; // The UI makes sure this id is unique across all plugins, symbols amd timeframes
        if (m_spotWSThread.joinable() && m_candleWS)
        {
            logInfo("BinanceCS", QString("Attempting to add %1 to the KLine WS").arg(symbol));
            //         auto subsVar = m_candleWS->subscribeKline(symbol.toStdString(), BinanceCS::mapIntervalFromUIToAPI(frame));
            //        if (std::holds_alternative<int>(subsVar))
            //            m_wsIds[std::get<int>(subsVar)] = symbol;
        }
        else
        {
            //       startCandleThread(symbol, BinanceCS::mapIntervalFromUIToAPI(frame));
        }
    }
}

void cen::plugin::BinanceCS::disengage(const uuid &id, uint64_t lastTimeframeStart, uint64_t lastTimeframeEnd) noexcept
{
    logTrace("BinanceCS", "BinanceCS::removeSymbolFromWatchlist()");

    auto iter = m_symbolsHandled.find(id);
    if (iter == m_symbolsHandled.end())
    {
        logError("BinanceCS", "The user interface expect to remove an item that is not registered");
        return;
    }
    auto &data         = iter->second;
    const auto &symbol = std::get<0>(data);

    m_symbolsHandled.erase(id);

    logInfo("BinanceCS", QString("Attempting to remove %1 from the candle WS").arg(symbol));

    if (m_symbolsHandled.empty())
    {
        /// if the list is empty prefer to stop the thread
        logWarn("BinanceCS", "The KLine SPOT WebSocket thread will be stopped because the candle list is empty.");

        // Stop the thread instead
        if (m_spotWSThread.joinable() && m_candleWS)
        {
            m_candleWS->terminate();
            m_spotWSThread.join();
        }
        m_candleWS.reset();
        logWarn("BinanceCS", "The Spot KLine WebSocket thread stopped.");
    }
    else
    {
        // Stop receiving data
        int id      = m_candleWS->unsubscribeKline(symbol.toStdString(), BinanceCS::mapIntervalFromUIToAPI(std::get<1>(data)));
        m_wsIds[id] = symbol;
    }
}

void cen::plugin::BinanceCS::resetStoredZoom(const uuid &id) noexcept
{
}

QList<QPair<uint64_t, cen::plugin::ICandleView::CandleData>> cen::plugin::BinanceCS::getCandlesByPeriod(const QString &symbol, uint64_t start, uint64_t end, cen::plugin::ICandleView::TimeFrame frame) noexcept
{
    const auto interval = BinanceCS::mapIntervalFromUIToAPI(frame);

    uint64_t total;
    auto data = BINAPI_NAMESPACE::BinanceAPI::getCandlesTimesAndLimits(interval, start, end, total);

    BINAPI_NAMESPACE::BinanceLimits limits;
    BINAPI_NAMESPACE::BinanceAPISpot spot { nullptr, &limits };

    try
    {
        QList<QPair<Timestamp, CandleData>> cd;
        for (auto i = 0ull; i < data.size(); ++i)
        {
            auto sym     = symbol.toStdString();
            auto candles = spot.candlestickData(sym.c_str(), interval, std::get<0>(data[i]), std::get<1>(data[i]), std::get<2>(data[i]));

            for (const auto &candle : candles)
            {
                if (!candle.isClosed)
                {
                    // Do not set the candle
                    continue;
                }

                cd.push_back({
                    candle.openTime,
                    {.high     = candle.high,
                               .open   = candle.open,
                               .close  = candle.close,
                               .low    = candle.low,
                               .volume = candle.volume}
                });
            }
        }
        // Sort the data
        std::sort(cd.begin(), cd.end(), [](const QPair<Timestamp, CandleData> &c1, const QPair<Timestamp, CandleData> &c2) {
            return c1.first < c2.first;
        });

        return cd;
    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        return {};
    }
}

bool cen::plugin::BinanceCS::realtimePlotAllowed() noexcept
{
    // This plugin dynamically sets the candles
    return true;
}

bool cen::plugin::BinanceCS::dynamicReframePlot() noexcept
{
    // The data can be reframed
    return true;
}

void cen::plugin::BinanceCS::reframe(cen::plugin::ICandleView::TimeFrame frame) noexcept
{
}

void cen::plugin::BinanceCS::startCandleThread(const QString &symbol, BINAPI_NAMESPACE::BinanceTimeIntervals interval)
{
    logTrace("BinanceCS", "BinanceSpotPlugin::runMarketWS");
    if (m_spotWSThread.joinable())
    {
        m_candleWS->terminate();
        m_spotWSThread.join();
    }

    m_candleWS.reset();

    std::promise<void> connected;
    std::future<void> future = connected.get_future();

    m_candleWS = std::make_unique<CandleSpotMarketWS>(std::move(connected));
    m_candleWS->initialize(this, m_logger);

    auto kline           = m_candleWS->subscribeKline(symbol.toStdString(), interval);
    const std::string ep = m_candleWS->constructEndPoint(std::get<std::string>(kline));

    m_candleWS->setEndPoint(ep);

    m_spotWSThread = std::thread([&] {
        logInfo("BinanceCS", "Spot WebSocket thread created.");
        m_candleWS->run();
    });

    future.wait();
    logInfo("BinanceCS", "The main thread is unblocked");
}

void cen::plugin::BinanceCS::onSubscription(bool subscribe, bool status, int id) noexcept
{
    logTrace("BinanceCS", "BinanceCS::onSubscription()");

    if (auto idIter = m_wsIds.find(id); idIter == m_wsIds.end())
    {
        logError("BinanceCS", "A unknown (un)subscribe message was received.");
        return;
    }
    else
    {

        auto &name = idIter->second;

        if (!status)
            logWarn("BinanceCS", QString("%1 for %2 with id: %3 failed").arg(subscribe ? "Subscription" : "Unsubscription", name).arg(id));
        else
            logInfo("BinanceCS", QString("%1 for %2 with id: %3 succeed").arg(subscribe ? "Subscription" : "Unsubscription", name).arg(id));

        m_wsIds.erase(id);
    }
}

void cen::plugin::BinanceCS::BinanceCS::onKLineUpdate(const QString &symbol, quint64 eventTime, C_UNUSED BINAPI_NAMESPACE::BinanceTimeIntervals interval, const BINAPI_NAMESPACE::Candlestick &cs) noexcept
{

    uuid id;

    for (const auto &uuids : m_symbolsHandled)
    {
        const auto &sym = std::get<0>(uuids.second);
        if (sym == symbol && BinanceCS::mapIntervalFromUIToAPI(std::get<1>(uuids.second)) == interval)
        {
            id = uuids.first;
            break;
        }
    }

    CandleData cd {
        .high   = cs.high,
        .open   = cs.open,
        .close  = cs.close,
        .low    = cs.low,
        .volume = cs.volume,
    };

    emit snRealTimeCandleUpdate(id, eventTime, cs.openTime, cd);
}
