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

void cen::plugin::BinanceCS::setPluginInterfaces(cen::interface::ILogger *logger, cen::interface::IConfiguration *config, C_UNUSED cen::interface::ILongOperation *lOper) noexcept
{
    m_logger = logger;
    m_config = config;
}

cen::uuid cen::plugin::BinanceCS::getPluginUUID() noexcept
{
    return uuid(g_uuidString);
}

bool cen::plugin::BinanceCS::addMenuAction(C_UNUSED QAction *action, C_UNUSED const uuid &menuId) noexcept
{
    return false;
}

QList<cen::plugin::PluginInformation> cen::plugin::BinanceCS::supportedExchanges() noexcept
{
    return m_supportedExchanges;
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
    }
}

void cen::plugin::BinanceCS::disengage(const uuid &id, uint64_t lastTimeframeStart, uint64_t lastTimeframeEnd) noexcept
{
}

void cen::plugin::BinanceCS::resetStoredZoom(const uuid &id) noexcept
{
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
QList<QPair<uint64_t, cen::plugin::ICandleView::CandleData>> cen::plugin::BinanceCS::getCandlesByPeriod(const QString &symbol, uint64_t start, uint64_t end, cen::plugin::ICandleView::TimeFrame frame) noexcept
{
    return QList<QPair<Timestamp, CandleData>>();
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

QList<QToolBar *> cen::plugin::BinanceCS::getPluginBasedToolBar() noexcept
{
    return QList<QToolBar *>();
}
