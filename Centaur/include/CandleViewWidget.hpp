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

#include "../../Centaur.hpp"
#include "../ui/ui_CandleViewWidget.h"
#include "CandleChart/CandleChartWidget.hpp"
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
            CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp begin;
            CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp end;
        };

    public:
        CandleViewWidget(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &emitter, const uuid &id, const QString &symbol, cen::plugin::ICandleView *view, cen::plugin::ICandleView::TimeFrame tf, QWidget *parent = nullptr);
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
        void snRetrieveCandles(CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp start, CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp end);
        void snUpdateSeries();

    public slots:
        void onRetrieveCandles(CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp start, CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp end) noexcept;
        void onUpdateSeries() noexcept;
        void onUpdateCandle(quint64 eventTime, cen::plugin::ICandleView::Timestamp ts, const cen::plugin::ICandleView::CandleData &cd) noexcept;

    protected:
        void updateLatency(quint64 event) noexcept;

        // Information
    protected:
        uuid m_uuid;
        QString m_symbol;
        cen::plugin::ICandleView *m_view;
        cen::plugin::ICandleView::TimeFrame m_tf;
        CENTAUR_PLUGIN_NAMESPACE::PluginInformation m_pi;

        // General data
    private:
        CandleWindow m_candleWindow;

        // Chart
    protected:
        QList<QPair<CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp, CENTAUR_PLUGIN_NAMESPACE::ICandleView::CandleData>> m_localData;

        // UI
    private:
        QToolBar *m_toolbar { nullptr };
        std::unique_ptr<CandleViewTimeFrameToolBarActions> m_candleViewTimeFrameToolBarActions;
        std::unique_ptr<Ui::CandleViewWidget> m_ui;

    private:
        // Builds a string identifier with: symbol@@pluginName@@timeFrame!!
        static QString buildSettingsGroupName(const QString &symbol, const QString &pluginName, cen::plugin::ICandleView::TimeFrame tf) noexcept;
        static QString timeFrameToString(cen::plugin::ICandleView::TimeFrame tf) noexcept;
        // Converts the timeframes to ms. For example: 1 seconds = 1000 ms; 60 min = 360,000 ms
        static CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp timeFrameToMilliseconds(cen::plugin::ICandleView::TimeFrame tf) noexcept;
        /// \brief Calculate the beginning and the end based on current timestamp
        /// \return [begin, end] timestamps
        static CandleWindow getClosedCandlesTimes(cen::plugin::ICandleView::TimeFrame tf) noexcept;
        static QPair<double, double> calculateMinMaxVerticalAxis(double highestHigh, double lowestLow) noexcept;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLEVIEWWIDGET_HPP
