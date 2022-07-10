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
    m_priceAxis { new PriceAxisItem(0, 0, 10., 0, 0) },
    m_timeAxis { new TimeAxisItem(18.0, 5.0, 2.0, 50, 0, 0, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::nullTime) },
    m_horzGridLinesGroup { new QGraphicsItemGroup },
    m_vertGridLinesGroup { new QGraphicsItemGroup },
    m_showHorzGridLines { true },
    m_showVertGridLines { true },
    m_pricePrecision { 0 }
{
    assert(m_scene != nullptr);
    assert(m_priceAxis != nullptr);

    setScene(m_scene);

    fixSize(viewport()->width(), viewport()->height());

    m_scene->addItem(m_priceAxis);
    m_scene->addItem(m_timeAxis);
    m_scene->addItem(m_horzGridLinesGroup);
    m_scene->addItem(m_vertGridLinesGroup);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMouseTracking(true);

    connect(this, &CandleChartWidget::snEnterEvent, m_scene, &CandleChartScene::onMouseEnter);
    connect(this, &CandleChartWidget::snLeaveEvent, m_scene, &CandleChartScene::onMouseLeave);

    updateAxisRect(rect().width(), rect().height());
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

    updateAxisRect(size.width(), size.height());

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

int cen::CandleChartWidget::getPriceMaxFloatingPrecisionPoint(double price)
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
    while (static_cast<int64_t>(price *= 10) == 0 && significant <= maxDecimals) ++significant;
    significant = std::min(maxDecimals, significant + 3);

    return significant;
}

QSizeF cen::CandleChartWidget::calculateDoubleToStringMetrics(double val, int precision) noexcept
{
    QFontMetrics metrics(QFont("Roboto", 12));
    const QString text = QString("%1").arg(QLocale(QLocale::English).toString(val, 'f', precision));

    //
    auto size = metrics.size(Qt::TextSingleLine, text);

    // qDebug() << text << size.width();

    return { static_cast<double>(size.width()), static_cast<double>(size.height()) };
}

void cen::CandleChartWidget::setLowestLowAndPriceFPPrecision(double low) noexcept
{
    // Calculate the axis precision based on the lowest low
    if (!m_priceMinMax.lowestLowSet)
    {
        m_priceMinMax.lowestLow    = low;
        m_priceMinMax.lowestLowSet = true;
    }
    else
        m_priceMinMax.lowestLow = std::min(low, m_priceMinMax.lowestLow);
    m_pricePrecision = getPriceMaxFloatingPrecisionPoint(m_priceMinMax.lowestLow);
}

void cen::CandleChartWidget::addCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{

    auto iter = m_candles.find(timestamp);
    if (iter == m_candles.end())
    {
        // Calculate the axis precision based on the lowest low
        setLowestLowAndPriceFPPrecision(low);
        auto candle = new CandleItem(timestamp, open, close, high, low);
        m_scene->addItem(candle);
        m_candles.insert(timestamp, candle);
        updatePriceAxisMetrics(open, close, high, low);
        candle->updateRect();
        updateAxisTimestamp(timestamp);
    }
    else
    {
        // Call on update
        updateCandle(timestamp, open, close, high, low);
    }
}

void cen::CandleChartWidget::updateCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{
    // Calculate the axis precision based on the lowest low
    setLowestLowAndPriceFPPrecision(low);

    auto &candle = m_candles[timestamp];
    if (candle == nullptr)
    {
        candle = new CandleItem(timestamp, open, close, high, low);
        m_candles.insert(timestamp, candle);
    }

    candle->setParameters(open, close, high, low);
    updatePriceAxisMetrics(open, close, high, low, true);
    candle->updateRect();
    updateAxisTimestamp(timestamp);
}

void cen::CandleChartWidget::setPriceMinMax(double min, double max) noexcept
{
    m_priceMinMax = { min, max };
    m_priceAxis->setMinMax(min, max); // This function will redraw
}

void cen::CandleChartWidget::setTimeMinMax(uint64_t min, uint64_t max) noexcept
{
    m_timeAxis->setTimestampRange(min, max);
}

