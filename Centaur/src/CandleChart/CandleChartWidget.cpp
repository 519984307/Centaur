/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChart/CandleChartWidget.hpp"
#include "CandleChart/CandleChartScene.hpp"

#include <QLocale>
#include <QResizeEvent>

#include <QScreen>
#include <QWindow>
#include <limits>

cen::CandleChartWidget::CandleChartWidget(QWidget *parent) :
    QGraphicsView(parent),
    m_scene { new QGraphicsScene(this) },
    m_scalingFactor { 1.0 },
    m_pricePrecision { 0 },
    m_tf { CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime },
    m_showHorzGridLines { true },
    m_showVertGridLines { true },
    m_precisionOverride { false }

{
    assert(m_scene != nullptr);

    setScene(m_scene);

    setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    // m_scene->setBackgroundBrush(QBrush(QColor(85, 85, 85)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    setMouseTracking(true);

    m_crossHairItems.vertical   = new QGraphicsLineItem;
    m_crossHairItems.horizontal = new QGraphicsLineItem;

    getHCrossHair()->setPen(QPen(QBrush(QColor(200, 200, 200)), 1, Qt::PenStyle::DashLine));
    getVCrossHair()->setPen(QPen(QBrush(QColor(200, 200, 200)), 1, Qt::PenStyle::DashLine));

    getHCrossHair()->setZValue(4);
    getVCrossHair()->setZValue(4);

    //   getHCrossHair()->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    //   getVCrossHair()->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    m_scene->setSceneRect(0, 0, 500, 200);

    m_scene->addItem(getHCrossHair());
    m_scene->addItem(getVCrossHair());
}

int cen::CandleChartWidget::getPriceMaxFloatingPrecisionPoint(double price) noexcept
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

void cen::CandleChartWidget::addCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{
    // Call to updateCandle will add the candle.
    updateCandle(timestamp, open, close, high, low);
}

void cen::CandleChartWidget::updateCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{

    auto iter          = m_candles.find(timestamp); // O(1) complexity (supposedly)
    CandleItem *candle = nullptr;
    if (iter == m_candles.end())
    {
        candle = new CandleItem(timestamp, open, close, high, low);
        candle->setZValue(3);
        m_candles.insert(timestamp, candle);
    }
    else
    {
        candle = *iter;
        candle->setParameters(open, close, high, low);
    }
    assert(candle != nullptr);

    if (!m_precisionOverride)
    {
        const auto lowPrecision   = getPriceMaxFloatingPrecisionPoint(low);
        const auto highPrecision  = getPriceMaxFloatingPrecisionPoint(high);
        const auto openPrecision  = getPriceMaxFloatingPrecisionPoint(open);
        const auto closePrecision = getPriceMaxFloatingPrecisionPoint(close);

        m_pricePrecision = std::max({ lowPrecision, highPrecision, openPrecision, closePrecision });
    }

    if (m_candles.size() > 1)
    {
        if (m_originParameters.timeDiff == 0)
            m_originParameters.timeDiff = timeFrameMilliseconds();
    }

    m_scene->addItem(candle);
    setPriceMinMax(low, high, true);
    setTimestampMinMax(timestamp, timestamp, true);
}

void cen::CandleChartWidget::updateItemRectangle(cen::CandleItem *item) noexcept
{
    const double resol    = viewport()->height() /* * m_scalingFactor */;
    const double relation = (getMaxPrice() - getMinPrice()) / (getMaxPrice() + getMinPrice());

    // const QPointF origin

    const double minAxis = getMinPrice() /* * m_scalingFactor */;
    const double maxAxis = getMaxPrice() /* * m_scalingFactor */;

    // Candle positions
    const double itemWidth = getCandleWidth() + getCandleSpacing();
    int64_t positions      = m_originParameters.timestamp > item->timestamp()
                                 ? -(static_cast<int64_t>((m_originParameters.timestamp - item->timestamp()) / m_originParameters.timeDiff))
                                 : static_cast<int64_t>((item->timestamp() - m_originParameters.timestamp) / m_originParameters.timeDiff);

    // qDebug() << positions;

    const double xPosition = static_cast<double>(positions) * itemWidth;

    // We will arrange the items in a rect of 1000 px of height

    const auto priceToPoint = [&](double price) {
        double a = (maxAxis - price) / (maxAxis - minAxis);

        return a * resol;
    };

    // qDebug() << priceToPoint(item->high()) << priceToPoint(item->low());

    // qDebug() << minAxis << maxAxis << minAxis * 1000 << (maxAxis - minAxis) * 1000;

    const auto &sceneRect = scene()->sceneRect();

    const QRectF candleRect = {
        xPosition,
        0,
        itemWidth,
        resol
    };

    // qDebug() << candleRect;

    item->setRect(candleRect);
    item->updateCandleRect(priceToPoint(item->open()), priceToPoint(item->close()), priceToPoint(item->high()), priceToPoint(item->low()));
}

