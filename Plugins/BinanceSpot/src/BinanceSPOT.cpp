/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include <QObject>

#include "BinanceSPOT.hpp"
#include "protocol.hpp"
#include <QMessageBox>
#include <fmt/core.h>

namespace
{
    /// UUID V5 string "CentaurProject-BinanceExchangeSPOT-0.1.0"
    constexpr char g_BinanceSpotName[]          = "BinanceSPOT";
    constexpr char g_BinanceSpotVersionString[] = "0.1.0";
    constexpr char g_uuidString[]               = "{85261bc6-8f92-57ca-802b-f08b819031db}";
} // namespace

#define CATCH_API_EXCEPTION()                                                                                                                                              \
    switch (ex.type())                                                                                                                                                     \
    {                                                                                                                                                                      \
        case BINAPI_NAMESPACE::APIException::Type::request:                                                                                                                \
            {                                                                                                                                                              \
                auto request = ex.request();                                                                                                                               \
                logError("BinanceSPOTPlugin", fmt::format("REQUEST.\nCode: {}. URL: {}\n{}\n", std::get<0>(request), std::get<1>(request), std::get<2>(request)).c_str()); \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::limits:                                                                                                                 \
            logError("BinanceSPOTPlugin", fmt::format("LIMITS.\n{}\n", std::get<0>(ex.limits())).c_str());                                                                 \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::http1:                                                                                                                  \
            {                                                                                                                                                              \
                auto http1 = ex.http1();                                                                                                                                   \
                logError("BinanceSPOTPlugin", fmt::format("HTTPa.\nCode: {}\nURL: {}\n{}\n", std::get<0>(http1), std::get<1>(http1), std::get<2>(http1)).c_str());         \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::http2:                                                                                                                  \
            {                                                                                                                                                              \
                auto http2 = ex.http2();                                                                                                                                   \
                logError("BinanceSPOTPlugin", fmt::format("HTTPb.\nStatus: {}. Code: {}\nURL: {}.\n{}\n",                                                                  \
                                                  std::get<0>(http2),                                                                                                      \
                                                  std::get<1>(http2),                                                                                                      \
                                                  std::get<2>(http2),                                                                                                      \
                                                  std::get<4>(http2))                                                                                                      \
                                                  .c_str());                                                                                                               \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::json:                                                                                                                   \
            {                                                                                                                                                              \
                auto json = ex.json();                                                                                                                                     \
                logError("BinanceSPOTPlugin", fmt::format("JSON.\nURL: {}\n{}\n", std::get<0>(json), std::get<1>(json)).c_str());                                          \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::api:                                                                                                                    \
            {                                                                                                                                                              \
                auto api1 = ex.api();                                                                                                                                      \
                logError("BinanceSPOTPlugin", fmt::format("API.\nCode: {}\nURL: {}\n{}", std::get<0>(api1), std::get<1>(api1), std::get<2>(api1)).c_str());                \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::schema:                                                                                                                 \
            {                                                                                                                                                              \
                auto schema = ex.schema();                                                                                                                                 \
                logError("BinanceSPOTPlugin", fmt::format("SCHEMA.\nURL: {}\n{}\n", std::get<0>(schema), std::get<1>(schema)).c_str());                                    \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
    }

CENTAUR_NAMESPACE::BinanceSpotPlugin::BinanceSpotPlugin(QObject *parent) :
    QObject(parent),
    m_spotWS { nullptr },
    m_globalPluginUuid { g_uuidString }
{
}

CENTAUR_NAMESPACE::BinanceSpotPlugin::~BinanceSpotPlugin()
{
    if (m_spotWSThread != nullptr && m_spotWSThread->joinable())
    {
        m_spotWS->terminate();
        m_spotWSThread->join();
    }
    m_spotWSThread.reset();
    m_spotWS.reset();
}

QObject *CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginObject() noexcept
{
    return reinterpret_cast<QObject *>(this);
}

QString CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginVersionString() noexcept
{
    return g_BinanceSpotVersionString;
}

QString CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginName() noexcept
{
    return g_BinanceSpotName;
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, CENTAUR_INTERFACE_NAMESPACE::ILongOperation *lOper) noexcept
{
    m_logger        = logger;
    m_config        = config;
    m_longOperation = lOper;
}

CENTAUR_NAMESPACE::uuid CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginUUID() noexcept
{
    return m_globalPluginUuid;
}

bool CENTAUR_NAMESPACE::BinanceSpotPlugin::addMenuAction(QAction *action, const uuid &menuId) noexcept
{
    const uuid spotStatus { "{5445eaec-d22c-4204-b720-ab07a570ab2e}" };
    if (menuId == spotStatus)
    {
        connect(action, &QAction::triggered, this, &BinanceSpotPlugin::onSpotStatus);
        return true;
    }

    return false;
}

bool CENTAUR_NAMESPACE::BinanceSpotPlugin::initialization() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::initialization");

    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;
    auto publicKeyFile = m_config->getPluginPublicKeyPath();

    try
    {
        ec.loadPublicKey(publicKeyFile);
    } catch (const std::exception &ex)
    {
        logError("BinanceSpotPlugin", QString("Could not load the plugin public key. %1").arg(ex.what()));
        return false;
    }

    bool apiError, secError;
    const auto apiKeyCip = m_config->getValue("apiKey", &apiError);
    const auto secKeyCip = m_config->getValue("secretKey", &secError);

    if (!apiError || !secError)
    {
        logError("BinanceSpotPlugin", "Either the API Key or the secret key are empty on the plugins configuration file.");
        return false;
    }

    const auto apiKey = ec.decryptPublic(apiKeyCip, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64);
    const auto secKey = ec.decryptPublic(secKeyCip, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64);

    if (apiKey.empty() || secKey.empty())
    {
        logError("BinanceSpotPlugin", "Could not decrypt the binance keys");
        return false;
    }

    // set the keys
    m_keys.apiKey    = apiKey;
    m_keys.secretKey = secKey;

    m_api            = std::make_unique<BINAPI_NAMESPACE::BinanceAPISpot>(&m_keys, &m_limits);

    try
    {
        m_api->ping();
        logInfo("BinanceSpotPlugin", "Binance Server SPOT ping correctly");
        if (!m_api->getExchangeStatus())
        {
            logError("BinanceSpotPlugin", "Binance Server is under maintenance");
            return false;
        }

        logInfo("BinanceSpotPlugin", "Acquiring Binance SPOT Exchange Information");
        m_exchInfo = m_api->getExchangeInformation();
        logInfo("BinanceSpotPlugin", QString("Binance SPOT Exchange Information Acquired in ##00FFFF#%1# secs").arg(m_api->getLastCallTime()));

        m_limits.setAPIRequestsLimits(m_exchInfo.limitWeight.limit, m_exchInfo.limitWeight.seconds);
        m_limits.setAPIOrderLimitLow(m_exchInfo.limitOrderSecond.limit, m_exchInfo.limitOrderSecond.seconds);
        m_limits.setAPIOrderLimitHigh(m_exchInfo.limitOrderDay.limit, m_exchInfo.limitOrderDay.seconds);

        logInfo("BinanceSpotPlugin", "Binance SPOT limits changed.");

        for (const auto &[sym, info] : m_exchInfo.symbols)
        {
            if (info.permissions.test(BINAPI_NAMESPACE::SPOT::AccountPermissions::spot))
                m_symbols.push_back({ QString { info.symbol.c_str() }, &m_cryptoIcon });
        }
        logInfo("BinanceSpotPlugin", QString("Symbols handled: ##00FFFF#%1#").arg(m_symbols.size()));
    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        CATCH_API_EXCEPTION()
        return false;
    }
    return true;
}

CENTAUR_PLUGIN_NAMESPACE::StringIcon CENTAUR_NAMESPACE::BinanceSpotPlugin::getSymbolListName() const noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::getSymbolListName");

    return { QString { g_BinanceSpotName }, &m_exchIcon };
}

CENTAUR_PLUGIN_NAMESPACE::StringIconVector CENTAUR_NAMESPACE::BinanceSpotPlugin::getSymbolList() const noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::getSymbolList");

    return m_symbols;
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::runMarketWS(const QString &symbol) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::runMarketWS");

    m_longOperation->show("Starting WebSocket service...", QIcon {}, false);

    if (m_spotWSThread && m_spotWSThread->joinable())
    {
        m_spotWS->terminate();
        m_spotWSThread->join();
    }
    m_spotWSThread.reset();
    m_spotWS.reset();

    std::promise<void> connected;
    std::future<void> future = connected.get_future();

    m_spotWS                 = std::make_unique<SpotMarketWS>(std::move(connected));
    m_spotWS->initialize(this, m_logger);

    auto ticker          = m_spotWS->subscribeIndividualMiniTicker(symbol.toStdString());
    const std::string ep = m_spotWS->constructEndPoint(std::get<std::string>(ticker));

    m_spotWS->setEndPoint(ep);

    m_spotWSThread = std::make_unique<std::thread>([&] {
        logInfo("BinanceSpotPlugin", "Spot WebSocket thread created.");
        m_spotWS->run();
    });

    future.wait();

    logInfo("BinanceSpotPlugin", "The main thread is unblocked");

    m_longOperation->hide();
}

bool CENTAUR_NAMESPACE::BinanceSpotPlugin::addSymbol(const QString &name, const int &item) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::addSymbol()");

    auto itemId = m_symbolId.find(name);
    if (itemId != m_symbolId.end())
    {
        logError("BinanceSpotPlugin", QString("The %1 symbol is already handled").arg(name));
        return false;
    }

    logInfo("BinanceSpotPlugin", QString("Attempting to add %1 to the watchlist").arg(name));

    if (!m_spotWSThread || !m_spotWS) // The WS is not running
    {
        logInfo("BinanceSpotPlugin", "Adding the symbol will attempt to run a websocket.");
        // Start the thread and specify the new data
        runMarketWS(name);
    }
    else
    {
        // Subscribe the method
        logInfo("BinanceSpotPluing", QString("Attempting to add %1 to the wathclist").arg(name));
        auto subsVar = m_spotWS->subscribeIndividualMiniTicker(name.toStdString());
        if (std::holds_alternative<int>(subsVar))
            m_wsIds[std::get<int>(subsVar)] = name;
    }

    // According to the API, there is no way to see if the stream was succesfully retrieved in this part of the code
    // since the stream receiving is asychronous
    // So we'll handle the symbol return true even if there is a slight chance that this will not happen
    m_symbolId[name] = item;
    return true;
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::removeSymbol(const QString &name) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::removeSymbol()");

    logInfo("BinanceSpotPlugin", QString("Attempting to remove %1 to the watchlist").arg(name));

    // Remove from the id's list
    m_symbolId.erase(name);

    if (m_symbolId.empty())
    {
        /// if the list is empty prefer to stop the thread
        logWarn("BinanceSpotPlugin", "The Spot WebSocket thread will be stopped because the watchlist is empty.");

        // Stop the thread instead
        if (m_spotWSThread != nullptr && m_spotWSThread->joinable())
        {
            m_spotWS->terminate();
            m_spotWSThread->join();
        }
        m_spotWSThread.reset();
        logWarn("BinanceSpotPlugin", "The Spot WebSocket thread stopped.");
    }
    else
    {
        // Stop receiving data
        int id      = m_spotWS->unsubscribeIndividualMiniTicker(name.toStdString());
        m_wsIds[id] = name;
    }
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onTickerUpdate(const QString &symbol, const quint64 &receivedTime, const double &price) noexcept
{
    // Don't log since this message will be sent every 500 ms

    // Find the symbol id
    auto itemId = m_symbolId.find(symbol);
    if (itemId == m_symbolId.end())
    {
        logError("BinanceSpotPlugin", QString("The id for the %1 symbol was not found. Ticker can not be send").arg(symbol));
        return;
    }

    // Redirect the signal
    emit snTickerUpdate(symbol, itemId->second, receivedTime, price);
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSubscription(const bool &subscribe, const bool &status, const int &id) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSubscription()");

    if (auto idIter = m_wsIds.find(id); idIter == m_wsIds.end())
    {
        logError("BinanceSpotPlugin", "A unknown (un)subscribe message was received.");
        return;
    }
    else
    {

        auto &name = idIter->second;

        if (!status)
            logWarn("BinanceSpotPlugin", QString("%1 for %2 with id: %3 failed").arg(subscribe ? "Subscription" : "Unsubscription", name).arg(id));
        else
            logInfo("BinanceSpotPlugin", QString("%1 for %2 with id: %3 succeed").arg(subscribe ? "Subscription" : "Unsubscription", name).arg(id));

        m_wsIds.erase(id);
    }
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::updateOrderbook(const QString &symbol) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotSymbolListPlugin::updateOrderbook()");

    auto itemId = m_symbolId.find(symbol);
    if (itemId == m_symbolId.end())
    {
        logError("BinanceSpotPlugin", QString("The %1 symbol is not handled and the orderbook can not be send").arg(symbol));
        return;
    }

    logInfo("BinanceSpotPlugin", QString("Attempting to subscribe Orderbook event for %1").arg(symbol));

    auto subsVar = m_spotWS->subscribeDiffBookDepth(symbol.toStdString(), 1000);

    if (std::holds_alternative<int>(subsVar))
    {
        int &id     = std::get<int>(subsVar);
        m_wsIds[id] = symbol;
        logInfo("BinanceSpotPlugin", QString("Orderbook subscription event id for %1 is %2").arg(symbol).arg(id));
    }

    m_symbolOrderbookSnapshot[symbol] = { false, 0 };
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::stopOrderbook(const QString &symbol) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotSymbolListPlugin::stopOrderbook()");
    auto itemId = m_symbolId.find(symbol);
    if (itemId == m_symbolId.end())
    {
        logError("BinanceSpotPlugin", QString("The %1 symbol is not handled and the orderbook can not be stopped").arg(symbol));
        return;
    }

    logInfo("BinanceSpotPlugin", QString("Attempting to unsubscribe Orderbook event for %1").arg(symbol));

    int id = m_spotWS->unsubscribeDiffBookDepth(symbol.toStdString(), 1000);
    logInfo("BinanceSpotPlugin", QString("Orderbook unsubscribe event id for %1 is %2").arg(symbol).arg(id));
    m_wsIds[id] = symbol;

    m_symbolOrderbookSnapshot.erase(symbol);
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onDepthUpdate(const QString &symbol, const quint64 &eventTime, const BINAPI_NAMESPACE::StreamDepthUpdate &sdp) noexcept
{
    /*
    Open a stream to wss://stream.binance.com:9443/ws/bnbbtc@@depth.
    Buffer the events you receive from the stream.
    Get a depth snapshot from https://api.binance.com/api/v3/depth?symbol=BNBBTC&limit=1000 .
    Drop any event where u is <= lastUpdateId in the snapshot.
    The first processed event should have U <= lastUpdateId+1 AND u >= lastUpdateId+1.
    While listening to the stream, each new event's U should be equal to the previous event's u+1.
    The data in each event is the absolute quantity for a price level.
    If the quantity is 0, remove the price level.
    Receiving an event that removes a price level that is not in your local order book can happen and is normal.
    */

    const auto &[snap, lastUpdate] = m_symbolOrderbookSnapshot[symbol];
    uint64_t lastUpdateId          = lastUpdate;
    if (!snap)
    {
        // Take the snapshot

        BINAPI_NAMESPACE::BinanceAPISpot api(nullptr, nullptr);

        try
        {
            auto orderbook                    = m_api->getOrderBook(symbol.toStdString(), 1000);
            m_symbolOrderbookSnapshot[symbol] = { true, lastUpdateId };
            lastUpdateId                      = orderbook.lastUpdateId;
            logInfo("BinanceSpotPlugin", QString("Orderbook snapshot successfully taken for %1").arg(symbol));
        } catch (const BINAPI_NAMESPACE::APIException &ex)
        {
            CATCH_API_EXCEPTION()
            return;
        }
    }

    if (sdp.finalUpdateId <= lastUpdateId)
    {
        logWarn("BinanceSpotPlugin", QString("Orderbook event ignored for %1").arg(symbol));
        return;
    }

    QMap<qreal, QPair<qreal, qreal>> bids, asks;

    for (const auto &[price, quantity] : sdp.bids)
    {
        if (quantity > 0.0)
        {
            const double total = price * quantity;

            bids[price]        = { quantity, total };
        }
    }

    for (const auto &[price, quantity] : sdp.asks)
    {
        if (quantity > 0.0)
        {
            const double total = price * quantity;

            asks[price]        = { quantity, total };
        }
    }

    emit snOrderbookUpdate(g_uuidString, symbol, eventTime, bids, asks);

    m_symbolOrderbookSnapshot[symbol] = { true, sdp.finalUpdateId };
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSpotStatus() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSpotStatus");
    QString message;
    try
    {
        if (m_api->getExchangeStatus())
            message = "System status: Normal";
        else
            message = "System status: System maintenance";
    } catch (C_UNUSED const std::exception &ex)
    {
        message = "System status: API Error";
    }

    QMessageBox::information(nullptr, "System status", message, QMessageBox::StandardButton::Ok);
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onCoinInformation() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onCoinInformation");
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSpotDepositHistory() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSpotDepositHistory");
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSpotWithdrawHistory() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSpotWithdrawHistory");
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onGetAllOrders() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onGetAllOrders");
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onAccountTradeList() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onAccountTradeList");
}
