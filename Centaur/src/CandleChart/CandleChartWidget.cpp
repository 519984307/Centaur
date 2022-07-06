/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChart/CandleChartWidget.hpp"
#include "CandleChart/CandleChartScene.hpp"

#include <QLocale>
#include <QResizeEvent>

#include <limits>

cen::CandleChartWidget::CandleChartWidget(QWidget *parent) :
    QGraphicsView(parent),
    m_scene { new CandleChartScene(this) },
    m_priceAxis { new PriceAxisItem(0, 0, 0, 0) },
    m_timeAxis { new TimeAxisItem(25.0, 15.0, 5.0, 0, 0, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::nullTime) },
    m_horzGridLinesGroup { new QGraphicsItemGroup },
    m_showHorzGridLines { true },
    m_showVertGridLines { true }
{
    assert(m_scene != nullptr);
    assert(m_priceAxis != nullptr);

    setScene(m_scene);

    fixSize(viewport()->width(), viewport()->height());

    m_scene->addItem(m_priceAxis);
    m_scene->addItem(m_timeAxis);
    m_scene->addItem(m_horzGridLinesGroup);



    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMouseTracking(true);

    connect(this, &CandleChartWidget::snEnterEvent, m_scene, &CandleChartScene::onMouseEnter);
    connect(this, &CandleChartWidget::snLeaveEvent, m_scene, &CandleChartScene::onMouseLeave);

    updatePriceAxisRect(rect().width(), rect().height());
    m_scene->setPriceAxisItem(m_priceAxis);
    m_scene->setTimeAxisItem(m_timeAxis);
}

void cen::CandleChartWidget::enterEvent(QEnterEvent *event)
{
    emit snEnterEvent(event);
    QWidget::enterEvent(event);
}

void cen::CandleChartWidget::leaveEvent(QEvent *event)
{
    emit snLeaveEvent(event);
    QWidget::leaveEvent(event);
}

void cen::CandleChartWidget::resizeEvent(QResizeEvent *event)
{
    const QSize size = event->size();
    fixSize(size.width(), size.height());

    updatePriceAxisRect(size.width(), size.height());

    QGraphicsView::resizeEvent(event);
}

void cen::CandleChartWidget::fixSize(int width, int height)
{
    // qDebug() << viewport()->width() << width;
    //  setFixedSize(width, height);

    setSceneRect(0, 0, width, height);
    // fitInView(0, 0, width, height, Qt::KeepAspectRatio);
}

double cen::CandleChartWidget::scaleVerticalPoint(double pt, double min, double max, double maxHeight) noexcept
{
    return maxHeight * ((pt - min) / (max - min));
}

double cen::CandleChartWidget::scaleHorizontalPoint(double pt, double min, double max, double maxWidth) noexcept
{
    return maxWidth * ((pt - min) / (max - min));
}

QSizeF cen::CandleChartWidget::calculateDoubleToStringMetrics(double val) noexcept
{
    constexpr int maxDecimals = 8; // Maximum number of decimals

    /// This function calculates where is the first significant decimal and then add two more decimals to be displayed
    /// For example
    ///     0.001
    ///     -> 0.001*10 = 0.01 to int -> 0 thus ++significant;
    ///     -> 0.01*10  = 0.1 to int -> 0 thus ++significant
    ///     -> 0.1*10   = 1 to int -> 1 thus significant = 2 then significant = min(significant + 3, maxDecimals)
    /// To string: "0.001000"

    int significant = 0;
    double v        = val;
    while (static_cast<int64_t>(v *= 10) == 0 && significant <= maxDecimals) ++significant;

    significant = std::min(maxDecimals, significant + 3);

    QFontMetrics metrics(QFont("Roboto", 12));

    const QString text = QString("%1").arg(QLocale(QLocale::English).toString(val, 'f', significant));

    //
    auto size = metrics.size(Qt::TextSingleLine, text);

    // qDebug() << text << size.width();

    return { static_cast<double>(size.width()), static_cast<double>(size.height()) };
}

void cen::CandleChartWidget::addCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{
    auto iter = m_candles.find(timestamp);
    if (iter == m_candles.end())
    {
        // m_candles.insert(timestamp, new CandleItem(open, close, high, low));

        updatePriceAxisMetrics(open, close, high, low);
    }
    else
    {
        // Call on update
        updateCandle(timestamp, open, close, high, low);
    }
}

void cen::CandleChartWidget::updateCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{
    auto &candle = m_candles[timestamp];
    if (candle == nullptr)
    {
        //  candle = new CandleItem(open, close, high, low);
        //    m_candles.insert(timestamp, candle);
    }

    candle->setParameters(open, close, high, low);
    updatePriceAxisMetrics(open, close, high, low, true);
}

void cen::CandleChartWidget::setPriceMinMax(double min, double max) noexcept
{
    m_priceMinMax = { min, max };
    m_priceAxis->setMinMax(min, max); // This function will redraw
}

