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

    struct CandleData
    {
        QString symbol;
        uuid exchange;
        uuid id;
        CENTAUR_PLUGIN_NAMESPACE::ICandleView::CandleData cd;
    };

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
        void setPluginInterfaces(cen::interface::ILogger *logger, cen::interface::IConfiguration *config, cen::interface::ILongOperation *lOper) noexcept override;
        uuid getPluginUUID() noexcept override;
        bool addMenuAction(QAction *action, const uuid &menuId) noexcept override;
        C_NODISCARD QList<PluginInformation> supportedExchanges() noexcept override;
        void acquire(const PluginInformation &pi, const QString &symbol, TimeFrame frame, const uuid &id) noexcept override;
        void disengage(const uuid &id, uint64_t lastTimeframeStart, uint64_t lastTimeframeEnd) noexcept override;
        void resetStoredZoom(const uuid &id) noexcept override;
        QList<TimeFrame> supportedTimeFrames() noexcept override;
        QList<QPair<Timestamp, CandleData>> getCandlesByPeriod(const QString &symbol, Timestamp start, Timestamp end, TimeFrame frame) noexcept override;
        bool realtimePlotAllowed() noexcept override;
        bool dynamicReframePlot() noexcept override;
        QList<QToolBar *> getPluginBasedToolBar() noexcept override;

    signals:
        void snRealTimeCandleUpdate(const uuid &id, Timestamp currentCandle, const CandleData &candle);
        void snRealTimeCandleClose(const uuid &id, Timestamp currentCandle, const CandleData &candle, TimeFrame frame);
        void snReframe(const uuid &id);

    protected:
        QList<PluginInformation> m_supportedExchanges;

    private:
        CENTAUR_INTERFACE_NAMESPACE::ILogger *m_logger { nullptr };
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config { nullptr };
        uuid m_thisUUID;
    };
} // namespace CENTAUR_PLUGIN_NAMESPACE

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
