/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 27/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_BINANCECS_HPP
#define CENTAUR_BINANCECS_HPP

#include "BinanceAPI.hpp"
#include <CentaurInterface.hpp>
#include <CentaurPlugin.hpp>
#include <QLabel>
#include <QMouseEvent>

namespace CENTAUR_PLUGIN_NAMESPACE
{

    struct SymbolCandleData
    {
        QString symbol;
        uuid exchange;
        uuid id;
        CENTAUR_PLUGIN_NAMESPACE::ICandleView::CandleData cd;
    };

    class CandleSpotMarketWS;
    class BinanceCS : public QObject,
                      public CENTAUR_PLUGIN_NAMESPACE::ICandleView
    {

        Q_OBJECT

        Q_PLUGIN_METADATA(IID "com.centaur-project.plugin.BinanceCS/1.0")
        Q_INTERFACES(CENTAUR_PLUGIN_NAMESPACE::IBase CENTAUR_PLUGIN_NAMESPACE::ICandleView)

    public:
        explicit BinanceCS(QObject *parent = nullptr);
        ~BinanceCS() override = default;

        QObject *getPluginObject() noexcept override;
        QString getPluginName() noexcept override;
        QString getPluginVersionString() noexcept override;
        void setPluginInterfaces(cen::interface::ILogger *logger, cen::interface::IConfiguration *config) noexcept override;
        uuid getPluginUUID() noexcept override;
        C_NODISCARD QList<PluginInformation> supportedExchanges() noexcept override;
        void acquire(const PluginInformation &pi, const QString &symbol, TimeFrame frame, const uuid &id) noexcept override;
        void disengage(const uuid &id, uint64_t lastTimeframeStart, uint64_t lastTimeframeEnd) noexcept override;
        void resetStoredZoom(const uuid &id) noexcept override;
        QList<TimeFrame> supportedTimeFrames() noexcept override;
        QList<QPair<Timestamp, CandleData>> getCandlesByPeriod(const QString &symbol, Timestamp start, Timestamp end, TimeFrame frame) noexcept override;
        bool realtimePlotAllowed() noexcept override;
        bool dynamicReframePlot() noexcept override;
        void reframe(TimeFrame frame) noexcept override;

    private:
        void startCandleThread(const QString &symbol, BINAPI_NAMESPACE::BinanceTimeIntervals interval);

    public slots:
        void onSubscription(bool subscribe, bool status, int id) noexcept;
        void onKLineUpdate(const QString &symbol, quint64 eventTime, BINAPI_NAMESPACE::BinanceTimeIntervals interval, const BINAPI_NAMESPACE::Candlestick &cs) noexcept;

    protected:
        static BINAPI_NAMESPACE::BinanceTimeIntervals mapIntervalFromUIToAPI(TimeFrame tf);

    signals:
        void snRealTimeCandleUpdate(const cen::uuid &id, quint64 eventTime, cen::plugin::ICandleView::Timestamp currentCandle, const cen::plugin::ICandleView::CandleData &candle);
        void snReframe(const uuid &id);

    private:
        std::unique_ptr<CandleSpotMarketWS> m_candleWS { nullptr };
        std::thread m_spotWSThread;
        std::map<int, QString> m_wsIds;

    protected:
        QList<PluginInformation> m_supportedExchanges;

        // Symbols handled
    protected:
        std::unordered_map<uuid, std::tuple<QString, TimeFrame>> m_symbolsHandled;

    private:
        CENTAUR_INTERFACE_NAMESPACE::ILogger *m_logger { nullptr };
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config { nullptr };
        uuid m_thisUUID;
    };

    class CandleSpotMarketWS : public BINAPI_NAMESPACE::ws::WSSpotBinanceAPI
    {
    public:
        CandleSpotMarketWS(std::promise<void> connected);
        ~CandleSpotMarketWS() override;

    public:
        void initialize(BinanceCS *obj, CENTAUR_INTERFACE_NAMESPACE::ILogger *logger);
        void connected() override;
        void close() override;
        void connectionError() override;
        void pingSent(bool success) noexcept override;

    protected:
        void subscribe(bool status, int id) override;
        void unsubscribe(bool status, int id) override;
        void kline(const std::string &symbol, quint64 eventTime, BINAPI_NAMESPACE::BinanceTimeIntervals interval, const BINAPI_NAMESPACE::Candlestick &cs) override;

    private:
        CENTAUR_INTERFACE_NAMESPACE::ILogger *m_logger { nullptr };
        BinanceCS *m_obj { nullptr };
        std::promise<void> m_connected;
    };

} // namespace CENTAUR_PLUGIN_NAMESPACE

Q_DECLARE_METATYPE(binapi::BinanceTimeIntervals)
Q_DECLARE_METATYPE(binapi::Candlestick)

// Helper macros
#define logInfo(x, y) \
    m_logger->info(x, y)

#define logWarn(x, y) \
    m_logger->warning(x, y)

#if !defined(NDEBUG)
#define logTrace(x, y) \
    m_logger->trace(x, y)
#define logDebug(x, y) \
    m_logger->debug(x, y)
#else
#define logTrace(x, y) \
    ((void *)0)
#define logDebug(x, y) \
    ((void *)0)
#endif /**/

#define logError(x, y) \
    m_logger->error(x, y)

#define logFatal(x, y) \
    m_logger->fatal(x, y)

#endif // CENTAUR_BINANCECS_HPP