void cen::CandleChartWidget::updateAllItemRectangles() noexcept
{
    for (auto &item : m_candles)
        updateItemRectangle(item);
}

void cen::CandleChartWidget::setPricePrecision(int precision) noexcept
{
    const int currentPrecision = m_pricePrecision;
    if (precision == -1)
    {
        m_pricePrecision    = 0;
        m_precisionOverride = false;
    }
    else
    {
        m_pricePrecision    = precision;
        m_precisionOverride = true;
    }

    if (currentPrecision != m_pricePrecision)
        update();
}

void cen::CandleChartWidget::setCandleParameters(double width, double spacing) noexcept
{
    bool doUpdate { false };
    if (!qFuzzyCompare(width, m_candleDimensions.width))
    {
        m_candleDimensions.width = width;
        doUpdate                 = true;
    }

    if (!qFuzzyCompare(spacing, m_candleDimensions.spacing))
    {
        m_candleDimensions.spacing = spacing;
        doUpdate                   = true;
    }

    if (doUpdate)
    {
        update();
    }
}

void cen::CandleChartWidget::setCandleWidth(double width) noexcept
{
    setCandleParameters(width, getCandleSpacing());
}

void cen::CandleChartWidget::setCandleSpacing(double spacing) noexcept
{
    setCandleParameters(getCandleWidth(), spacing);
}

void cen::CandleChartWidget::setPriceMinMax(double min, double max, bool preventUpdate) noexcept
{
    bool doUpdate = false;
    if (!m_chartParameters.priceMinSet)
    {
        m_chartParameters.priceMin    = min;
        m_chartParameters.priceMinSet = true;
        doUpdate                      = true;
    }
    else
    {
        if (!qFuzzyCompare(min, m_chartParameters.priceMin))
        {
            const auto prevMin         = m_chartParameters.priceMin;
            m_chartParameters.priceMin = std::min(min, m_chartParameters.priceMin);
            if (!qFuzzyCompare(prevMin, m_chartParameters.priceMin))
                doUpdate = true;
        }
    }

    if (!qFuzzyCompare(max, m_chartParameters.priceMax))
    {
        const auto prevMax         = m_chartParameters.priceMax;
        m_chartParameters.priceMax = std::max(max, m_chartParameters.priceMax);
        if (!qFuzzyCompare(m_chartParameters.priceMax, prevMax))
            doUpdate = true;
    }

    if (doUpdate)
    {
        // This must be done for proper candle
        updateAllItemRectangles();
    }

    if (!preventUpdate && doUpdate)
        update();
}

void cen::CandleChartWidget::setPriceMin(double min) noexcept
{
    setPriceMinMax(min, getMaxPrice());
}

void cen::CandleChartWidget::setPriceMax(double max) noexcept
{
    setPriceMinMax(getMinPrice(), max);
}

void cen::CandleChartWidget::setTimestampMin(uint64_t min) noexcept
{
    setTimestampMinMax(min, getMaxTimestamp());
}

void cen::CandleChartWidget::setTimestampMax(uint64_t max) noexcept
{
    setTimestampMinMax(getMinTimestamp(), max);
}

