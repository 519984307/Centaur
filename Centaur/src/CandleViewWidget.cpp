/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 28/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleViewWidget.hpp"
#include "CandleChartScene.hpp"
#include "CandleItem.hpp"
#include "CentaurApp.hpp"
#include <QCloseEvent>
#include <QSettings>

cen::CandleViewWidget::CandleViewTimeFrameToolBarActions::CandleViewTimeFrameToolBarActions(QObject *parent) :
    aSeconds_1 { new QAction(QCoreApplication::tr("1 second"), parent) },
    aSeconds_5 { new QAction(QCoreApplication::tr(" 5 seconds"), parent) },
    aSeconds_10 { new QAction(QCoreApplication::tr("10 seconds"), parent) },
    aSeconds_30 { new QAction(QCoreApplication::tr("30 seconds"), parent) },
    aSeconds_45 { new QAction(QCoreApplication::tr("45 seconds"), parent) },
    aMinutes_1 { new QAction(QCoreApplication::tr("1 minute"), parent) },
    aMinutes_2 { new QAction(QCoreApplication::tr("2 minutes"), parent) },
    aMinutes_3 { new QAction(QCoreApplication::tr("3 minutes"), parent) },
    aMinutes_5 { new QAction(QCoreApplication::tr("5 minutes"), parent) },
    aMinutes_10 { new QAction(QCoreApplication::tr("10 minutes"), parent) },
    aMinutes_15 { new QAction(QCoreApplication::tr("15 minutes"), parent) },
    aMinutes_30 { new QAction(QCoreApplication::tr("30 minutes"), parent) },
    aMinutes_45 { new QAction(QCoreApplication::tr("45 minutes"), parent) },
    aHours_1 { new QAction(QCoreApplication::tr("1 hour"), parent) },
    aHours_2 { new QAction(QCoreApplication::tr("2 hours"), parent) },
    aHours_4 { new QAction(QCoreApplication::tr("4 hours"), parent) },
    aHours_6 { new QAction(QCoreApplication::tr("6 hours"), parent) },
    aHours_8 { new QAction(QCoreApplication::tr("8 hours"), parent) },
    aHours_12 { new QAction(QCoreApplication::tr("12 hours"), parent) },
    aDays_1 { new QAction(QCoreApplication::tr("1 day"), parent) },
    aDays_3 { new QAction(QCoreApplication::tr("3 days"), parent) },
    aWeeks_1 { new QAction(QCoreApplication::tr("1 week"), parent) },
    aMonths_1 { new QAction(QCoreApplication::tr("1 month"), parent) }

{
}

cen::CandleViewWidget::CandleViewWidget(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &emitter, const uuid &id, const QString &symbol, cen::plugin::TimeFrame tf, QWidget *parent) :
    QWidget(parent),
    m_uuid { id },
    m_symbol { symbol },
    m_tf { tf },
    m_pi { emitter },
    m_ui { new Ui::CandleViewWidget }
{
    m_ui->setupUi(this);

    // m_ui->graphicsView->setEnabled(true);
    setWindowTitle(symbol);

    connect(this, &CandleViewWidget::snRetrieveCandles, this, &CandleViewWidget::onRetrieveCandles);
    connect(this, &CandleViewWidget::snUpdateSeries, this, &CandleViewWidget::onUpdateSeries);

    /*
    if (view->realtimePlotAllowed())
    {
        initToolBar();
    }                                          */

    initChart();

    m_ui->graphicsView->setChartTimeFrame(tf);
    // Inform the plugin that the user wants to start acquiring the data

    /// view->acquire(emitter, symbol, tf, m_uuid);

    // Load the last window of times for the specific timeframe and symbol
    loadLastTimeWindow();
    if (m_candleWindow.begin == 0 && m_candleWindow.end == 0)
    {
        // Get basic begin and end timestamps
        m_candleWindow = CandleViewWidget::getClosedCandlesTimes(m_tf);
    }

    // Acquire the candles from the interface
    emit snRetrieveCandles(m_candleWindow.begin, m_candleWindow.end);

    connect(m_ui->graphicsView, &CandleChartWidget::snUpdateCandleMousePosition, this, &CandleViewWidget::onUpdateCandleMousePosition);
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
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime: m_toolbar->addSeparator(); break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_1);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_5:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_5);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_5, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_10:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_10);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_10, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_30:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_30);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_30, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_45:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aSeconds_45);
                connect(m_candleViewTimeFrameToolBarActions->aSeconds_45, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_1);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_2:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_2);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_2, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_3:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_3);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_3, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_5:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_5);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_5, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_10:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_10);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_10, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_15:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_15);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_15, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_30:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_30);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_30, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_45:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMinutes_45);
                connect(m_candleViewTimeFrameToolBarActions->aMinutes_45, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_1);
                connect(m_candleViewTimeFrameToolBarActions->aHours_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_2:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_2);
                connect(m_candleViewTimeFrameToolBarActions->aHours_2, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_4:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_4);
                connect(m_candleViewTimeFrameToolBarActions->aHours_4, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_6:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_6);
                connect(m_candleViewTimeFrameToolBarActions->aHours_6, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_8:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_8);
                connect(m_candleViewTimeFrameToolBarActions->aHours_8, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_12:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aHours_12);
                connect(m_candleViewTimeFrameToolBarActions->aHours_12, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Days_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aDays_1);
                connect(m_candleViewTimeFrameToolBarActions->aDays_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Days_3:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aDays_3);
                connect(m_candleViewTimeFrameToolBarActions->aDays_3, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Weeks_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aWeeks_1);
                connect(m_candleViewTimeFrameToolBarActions->aWeeks_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
            case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Months_1:
                m_toolbar->addAction(m_candleViewTimeFrameToolBarActions->aMonths_1);
                connect(m_candleViewTimeFrameToolBarActions->aMonths_1, &QAction::triggered, this, [&]() { m_view->reframe(stf); });
                break;
        }
    }
}

