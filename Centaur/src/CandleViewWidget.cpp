/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 28/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleViewWidget.hpp"
#include "CentaurApp.hpp"
#include <QCloseEvent>
#include <QSettings>

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

cen::CandleViewWidget::CandleViewWidget(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &emitter, const uuid &id, const QString &symbol, cen::plugin::ICandleView *view, cen::plugin::ICandleView::TimeFrame tf, QWidget *parent) :
    QWidget(parent),
    m_uuid { id },
    m_symbol { symbol },
    m_view { view },
    m_tf { tf },
    m_pi { emitter },
    m_ui { new Ui::CandleViewWidget }
{
    m_ui->setupUi(this);
    setWindowTitle(symbol);

    connect(this, &CandleViewWidget::snRetrieveCandles, this, &CandleViewWidget::onRetrieveCandles);
    connect(this, &CandleViewWidget::snUpdateSeries, this, &CandleViewWidget::onUpdateSeries);

    if (view->realtimePlotAllowed())
    {
        initToolBar();
    }

    initChart();

    m_ui->graphicsView->setChartTimeFrame(tf);
    // Inform the plugin that the user wants to start acquiring the data
    view->acquire(emitter, symbol, tf, m_uuid);

    // Load the last window of times for the specific timeframe and symbol
    loadLastTimeWindow();
    if (m_candleWindow.begin == 0 && m_candleWindow.end == 0)
    {
        // Get basic begin and end timestamps
        m_candleWindow = CandleViewWidget::getClosedCandlesTimes(m_tf);
    }

    // Acquire the candles from the interface
    emit snRetrieveCandles(m_candleWindow.begin, m_candleWindow.end);
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

QString cen::CandleViewWidget::buildSettingsGroupName(const QString &symbol, const QString &pluginName, cen::plugin::ICandleView::TimeFrame tf) noexcept
{
    return QString("%1@@%2@@%3!!").arg(symbol, pluginName, timeFrameToString(tf));
}

QString cen::CandleViewWidget::timeFrameToString(cen::plugin::ICandleView::TimeFrame tf) noexcept
{
    switch (tf)
    {
        case plugin::ICandleView::TimeFrame::nullTime: return "0";
        case plugin::ICandleView::TimeFrame::Seconds_1: return "1s";
        case plugin::ICandleView::TimeFrame::Seconds_5: return "5s";
        case plugin::ICandleView::TimeFrame::Seconds_10: return "10s";
        case plugin::ICandleView::TimeFrame::Seconds_30: return "30s";
        case plugin::ICandleView::TimeFrame::Seconds_45: return "45s";
        case plugin::ICandleView::TimeFrame::Minutes_1: return "1m";
        case plugin::ICandleView::TimeFrame::Minutes_2: return "2m";
        case plugin::ICandleView::TimeFrame::Minutes_3: return "3m";
        case plugin::ICandleView::TimeFrame::Minutes_5: return "5m";
        case plugin::ICandleView::TimeFrame::Minutes_10: return "10m";
        case plugin::ICandleView::TimeFrame::Minutes_15: return "15m";
        case plugin::ICandleView::TimeFrame::Minutes_30: return "30m";
        case plugin::ICandleView::TimeFrame::Minutes_45: return "45m";
        case plugin::ICandleView::TimeFrame::Hours_1: return "1H";
        case plugin::ICandleView::TimeFrame::Hours_2: return "2H";
        case plugin::ICandleView::TimeFrame::Hours_4: return "4H";
        case plugin::ICandleView::TimeFrame::Hours_6: return "6H";
        case plugin::ICandleView::TimeFrame::Hours_8: return "8H";
        case plugin::ICandleView::TimeFrame::Hours_12: return "12H";
        case plugin::ICandleView::TimeFrame::Days_1: return "1H";
        case plugin::ICandleView::TimeFrame::Days_3: return "3H";
        case plugin::ICandleView::TimeFrame::Weeks_1: return "1W";
        case plugin::ICandleView::TimeFrame::Months_1: return "1M";
    }
}
CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp cen::CandleViewWidget::timeFrameToMilliseconds(cen::plugin::ICandleView::TimeFrame tf) noexcept
{
    constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp sec   = 1000;
    constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp min   = sec * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp hr    = min * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp day   = hr * 24;
    constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp week  = day * 7;
    constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp month = day * 365 / 12;

    switch (tf)
    {
        case plugin::ICandleView::TimeFrame::nullTime: return 0;
        case plugin::ICandleView::TimeFrame::Seconds_1: return sec;
        case plugin::ICandleView::TimeFrame::Seconds_5: return sec * 5;
        case plugin::ICandleView::TimeFrame::Seconds_10: return sec * 10;
        case plugin::ICandleView::TimeFrame::Seconds_30: return sec * 30;
        case plugin::ICandleView::TimeFrame::Seconds_45: return sec * 45;
        case plugin::ICandleView::TimeFrame::Minutes_1: return min;
        case plugin::ICandleView::TimeFrame::Minutes_2: return min * 2;
        case plugin::ICandleView::TimeFrame::Minutes_3: return min * 3;
        case plugin::ICandleView::TimeFrame::Minutes_5: return min * 5;
        case plugin::ICandleView::TimeFrame::Minutes_10: return min * 10;
        case plugin::ICandleView::TimeFrame::Minutes_15: return min * 15;
        case plugin::ICandleView::TimeFrame::Minutes_30: return min * 30;
        case plugin::ICandleView::TimeFrame::Minutes_45: return min * 45;
        case plugin::ICandleView::TimeFrame::Hours_1: return hr;
        case plugin::ICandleView::TimeFrame::Hours_2: return hr * 2;
        case plugin::ICandleView::TimeFrame::Hours_4: return hr * 4;
        case plugin::ICandleView::TimeFrame::Hours_6: return hr * 6;
        case plugin::ICandleView::TimeFrame::Hours_8: return hr * 8;
        case plugin::ICandleView::TimeFrame::Hours_12: return hr * 12;
        case plugin::ICandleView::TimeFrame::Days_1: return day;
        case plugin::ICandleView::TimeFrame::Days_3: return day * 3;
        case plugin::ICandleView::TimeFrame::Weeks_1: return week;
        case plugin::ICandleView::TimeFrame::Months_1: return month;
    }
}

cen::CandleViewWidget::CandleWindow cen::CandleViewWidget::getClosedCandlesTimes(cen::plugin::ICandleView::TimeFrame tf) noexcept
{
    const auto end   = static_cast<CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto ms    = CandleViewWidget::timeFrameToMilliseconds(tf);
    auto times       = 0ull;
    const auto t_end = end - (end % ms);

    if (ms <= 2'700'000)                          // less than 45 minutes
        times = 10ull;                            // 200 candles at the beginning
    else if (ms > 2'700'000 && ms <= 86'400'000)  // For hours
        times = 150ull;                           // 150 hour candles
    else if (ms > 86'400'000 && ms < 345'600'000) // For up to 4 weeks
        times = 26;                               // display last half year in weeks candles
    else
        times = 12; // just display las 12 months

    const auto t_begin = t_end - ms * times;

    return { t_begin, t_end };
}

QPair<double, double> cen::CandleViewWidget::calculateMinMaxVerticalAxis(double highestHigh, double lowestLow) noexcept
{
    const double relation = (highestHigh - lowestLow) / (highestHigh + lowestLow);
    const double minAxis  = (1.0 - relation) * lowestLow;
    const double maxAxis  = (1.0 + relation) * highestHigh;
    return { minAxis, maxAxis };
}

void cen::CandleViewWidget::closeEvent(QCloseEvent *event)
{
    m_view->disengage(m_uuid, m_candleWindow.begin, m_candleWindow.end);
    storeLastTimeWindow();
    event->accept();
}

void cen::CandleViewWidget::storeLastTimeWindow() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup(CandleViewWidget::buildSettingsGroupName(m_symbol, m_pi.name, m_tf));
    settings.setValue("begin", m_candleWindow.begin);
    settings.setValue("end", m_candleWindow.end);
    settings.endGroup();
}