void cen::CandleChartWidget::setChartTimeFrame(cen::plugin::ICandleView::TimeFrame tf)
{
    m_timeAxis->setTimeFrame(tf); // SetTimeFrame redraws the scene
}

void cen::CandleChartWidget::updatePriceAxisMetrics(double open, double close, double high, double low, bool updateScene) noexcept
{
    // qDebug() << m_pricePrecision;
    const auto openSize  = CandleChartWidget::calculateDoubleToStringMetrics(open, m_pricePrecision);
    const auto closeSize = CandleChartWidget::calculateDoubleToStringMetrics(close, m_pricePrecision);
    const auto highSize  = CandleChartWidget::calculateDoubleToStringMetrics(high, m_pricePrecision);
    const auto lowSize   = CandleChartWidget::calculateDoubleToStringMetrics(low, m_pricePrecision);

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

void cen::CandleChartWidget::updateAxisRect(int windowWidth, int windowHeight) noexcept
{
    const auto timeAxisHeight = static_cast<int>(m_timeAxis->getHeight());

    const auto origin         = mapToScene(0, 0);
    const auto mapping        = mapToScene(windowWidth - frameWidth(), windowHeight - frameWidth() - timeAxisHeight - viewport()->rect().top());

    m_priceAxis->setRect(QRectF { mapping.x() - m_priceAxisSizeParameters.priceAxisWidth, origin.y(), m_priceAxisSizeParameters.priceAxisWidth, mapping.y() });

    m_timeAxis->setRect(QRectF(origin.x(), mapping.y(), mapping.x() - m_priceAxisSizeParameters.priceAxisWidth, timeAxisHeight));

    m_timeAxis->calculateCandleRects(); // Recalculate
    m_timeAxis->recalculateAxisLabels();

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
    QPen linePen { QColor(200, 200, 200, 85), 1 };
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

void cen::CandleChartWidget::updateVertGridLines() noexcept
{
    if (!m_showVertGridLines)
        return;

    auto lines          = m_timeAxis->getGridLinePositions();
    const auto lineSize = lines.size();

    m_vertGridLinesGroup->hide();

    // Reset all lines
    for (auto &l : m_vertGridLines)
        l->setLine(QLineF {});

    if (m_vertGridLines.size() < lineSize)
    {
        const auto newLines = lineSize - m_vertGridLines.size();
        for (auto i = 0ll; i < newLines; ++i)
        {
            auto line = new QGraphicsLineItem;

            m_vertGridLines.append(line);
            m_vertGridLinesGroup->addToGroup(line);
        }
        assert(lineSize <= m_vertGridLines.size());
    }

    const auto thisHeight = viewport()->rect().height();
    const auto axisHeight = static_cast<int>(m_timeAxis->rect().height());

    // Origin point
    QPen linePen { QColor(200, 200, 200, 85), 1 };
    qreal lastZOrder = 0;
    for (auto idx = 0ll; idx < lines.size(); ++idx)
    {
        const auto lineXPos   = static_cast<int>(lines[idx]);

        const auto origin     = mapToScene(lineXPos, 0);
        const auto finalPoint = mapToScene(lineXPos, thisHeight - axisHeight);

        m_vertGridLines[idx]->setPen(linePen);
        m_vertGridLines[idx]->setLine({ origin, finalPoint });

        lastZOrder = m_vertGridLines[idx]->zValue();
    }

    m_timeAxis->setZValue(lastZOrder + 1);

    m_vertGridLinesGroup->show();
}

void cen::CandleChartWidget::updateAxisTimestamp(uint64_t timestamp) noexcept
{
    uint64_t min = std::min(m_timeAxis->getMin(), timestamp);
    if (min != m_timeAxis->getMin())
        m_timeAxis->setTimestampRange(min, m_timeAxis->getMax());
}

void cen::CandleChartWidget::updateCandleRects()
{
    for (auto &item : m_candles)
    {
        item->updateRect();
    }
    update();
}

cen::CandleItem *cen::CandleChartWidget::getCandleItem(uint64_t timestamp) noexcept
{
    auto iter = m_candles.find(timestamp);
    if (iter == m_candles.end())
        return nullptr;
    return *iter;
}
