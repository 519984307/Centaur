/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 30/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "BinanceCS.hpp"

CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::CandleSpotMarketWS(std::promise<void> connected) :
    BINAPI_NAMESPACE::ws::WSSpotBinanceAPI(""),
    m_connected { std::move(connected) }

{
    qRegisterMetaType<binapi::BinanceTimeIntervals>("binapi::BinanceTimeIntervals");
    qRegisterMetaType<binapi::Candlestick>("binapi::Candlestick");
}

CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::~CandleSpotMarketWS() = default;

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::initialize(BinanceCS *obj, CENTAUR_INTERFACE_NAMESPACE::ILogger *logger)
{
    m_obj    = obj;
    m_logger = logger;
}

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::connected()
{
    if (m_logger)
    {

        logTrace("CandleSpotMarketWS", "CandleSpotMarketWS::initialize");
        m_logger->info("CandleSpotMarketWS", "Binance Spot for candles WebSocket is connected");
    }

    m_connected.set_value();
}

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::close()
{
    //    qDebug() << "Warning: CandleSpotMarketWS is closed.";
}

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::connectionError()
{
    if (m_logger)
        m_logger->error("CandleSpotMarketWS", "Binance Spot for candles WebSockets failed to connect");

    m_connected.set_value();
}

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::subscribe(bool status, int id)
{
    logTrace("CandleSpotMarketWS", "CandleSpotMarketWS::subscribe");

    QMetaObject::invokeMethod(m_obj->getPluginObject(), "onSubscription",
        Qt::QueuedConnection,
        Q_ARG(bool, true),
        Q_ARG(bool, status),
        Q_ARG(int, id));
}

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::unsubscribe(bool status, int id)
{
    logTrace("CandleSpotMarketWS", "CandleSpotMarketWS::unsubscribe");

    QMetaObject::invokeMethod(m_obj->getPluginObject(), "onSubscription",
        Qt::QueuedConnection,
        Q_ARG(bool, false),
        Q_ARG(bool, status),
        Q_ARG(int, id));
}

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::kline(const std::string &symbol, uint64_t eventTime, BINAPI_NAMESPACE::BinanceTimeIntervals interval, const BINAPI_NAMESPACE::Candlestick &cs)
{
    // void onKLineUpdate(const QString &symbol, uint64_t eventTime, BINAPI_NAMESPACE::BinanceTimeIntervals interval, const BINAPI_NAMESPACE::Candlestick &cs);

    QMetaObject::invokeMethod(m_obj->getPluginObject(), "onKLineUpdate",
        Qt::QueuedConnection,
        Q_ARG(QString, QString(symbol.c_str())),
        Q_ARG(quint64, static_cast<quint64>(eventTime)),
        Q_ARG(binapi::BinanceTimeIntervals, interval),
        Q_ARG(binapi::Candlestick, cs));
}

void CENTAUR_PLUGIN_NAMESPACE::CandleSpotMarketWS::pingSent(bool success) noexcept
{
    logTrace("CandleSpotMarketWS", QString("Market SpotWS ping sent: %1").arg(success ? "Succeeded" : "Failed"));
}