void cen::CandleViewWidget::loadLastTimeWindow() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup(CandleViewWidget::buildSettingsGroupName(m_symbol, m_pi.name, m_tf));
    m_candleWindow.begin = settings.value("begin", 0).toULongLong();
    m_candleWindow.end   = settings.value("end", 0).toULongLong();
    settings.endGroup();
}

void cen::CandleViewWidget::onRetrieveCandles(cen::plugin::ICandleView::Timestamp start, cen::plugin::ICandleView::Timestamp end) noexcept
{
    auto data            = m_view->getCandlesByPeriod(m_symbol, start, end, m_tf);
    m_candleWindow.begin = start;
    m_candleWindow.end   = end;

    double min = 0, max = 0;
    bool minSet = false;
    for (const auto &cd : data)
    {
        // Add the candle
        m_ui->graphicsView->addCandle(cd.first, cd.second.open, cd.second.close, cd.second.high, cd.second.low);

        max = std::max(max, cd.second.high);
        if (!minSet)
        {
            min    = std::min(max, cd.second.low); // Sets the first min as the candle low
            minSet = true;
        }
        else
            min = std::min(min, cd.second.low);
    }
    m_ui->graphicsView->setPriceMinMax(min, max);

    // emit snUpdateSeries();
}

void cen::CandleViewWidget::initChart() noexcept
{
}

void cen::CandleViewWidget::onUpdateSeries() noexcept
{
}

void cen::CandleViewWidget::updateLatency(quint64 event) noexcept
{
    // Calculate latency
    const auto ms      = static_cast<quint64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto latency = event > ms ? 0ull : ms - event;

    QPalette palette   = m_ui->labelLatency->palette();

    if (latency >= g_globals->params.symbolsDockParameters.latencyLowMin && latency <= g_globals->params.symbolsDockParameters.latencyLowMax)
    {
        palette.setColor(QPalette::ColorRole::WindowText, QColor(0, 255, 0));
    }
    else if (latency >= g_globals->params.symbolsDockParameters.latencyMediumMin && latency <= g_globals->params.symbolsDockParameters.latencyMediumMax)
    {
        palette.setColor(QPalette::ColorRole::WindowText, QColor(255, 255, 0));
    }
    else if (latency >= g_globals->params.symbolsDockParameters.latencyHighMin && latency <= g_globals->params.symbolsDockParameters.latencyHighMax)
    {
        palette.setColor(QPalette::ColorRole::WindowText, QColor(255, 0, 0));
    }
}

void cen::CandleViewWidget::onUpdateCandle(quint64 eventTime, cen::plugin::ICandleView::Timestamp ts, const cen::plugin::ICandleView::CandleData &cd) noexcept
{
}