QString cen::CandleViewWidget::buildSettingsGroupName(const QString &symbol, const QString &pluginName, cen::plugin::TimeFrame tf) noexcept
{
    return QString("%1@@%2@@%3!!").arg(symbol, pluginName, timeFrameToString(tf));
}

QString cen::CandleViewWidget::timeFrameToString(cen::plugin::TimeFrame tf) noexcept
{
    switch (tf)
    {
        case plugin::TimeFrame::nullTime: return "0";
        case plugin::TimeFrame::Seconds_1: return "1s";
        case plugin::TimeFrame::Seconds_5: return "5s";
        case plugin::TimeFrame::Seconds_10: return "10s";
        case plugin::TimeFrame::Seconds_30: return "30s";
        case plugin::TimeFrame::Seconds_45: return "45s";
        case plugin::TimeFrame::Minutes_1: return "1m";
        case plugin::TimeFrame::Minutes_2: return "2m";
        case plugin::TimeFrame::Minutes_3: return "3m";
        case plugin::TimeFrame::Minutes_5: return "5m";
        case plugin::TimeFrame::Minutes_10: return "10m";
        case plugin::TimeFrame::Minutes_15: return "15m";
        case plugin::TimeFrame::Minutes_30: return "30m";
        case plugin::TimeFrame::Minutes_45: return "45m";
        case plugin::TimeFrame::Hours_1: return "1H";
        case plugin::TimeFrame::Hours_2: return "2H";
        case plugin::TimeFrame::Hours_4: return "4H";
        case plugin::TimeFrame::Hours_6: return "6H";
        case plugin::TimeFrame::Hours_8: return "8H";
        case plugin::TimeFrame::Hours_12: return "12H";
        case plugin::TimeFrame::Days_1: return "1H";
        case plugin::TimeFrame::Days_3: return "3H";
        case plugin::TimeFrame::Weeks_1: return "1W";
        case plugin::TimeFrame::Months_1: return "1M";
    }
}
CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp cen::CandleViewWidget::timeFrameToMilliseconds(cen::plugin::TimeFrame tf) noexcept
{
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp sec   = 1000;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp min   = sec * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp hr    = min * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp day   = hr * 24;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp week  = day * 7;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp month = day * 365 / 12;

    switch (tf)
    {
        case plugin::TimeFrame::nullTime: return 0;
        case plugin::TimeFrame::Seconds_1: return sec;
        case plugin::TimeFrame::Seconds_5: return sec * 5;
        case plugin::TimeFrame::Seconds_10: return sec * 10;
        case plugin::TimeFrame::Seconds_30: return sec * 30;
        case plugin::TimeFrame::Seconds_45: return sec * 45;
        case plugin::TimeFrame::Minutes_1: return min;
        case plugin::TimeFrame::Minutes_2: return min * 2;
        case plugin::TimeFrame::Minutes_3: return min * 3;
        case plugin::TimeFrame::Minutes_5: return min * 5;
        case plugin::TimeFrame::Minutes_10: return min * 10;
        case plugin::TimeFrame::Minutes_15: return min * 15;
        case plugin::TimeFrame::Minutes_30: return min * 30;
        case plugin::TimeFrame::Minutes_45: return min * 45;
        case plugin::TimeFrame::Hours_1: return hr;
        case plugin::TimeFrame::Hours_2: return hr * 2;
        case plugin::TimeFrame::Hours_4: return hr * 4;
        case plugin::TimeFrame::Hours_6: return hr * 6;
        case plugin::TimeFrame::Hours_8: return hr * 8;
        case plugin::TimeFrame::Hours_12: return hr * 12;
        case plugin::TimeFrame::Days_1: return day;
        case plugin::TimeFrame::Days_3: return day * 3;
        case plugin::TimeFrame::Weeks_1: return week;
        case plugin::TimeFrame::Months_1: return month;
    }
}

