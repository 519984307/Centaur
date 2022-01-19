/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "BinanceSPOT.hpp"

SpotMarketWS::SpotMarketWS()
{
    qRegisterMetaType<trader::api::StreamDepthUpdate>("trader::api::StreamDepthUpdate");
}

SpotMarketWS::~SpotMarketWS()
{
}

void SpotMarketWS::initialize(BinanceSpotPlugin *obj, cent::interface::ILogger *logger)
{
    m_obj    = obj;
    m_logger = logger;
    logger->trace("SpotMarketWS", "SpotMarketWS::initialize");
}

void SpotMarketWS::connected()
{
    if (m_logger)
    {
        m_logger->trace("SpotMarketWS", "SpotMarketWS::initialize");
        m_logger->info("SpotMarketWS", "Binance Spot WebSocket is connected");
    }
}

void SpotMarketWS::close()
{
    qDebug() << "Warning: SpotMarketWS is closed.";
}

void SpotMarketWS::connectionError()
{
    if (m_logger)
        m_logger->error("spotWS", "Binance Spot WebSocket failed to connect");
}

void SpotMarketWS::subscribe(const bool &status, const int &id)
{
    m_logger->trace("SpotMarketWS", "SpotMarketWS::subscribe");



    QMetaObject::invokeMethod(m_obj, "onSubscription",
        Qt::QueuedConnection,
        Q_ARG(bool, true),
        Q_ARG(bool, status),
        Q_ARG(int, id));
}

void SpotMarketWS::unsubscribe(const bool &status, const int &id)
{
    m_logger->trace("SpotMarketWS", "SpotMarketWS::unsubscribe");



    QMetaObject::invokeMethod(m_obj, "onSubscription",
        Qt::QueuedConnection,
        Q_ARG(bool, false),
        Q_ARG(bool, status),
        Q_ARG(int, id));
}

void SpotMarketWS::individualSymbolMiniTicker(const std::string &symbol, const uint64_t &eventTime, const trader::api::StreamIndividualSymbolMiniTicker &ticker)
{
    // m_logger->trace("SpotMarketWS", " SpotMarketWS::individualSymbolMiniTicker");

    // Return to the caller thread with the information necessary
    QMetaObject::invokeMethod(m_obj, "onTickerUpdate",
        Qt::QueuedConnection,
        Q_ARG(QString, QString(symbol.c_str())),
        Q_ARG(quint64, static_cast<quint64>(eventTime)),
        Q_ARG(double, static_cast<double>(ticker.closePrice)));
}

void SpotMarketWS::depthUpdate(const std::string &symbol, const uint64_t &eventTime, const trader::api::StreamDepthUpdate &sdp)
{
    // m_logger->trace("SpotMarketWS", " SpotMarketWS::depthUpdate");

    QMetaObject::invokeMethod(m_obj, "onDepthUpdate",
        Qt::QueuedConnection,
        Q_ARG(QString, QString(symbol.c_str())),
        Q_ARG(quint64, static_cast<quint64>(eventTime)),
        Q_ARG(trader::api::StreamDepthUpdate, sdp));
}