void cen::CandleChartWidget::setTimestampMinMax(uint64_t min, uint64_t max, bool preventUpdate) noexcept
{
    bool doUpdate = false;
    if (!m_chartParameters.minTimestampSet)
    {
        m_chartParameters.minTimestamp    = min;
        m_originParameters.timestamp      = min;
        m_chartParameters.minTimestampSet = true;
        doUpdate                          = true;
    }
    else
    {
        if (m_chartParameters.minTimestampSet != min)
        {
            const auto prevMin             = m_chartParameters.minTimestamp;
            m_chartParameters.minTimestamp = std::min(min, m_chartParameters.minTimestamp);
            if (m_chartParameters.minTimestampSet != prevMin)
                doUpdate = true;
        }
    }

    if (max != m_chartParameters.maxTimestamp)
    {
        const auto prevMax             = m_chartParameters.maxTimestamp;
        m_chartParameters.maxTimestamp = std::max(max, m_chartParameters.maxTimestamp);
        if (m_chartParameters.maxTimestamp != prevMax)
            doUpdate = true;
    }

    if (!preventUpdate && doUpdate)
        update();
}

uint64_t cen::CandleChartWidget::timeFrameMilliseconds() noexcept
{
    /*1 hour	3600 seconds
    1 day	86400 seconds
    1 week	604800 seconds
    1 month (30.44 days) 	2629743 seconds
    1 year (365.24 days) 	 31556926 seconds
    */

    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp sec   = 1000;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp min   = sec * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp hr    = min * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp day   = hr * 24;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp week  = day * 7;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp month = 2'629'743'000;

    switch (m_tf)
    {
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime: return 0;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_1: return sec;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_5: return sec * 5;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_10: return sec * 10;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_30: return sec * 30;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Seconds_45: return sec * 45;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_1: return min;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_2: return min * 2;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_3: return min * 3;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_5: return min * 5;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_10: return min * 10;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_15: return min * 15;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_30: return min * 30;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Minutes_45: return min * 45;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_1: return hr;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_2: return hr * 2;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_4: return hr * 4;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_6: return hr * 6;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_8: return hr * 8;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_12: return hr * 12;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Days_1: return day;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Days_3: return day * 3;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Weeks_1: return week;
        case CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Months_1: return month;
    }
}

void cen::CandleChartWidget::setChartTimeFrame(cen::plugin::TimeFrame tf)
{
    m_tf                        = tf;
    m_originParameters.timeDiff = timeFrameMilliseconds();
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

cen::CandleItem *cen::CandleChartWidget::getCandleItem(uint64_t timestamp) noexcept
{
    auto iter = m_candles.find(timestamp);
    if (iter == m_candles.end())
        return nullptr;
    return *iter;
}

void cen::CandleChartWidget::hoverItem(uint64_t timestamp) noexcept
{
    // updateVCrossHair(timestamp);
    // emit snUpdateCandleMousePosition(timestamp);
}

void cen::CandleChartWidget::resizeEvent(QResizeEvent *event)
{
    setSceneRect(scene()->itemsBoundingRect());
    // setSceneRect(QRectF { QPointF { 0, 0 }, QSizeF { event->size() } });
    // fitInView(sceneRect(), Qt::KeepAspectRatio);
    // setMaximumSize(500, 500);
    updateAllItemRectangles();
    qDebug() << sceneRect();
    QGraphicsView::resizeEvent(event);
}

void cen::CandleChartWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_0)
    {
        // Reset scaling
        m_scalingFactor = 1 / m_scalingFactor;
        scale(m_scalingFactor, m_scalingFactor);
        m_scalingFactor = transform().m11();
    }
    QGraphicsView::keyPressEvent(event);
}

void cen::CandleChartWidget::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "key up";
    QGraphicsView::keyReleaseEvent(event);
}
void cen::CandleChartWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "double click";
    QGraphicsView::mouseDoubleClickEvent(event);
}

