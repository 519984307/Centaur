/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "BinanceSPOT.hpp"

CENTAUR_NAMESPACE::SpotMarketWS::SpotMarketWS(std::promise<void> connected) :
    BINAPI_NAMESPACE::ws::WSSpotBinanceAPI(""),
    m_connected { std::move(connected) }

{
    qRegisterMetaType<binapi::StreamDepthUpdate>("binapi::StreamDepthUpdate");
}

CENTAUR_NAMESPACE::SpotMarketWS::~SpotMarketWS() = default;

void CENTAUR_NAMESPACE::SpotMarketWS::initialize(BinanceSpotPlugin *obj, CENTAUR_INTERFACE_NAMESPACE::ILogger *logger)
{
    m_obj    = obj;
    m_logger = logger;
}

void CENTAUR_NAMESPACE::SpotMarketWS::connected()
{
    if (m_logger)
    {

        logTrace("SpotMarketWS", "SpotMarketWS::initialize");
        m_logger->info("SpotMarketWS", "Binance Spot WebSocket is connected");
    }

    m_connected.set_value();
}

void CENTAUR_NAMESPACE::SpotMarketWS::close()
{
    //    qDebug() << "Warning: SpotMarketWS is closed.";
}

void CENTAUR_NAMESPACE::SpotMarketWS::connectionError()
{
    if (m_logger)
        m_logger->error("spotWS", "Binance Spot WebSocket failed to connect");

    m_connected.set_value();
}

void CENTAUR_NAMESPACE::SpotMarketWS::subscribe(const bool &status, const int &id)
{
    logTrace("SpotMarketWS", "SpotMarketWS::subscribe");

    QMetaObject::invokeMethod(m_obj->getPluginObject(), "onSubscription",
        Qt::QueuedConnection,
        Q_ARG(bool, true),
        Q_ARG(bool, status),
        Q_ARG(int, id));
}

void CENTAUR_NAMESPACE::SpotMarketWS::unsubscribe(const bool &status, const int &id)
{
    logTrace("SpotMarketWS", "SpotMarketWS::unsubscribe");

    QMetaObject::invokeMethod(m_obj->getPluginObject(), "onSubscription",
        Qt::QueuedConnection,
        Q_ARG(bool, false),
        Q_ARG(bool, status),
        Q_ARG(int, id));
}

void CENTAUR_NAMESPACE::SpotMarketWS::individualSymbolMiniTicker(const std::string &symbol, const uint64_t &eventTime, const BINAPI_NAMESPACE::StreamIndividualSymbolMiniTicker &ticker)
{
    // m_logger->trace("SpotMarketWS", " SpotMarketWS::individualSymbolMiniTicker");

    // Return to the caller thread with the information necessary
    QMetaObject::invokeMethod(m_obj->getPluginObject(), "onTickerUpdate",
        Qt::QueuedConnection,
        Q_ARG(QString, QString(symbol.c_str())),
        Q_ARG(quint64, static_cast<quint64>(eventTime)),
        Q_ARG(double, static_cast<double>(ticker.closePrice)));
}

void CENTAUR_NAMESPACE::SpotMarketWS::depthUpdate(const std::string &symbol, const uint64_t &eventTime, const BINAPI_NAMESPACE::StreamDepthUpdate &sdp)
{
    // m_logger->trace("SpotMarketWS", " SpotMarketWS::depthUpdate");

    QMetaObject::invokeMethod(m_obj->getPluginObject(), "onDepthUpdate",
        Qt::QueuedConnection,
        Q_ARG(QString, QString(symbol.c_str())),
        Q_ARG(quint64, static_cast<quint64>(eventTime)),
        Q_ARG(binapi::StreamDepthUpdate, sdp));
}

void cen::SpotMarketWS::pingSent(const bool &success) noexcept
{
    logTrace("SpotMarketWS", QString("Market SpotWS ping sent: %1").arg(success ? "Succeeded" : "Failed"));
}
