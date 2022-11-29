/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 28/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CANDLEVIEWWIDGET_HPP
#define CENTAUR_CANDLEVIEWWIDGET_HPP

#include "../ui/ui_CandleViewWidget.h"
#include "CandleChartWidget.hpp"
#include "Centaur.hpp"
#include "CentaurPlugin.hpp"
#include "Globals.hpp"

#include <QToolBar>

namespace CENTAUR_NAMESPACE
{
    class CandleViewWidget : public QWidget
    {
        Q_OBJECT
        struct CandleViewTimeFrameToolBarActions
        {
            explicit CandleViewTimeFrameToolBarActions(QObject *parent);

        public:
            QAction *aSeconds_1 { nullptr };
            QAction *aSeconds_5 { nullptr };
            QAction *aSeconds_10 { nullptr };
            QAction *aSeconds_30 { nullptr };
            QAction *aSeconds_45 { nullptr };
            QAction *aMinutes_1 { nullptr };
            QAction *aMinutes_2 { nullptr };
            QAction *aMinutes_3 { nullptr };
            QAction *aMinutes_5 { nullptr };
            QAction *aMinutes_10 { nullptr };
            QAction *aMinutes_15 { nullptr };
            QAction *aMinutes_30 { nullptr };
            QAction *aMinutes_45 { nullptr };
            QAction *aHours_1 { nullptr };
            QAction *aHours_2 { nullptr };
            QAction *aHours_4 { nullptr };
            QAction *aHours_6 { nullptr };
            QAction *aHours_8 { nullptr };
            QAction *aHours_12 { nullptr };
            QAction *aDays_1 { nullptr };
            QAction *aDays_3 { nullptr };
            QAction *aWeeks_1 { nullptr };
            QAction *aMonths_1 { nullptr };
        };

        struct CandleWindow
        {
            CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp begin {};
            CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp end {};
        };

    public:
        CandleViewWidget(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &emitter, const uuid &id, const QString &symbol, cen::plugin::TimeFrame tf, QWidget *parent = nullptr);
        ~CandleViewWidget() override = default;

    protected:
        void closeEvent(QCloseEvent *event) override;

    protected:
        void initToolBar() noexcept;
        void initChart() noexcept;

    protected:
        void storeLastTimeWindow() noexcept;
        void loadLastTimeWindow() noexcept;

    signals:
        void snRetrieveCandles(CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp start, CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp end);
        void snUpdateSeries();

    public slots:
        void onRetrieveCandles(CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp start, CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp end) noexcept;
        void onUpdateSeries() noexcept;
        void onUpdateCandle(quint64 eventTime, CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp ts, const CENTAUR_PLUGIN_NAMESPACE::CandleData &cd) noexcept;
        void onUpdateCandleMousePosition(uint64_t timestamp);

    protected:
        void updateLatency(quint64 event) noexcept;

        // Information
    protected:
        CENTAUR_PLUGIN_NAMESPACE::IExchange *m_view;
        uuid m_uuid;
        QString m_symbol;
        cen::plugin::TimeFrame m_tf;
        CENTAUR_PLUGIN_NAMESPACE::PluginInformation m_pi;

        // General data
    private:
        CandleWindow m_candleWindow;

        // UI
    private:
        QToolBar *m_toolbar { nullptr };
        std::unique_ptr<CandleViewTimeFrameToolBarActions> m_candleViewTimeFrameToolBarActions;
        std::unique_ptr<Ui::CandleViewWidget> m_ui;

    private:
        // Builds a string identifier with: symbol@@pluginName@@timeFrame!!
        static QString buildSettingsGroupName(const QString &symbol, const QString &pluginName, cen::plugin::TimeFrame tf) noexcept;
        static QString timeFrameToString(cen::plugin::TimeFrame tf) noexcept;
        // Converts the timeframes to ms. For example: 1 seconds = 1000 ms; 60 min = 360,000 ms
        static CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp timeFrameToMilliseconds(cen::plugin::TimeFrame tf) noexcept;
        /// \brief Calculate the beginning and the end based on current timestamp
        /// \return [begin, end] timestamps
        static CandleWindow getClosedCandlesTimes(cen::plugin::TimeFrame tf) noexcept;
        static QPair<double, double> calculateMinMaxVerticalAxis(double highestHigh, double lowestLow) noexcept;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLEVIEWWIDGET_HPP