void cen::CandleChartWidget::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << event;

    if (event->buttons() & Qt::LeftButton)
    {
        scrollAreaHorizontal(event->position().x());
        scrollAreaVertical(event->position().y());

        m_scrollPosition.prevXPoint = event->position().x();
        m_scrollPosition.prevYPoint = event->position().y();
    }

    updateCrossHair(event->position());

    QGraphicsView::mouseMoveEvent(event);
}
void cen::CandleChartWidget::mousePressEvent(QMouseEvent *event)
{
    m_scrollPosition.prevXPoint = event->position().x();
    m_scrollPosition.prevYPoint = event->position().y();

    QGraphicsView::mousePressEvent(event);
}

void cen::CandleChartWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "release";
    QGraphicsView::mouseReleaseEvent(event);
}

void cen::CandleChartWidget::wheelEvent(QWheelEvent *event)
{
    const double scaling = [&event]() {
        const auto delta = event->angleDelta().y();
        const auto out   = qPow(1.0005, 120.0);
        const auto in    = qPow(1.0005, -120);

        if ((event->isInverted() && delta > 0) || (!event->isInverted() && delta < 0))
            return out;
        else
            return in;
    }();

    scale(scaling, scaling);

    m_scalingFactor = transform().m11();

    centerOn(mapToScene(event->position().toPoint().x(), event->position().toPoint().y()));
    updateCrossHair(event->position());

    // qDebug() << sceneRect();
    QGraphicsView::wheelEvent(event);
}

void cen::CandleChartWidget::scrollAreaHorizontal(double newX) noexcept
{
    auto relative = newX - m_scrollPosition.prevXPoint;
    if (qFuzzyCompare(relative, 0.0))
        return;

    // qDebug() << relative * 10;
    translate(relative, 0);
}

void cen::CandleChartWidget::scrollAreaVertical(double newY) noexcept
{
    auto relative = newY - m_scrollPosition.prevYPoint;
    if (qFuzzyCompare(relative, 0.0))
        return;

    // qDebug() << relative * 10;
    translate(0, relative);
}

void cen::CandleChartWidget::updateCrossHair(const QPointF &pt) noexcept
{
    const QPointF origin = mapToScene(0, 0);
    const QPointF size   = mapToScene(viewport()->width(), viewport()->height());
    const QPointF pts    = mapToScene(pt.toPoint().x(), pt.toPoint().y());

    const QPointF horzOrigin {
        origin.x(),
        pts.y()
    };
    const QPointF horzFinal {
        size.x(),
        pts.y()
    };

    getHCrossHair()->setLine({ horzOrigin, horzFinal });

    const auto width    = getCandleWidth() + getCandleSpacing();
    const auto position = std::floor(pts.x() / width);
    const auto XdW_1    = static_cast<int64_t>(position) * static_cast<int64_t>(m_originParameters.timeDiff);

    const auto timestamp = XdW_1 > 0
                               ? m_originParameters.timestamp + XdW_1
                               : m_originParameters.timestamp - static_cast<uint64_t>(-XdW_1);

    auto candle = getCandleItem(timestamp);
    emit snUpdateCandleMousePosition(timestamp);

    double vertX = 0;
    if (candle != nullptr)
    {
        vertX = candle->rect().left() + width / 2;
    }
    else
    {
        vertX = (position * width) + (width / 2);
    }

    const QPointF vertOrigin {
        vertX,
        origin.y()
    };
    const QPointF vertFinal {
        vertX,
        size.y()
    };

    getVCrossHair()->setLine({ vertOrigin, vertFinal });

    qDebug() << m_candles.size() << position << timestamp;
}

void cen::CandleChartWidget::updateVCrossHair(uint64_t timestamp) noexcept
{
    /*
    auto item = getCandleItem(timestamp);
    if (item == nullptr)
        return;

    const QRectF &itemRect = item->rect();

    const QPointF origin   = mapToScene(0, 0);
    const QPointF size     = mapToScene(viewport()->width(), viewport()->height());
    const double X         = (itemRect.left() + (itemRect.width() / 2));

    //  qDebug() << origin;

    const QPointF vertOrigin {
        X,
        origin.y()
    };
    const QPointF vertFinal {
        X,
        size.y()
    };

    getVCrossHair()->setLine({ vertOrigin, vertFinal });*/
}