cen::CandleViewWidget::CandleWindow cen::CandleViewWidget::getClosedCandlesTimes(cen::plugin::TimeFrame tf) noexcept
{
    const auto end   = static_cast<CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto ms    = CandleViewWidget::timeFrameToMilliseconds(tf);
    auto times       = 0ull;
    const auto t_end = end - (end % ms);

    if (ms <= 2'700'000)                          // less than 45 minutes
        times = 50ull;                            // 200 candles at the beginning
    else if (ms > 2'700'000 && ms <= 86'400'000)  // For hours
        times = 150ull;                           // 150 hour candles
    else if (ms > 86'400'000 && ms < 345'600'000) // For up to 4 weeks
        times = 26;                               // display last half year in weeks candles
    else
        times = 12; // just display las 12 months

    const auto t_begin = t_end - ms * times;

    //return { t_begin, t_end };
    return {0,0};
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
    // m_view->disengage(m_uuid, m_candleWindow.begin, m_candleWindow.end);
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

void cen::CandleViewWidget::onRetrieveCandles(cen::plugin::IExchange::Timestamp start, cen::plugin::IExchange::Timestamp end) noexcept
{
    auto data            = m_view->getCandlesByPeriod(m_symbol, start, end, m_tf);
    m_candleWindow.begin = start;
    m_candleWindow.end   = end;

    parentWidget()->setMaximumSize(500, 500);

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

    parentWidget()->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    //  m_ui->graphicsView->setPriceMinMax(min, max);
    //  m_ui->graphicsView->setTimeMinMax(start, end);

    // emit snUpdateSeries();
}

void cen::CandleViewWidget::initChart() noexcept
{
}

void cen::CandleViewWidget::onUpdateSeries() noexcept
{
}

void cen::CandleViewWidget::onUpdateCandleMousePosition(uint64_t timestamp)
{
   /* auto candle = m_ui->graphicsView->getCandleItem(timestamp);

    const QString timeFormat = [&]() {
        const uint64_t candleTimeframe = timeFrameToMilliseconds(m_tf);
        if (candleTimeframe < 3'600'000) // 3,600,000 is the number of milliseconds in a minute, thus, if the time is less than a minute we must include the seconds on the label
            return "dd-MM-yyyy HH:mm:ss";
        else
            return "dd-MM-yyyy HH:mm";
    }();

    // const auto &time       = m_candleRects.at(m_cursorIndex);
    QDateTime dt      = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(timestamp));
    QString infoLabel = dt.toString(timeFormat);

    m_ui->labelTime->setText(infoLabel);
    if (candle == nullptr)
    {
        m_ui->labelClose->setText({});
        m_ui->labelOpen->setText({});
        m_ui->labelHigh->setText({});
        m_ui->labelLow->setText({});
    }
    else
    {
        auto setLabelColor = [](QLabel *label, const QColor &color) {
            QPalette palette = label->palette();
            palette.setColor(QPalette::ColorRole::WindowText, color);
            label->setPalette(palette);
        };

        QColor color = candle->isCandleBullish() ? QColor { 0, 255, 0 } : QColor { 255, 0, 0 };

        setLabelColor(m_ui->labelClose, color);
        setLabelColor(m_ui->labelOpen, color);
        setLabelColor(m_ui->labelHigh, color);
        setLabelColor(m_ui->labelLow, color);

        int precision = m_ui->graphicsView->getPricePrecision();

        m_ui->labelClose->setText(QString("%1").arg(QLocale(QLocale::English).toString(candle->close(), 'f', precision)));
        m_ui->labelOpen->setText(QString("%1").arg(QLocale(QLocale::English).toString(candle->open(), 'f', precision)));
        m_ui->labelHigh->setText(QString("%1").arg(QLocale(QLocale::English).toString(candle->high(), 'f', precision)));
        m_ui->labelLow->setText(QString("%1").arg(QLocale(QLocale::English).toString(candle->low(), 'f', precision)));
    }*/
}

void cen::CandleViewWidget::updateLatency(quint64 event) noexcept
{
    // Calculate latency
    const auto ms      = static_cast<quint64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto latency = event > ms ? 0ull : ms - event;
    /*
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
        }*/
}

void cen::CandleViewWidget::onUpdateCandle(quint64 eventTime, cen::plugin::IExchange::Timestamp ts, const cen::plugin::CandleData &cd) noexcept
{
}
