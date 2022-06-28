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
#include "CentaurPlugin.hpp"
#include "Globals.hpp"

#include <QCandlestickSeries>
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

    public:
        CandleViewWidget(const QString &symbol, cen::plugin::ICandleView *view, cen::plugin::ICandleView::TimeFrame tf, QWidget *parent = nullptr);
        ~CandleViewWidget() override = default;

    protected:
        void initToolBar() noexcept;
        void initChart() noexcept;

    public slots:
        /// void updateCandle();

    protected:
        QString m_symbol;
        cen::plugin::ICandleView *m_view;
        cen::plugin::ICandleView::TimeFrame m_tf;

    private:
        QToolBar *m_toolbar { nullptr };
        std::unique_ptr<CandleViewTimeFrameToolBarActions> m_candleViewTimeFrameToolBarActions;
        std::unique_ptr<Ui::CandleViewWidget> m_ui;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLEVIEWWIDGET_HPP