void cen::CandleChartWidget::updatePriceAxisMetrics(double open, double close, double high, double low, bool updateScene) noexcept
{
    const auto openSize  = CandleChartWidget::calculateDoubleToStringMetrics(open);
    const auto closeSize = CandleChartWidget::calculateDoubleToStringMetrics(close);
    const auto highSize  = CandleChartWidget::calculateDoubleToStringMetrics(high);
    const auto lowSize   = CandleChartWidget::calculateDoubleToStringMetrics(low);

    //
    double newWidth   = std::max({ m_priceAxisSizeParameters.priceAxisWidth, highSize.width(), lowSize.width(), openSize.width() });
    double newHeight  = std::max({ m_priceAxisSizeParameters.priceAxisHeight, highSize.height(), lowSize.height(), closeSize.height(), openSize.height() });

    bool shouldUpdate = false;

    auto equalDouble  = [](double a, double b) -> bool {
        double relative = std::max({ 1.0, std::fabs(a), std::fabs(b) });
        return std::fabs(a - b) <= std::numeric_limits<double>::epsilon() * relative;
    };

    if (!equalDouble(newWidth, m_priceAxisSizeParameters.priceAxisWidth))
    {
        m_priceAxisSizeParameters.priceAxisWidth = newWidth;
        m_priceAxis->setWidth(m_priceAxisSizeParameters.priceAxisWidth);
        shouldUpdate = true;
    }

    if (!equalDouble(newHeight, m_priceAxisSizeParameters.priceAxisHeight))
    {
        m_priceAxisSizeParameters.priceAxisHeight = newHeight;
        m_priceAxis->setMaxLabelHeight(m_priceAxisSizeParameters.priceAxisHeight);
        shouldUpdate = true;
    }

    if (updateScene && shouldUpdate)
        update();
}

void cen::CandleChartWidget::showHorzGridLines(bool show) noexcept
{
    m_showHorzGridLines = show;
    update();
}

void cen::CandleChartWidget::showVertGridLines(bool show) noexcept
{
    m_showVertGridLines = show;
    update();
}

void cen::CandleChartWidget::updatePriceAxisRect(int windowWidth, int windowHeight) noexcept
{

    const auto timeAxisHeight = static_cast<int>(m_timeAxis->getHeight());

    const auto origin         = mapToScene(0, 0);
    const auto mapping        = mapToScene(windowWidth - frameWidth(), windowHeight - frameWidth() - timeAxisHeight);

    m_priceAxis->setRect(QRectF { mapping.x() - m_priceAxisSizeParameters.priceAxisWidth, origin.y(), m_priceAxisSizeParameters.priceAxisWidth, mapping.y() });

    m_timeAxis->setRect(QRectF(origin.x(), mapping.y(), mapping.x() - m_priceAxisSizeParameters.priceAxisWidth, timeAxisHeight));

    m_timeAxis->calculateCandleRects(); // Recalculate

    updateHorzGridLines();
    updateVertGridLines();
}

void cen::CandleChartWidget::updateHorzGridLines() noexcept
{
    if (!m_showHorzGridLines)
        return;

    auto lines          = m_priceAxis->getGridLinePositions();
    const auto lineSize = lines.size();

    m_horzGridLinesGroup->hide();

    // Reset all lines
    for (auto &l : m_horzGridLines)
        l->setLine(QLineF {});

    if (m_horzGridLines.size() < lineSize)
    {
        const auto newLines = lineSize - m_horzGridLines.size();
        for (auto i = 0ll; i < newLines; ++i)
        {
            auto line = new QGraphicsLineItem;

            m_horzGridLines.append(line);
            m_horzGridLinesGroup->addToGroup(line);
        }
        assert(lineSize <= m_horzGridLines.size());
    }

    const auto thisWidth = viewport()->width();
    const auto axisWidth = static_cast<int>(m_priceAxis->getAxisWidth());

    // Origin point
    QPen linePen { QColor(200, 200, 200, 200), 1 };
    qreal lastZOrder = 0;
    for (auto idx = 0ll; idx < lines.size(); ++idx)
    {
        const auto lineYPos   = static_cast<int>(lines[idx]);

        const auto origin     = mapToScene(0, lineYPos);
        const auto finalPoint = mapToScene(thisWidth - axisWidth, lineYPos);

        m_horzGridLines[idx]->setPen(linePen);
        m_horzGridLines[idx]->setLine({ origin, finalPoint });

        lastZOrder = m_horzGridLines[idx]->zValue();
    }

    m_priceAxis->setZValue(lastZOrder + 1);

    m_horzGridLinesGroup->show();
}

void cen::CandleChartWidget::setChartTimeFrame(cen::plugin::ICandleView::TimeFrame tf)
{
    m_timeAxis->setTimeFrame(tf); // SetTimeFrame redraws the scene
}

void cen::CandleChartWidget::updateVertGridLines() noexcept
{
}
