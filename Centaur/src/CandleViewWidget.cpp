/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 28/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleViewWidget.hpp"

cen::CandleViewWidget::CandleViewTimeFrameToolBarActions::CandleViewTimeFrameToolBarActions(QObject *parent) :
    aSeconds_1 { new QAction(LS("ui-docs-symbols-candleview-1s-tb"), parent) },
    aSeconds_5 { new QAction(LS("ui-docs-symbols-candleview-5s-tb"), parent) },
    aSeconds_10 { new QAction(LS("ui-docs-symbols-candleview-10s-tb"), parent) },
    aSeconds_30 { new QAction(LS("ui-docs-symbols-candleview-30s-tb"), parent) },
    aSeconds_45 { new QAction(LS("ui-docs-symbols-candleview-45s-tb"), parent) },
    aMinutes_1 { new QAction(LS("ui-docs-symbols-candleview-1m-tb"), parent) },
    aMinutes_2 { new QAction(LS("ui-docs-symbols-candleview-2m-tb"), parent) },
    aMinutes_3 { new QAction(LS("ui-docs-symbols-candleview-3m-tb"), parent) },
    aMinutes_5 { new QAction(LS("ui-docs-symbols-candleview-5m-tb"), parent) },
    aMinutes_10 { new QAction(LS("ui-docs-symbols-candleview-10m-tb"), parent) },
    aMinutes_15 { new QAction(LS("ui-docs-symbols-candleview-15m-tb"), parent) },
    aMinutes_30 { new QAction(LS("ui-docs-symbols-candleview-30m-tb"), parent) },
    aMinutes_45 { new QAction(LS("ui-docs-symbols-candleview-45m-tb"), parent) },
    aHours_1 { new QAction(LS("ui-docs-symbols-candleview-1h-tb"), parent) },
    aHours_2 { new QAction(LS("ui-docs-symbols-candleview-2h-tb"), parent) },
    aHours_4 { new QAction(LS("ui-docs-symbols-candleview-4h-tb"), parent) },
    aHours_6 { new QAction(LS("ui-docs-symbols-candleview-6h-tb"), parent) },
    aHours_8 { new QAction(LS("ui-docs-symbols-candleview-8h-tb"), parent) },
    aHours_12 { new QAction(LS("ui-docs-symbols-candleview-12h-tb"), parent) },
    aDays_1 { new QAction(LS("ui-docs-symbols-candleview-1d-tb"), parent) },
    aDays_3 { new QAction(LS("ui-docs-symbols-candleview-3d-tb"), parent) },
    aWeeks_1 { new QAction(LS("ui-docs-symbols-candleview-1w-tb"), parent) },
    aMonths_1 { new QAction(LS("ui-docs-symbols-candleview-1M-tb"), parent) }

{
}

cen::CandleViewWidget::CandleViewWidget(const QString &symbol, cen::plugin::ICandleView *view, cen::plugin::ICandleView::TimeFrame tf, QWidget *parent) :
    QWidget(parent),
    m_view { view },
    m_tf { tf },
    m_ui { new Ui::CandleViewWidget }
{
    m_ui->setupUi(this);
    setWindowTitle(symbol);

    if (view->realtimePlotAllowed())
    {
        initToolBar();
    }
}
void cen::CandleViewWidget::initToolBar() noexcept
{
    m_toolbar = new QToolBar(this);
    m_toolbar->setContentsMargins(0, 0, 0, 0);
    m_ui->verticalLayout->insertWidget(0, m_toolbar);
    
    //
    m_candleViewTimeFrameToolBarActions = std::make_unique<CandleViewTimeFrameToolBarActions>(this);
    for (const auto &stf : m_view->supportedTimeFrames())
    {
        switch (stf)
        {
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::nullTime: m_toolbar->addSeparator(); break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_1);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_5:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_5);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_5, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_10:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_10);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_10, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_30:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_30);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_30, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_45:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_45);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_45, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_1);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_2:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_2);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_2, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_3:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_3);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_3, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_5:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_5);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_5, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_10:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_10);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_10, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_15:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_15);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_15, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_30:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_30);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_30, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_45:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_45);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_45, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_1);
                connect(m_candleViewTimeFrameToolBarActions->aHours_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_2:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_2);
                connect(m_candleViewTimeFrameToolBarActions->aHours_2, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_4:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_4);
                connect(m_candleViewTimeFrameToolBarActions->aHours_4, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_6:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_6);
                connect(m_candleViewTimeFrameToolBarActions->aHours_6, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_8:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_8);
                connect(m_candleViewTimeFrameToolBarActions->aHours_8, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_12:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_12);
                connect(m_candleViewTimeFrameToolBarActions->aHours_12, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aDays_1);
                connect(m_candleViewTimeFrameToolBarActions->aDays_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_3:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aDays_3);
                connect(m_candleViewTimeFrameToolBarActions->aDays_3, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Weeks_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aWeeks_1);
                connect(m_candleViewTimeFrameToolBarActions->aWeeks_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Months_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMonths_1);
                connect(m_candleViewTimeFrameToolBarActions->aMonths_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
        }
    }
}
