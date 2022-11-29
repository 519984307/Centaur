/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChartWidget.hpp"
#include "CandleChartScene.hpp"
#include "CandlePriceAxisItem.hpp"

#include <QLocale>
#include <QResizeEvent>

#include <QScreen>
#include <QWindow>

BEGIN_CENTAUR_NAMESPACE

struct CandleChartWidget::Impl
{
    explicit Impl(QWidget *parent) :
        scene { new CandleChartScene(parent) },
        verticalCrosshairLine { new QGraphicsLineItem },
        horizontalCrosshairLine { new QGraphicsLineItem }
    {
        // This will prevent weird behavior when the mouse enter for the first time
        verticalCrosshairLine->setData(Qt::UserRole + 1, true);
        horizontalCrosshairLine->setData(Qt::UserRole + 1, true);

        verticalCrosshairLine->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        horizontalCrosshairLine->setFlag(QGraphicsItem::ItemIgnoresTransformations);

        addCrosshairLines();
    }
    ~Impl() = default;

protected:
    void addCrosshairLines() const noexcept
    {
        horizontalCrosshairLine->setPen(QPen(QBrush(QColor(255, 255, 255)), 1, Qt::PenStyle::DashLine));
        verticalCrosshairLine->setPen(QPen(QBrush(QColor(255, 255, 255)), 1, Qt::PenStyle::DashLine));

        scene->addItem(horizontalCrosshairLine);
        scene->addItem(verticalCrosshairLine);

        horizontalCrosshairLine->setZValue(4);
        verticalCrosshairLine->setZValue(4);
    }

public:
    CandleChartScene *scene;

    // Tracking lines (Crosshair)
public:
    QGraphicsLineItem *verticalCrosshairLine { nullptr };
    QGraphicsLineItem *horizontalCrosshairLine { nullptr };

public:
    QPointF grabPoint;
    QPointF movePoint;

public:
    Qt::Orientation grabbedAxis;

    QGraphicsRectItem *rect { nullptr };

public:
    CENTAUR_PLUGIN_NAMESPACE::TimeFrame chartTimeFrame { CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime };
};

CandleChartWidget::CandleChartWidget(QWidget *parent) :
    QGraphicsView(parent),
    _impl { new CandleChartWidget::Impl(this) }
{
    assert(_impl->scene != nullptr);

    setScene(_impl->scene);

    // Set this rect by default
    _impl->scene->setSceneRect(QRectF(0, 0, 2058, 2058));

    setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    setMouseTracking(true);
}

CandleChartWidget::~CandleChartWidget() = default;

void CandleChartWidget::setChartTimeFrame(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf)
{
    _impl->chartTimeFrame = tf;
    _impl->scene->getTimeAxis()->setTimeFrame(tf);
}

bool CandleChartWidget::isHorizontalLineVisible() const noexcept
{
    return _impl->horizontalCrosshairLine->isVisible();
}

bool CandleChartWidget::isVerticalLineVisible() const noexcept
{
    return _impl->verticalCrosshairLine->isVisible();
}

void CandleChartWidget::onShowHorizontalLine(bool show) noexcept
{
    _impl->horizontalCrosshairLine->setVisible(show);
    // Store the visible status
    // This way if the crosshair is not visible when the mouse leaves. Once the mouse reenters the view, the crosshair will not be shown
    _impl->horizontalCrosshairLine->setData(Qt::UserRole + 1, _impl->horizontalCrosshairLine->isVisible());
}

void CandleChartWidget::onShowVerticalLine(bool show) noexcept
{
    _impl->verticalCrosshairLine->setVisible(show);
    // Store the visible status
    // This way if the crosshair is not visible when the mouse leaves. Once the mouse reenters the view, the crosshair will not be shown
    _impl->verticalCrosshairLine->setData(Qt::UserRole + 1, _impl->verticalCrosshairLine->isVisible());
}

void CandleChartWidget::addCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{
    // Call to updateCandle will add the candle.
    updateCandle(timestamp, open, close, high, low);

    _impl->scene->centerPriceAxisAt(low + (high - low) / 2);

    centerOn(0, _impl->scene->priceAxisCenterPosition());

    _impl->scene->onViewRectChange(QSizeF());

    const qreal pY1 = _impl->scene->priceToPoint(24613);
    const qreal pY2 = _impl->scene->priceToPoint(24000);

    _impl->rect = new QGraphicsRectItem(
        150.0,
        pY1,
        20,
        pY2 - pY1);

    _impl->rect->setBrush(QColor(124, 32, 223));
    _impl->scene->addItem(_impl->rect);
}

void CandleChartWidget::updateCandle(quint64 timestamp, double open, double close, double high, double low) noexcept
{
    assert(_impl->chartTimeFrame != CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime);

    _impl->scene->calculatePriceMax(high);


}


void CandleChartWidget::setHorizontalLinePen(const QPen &pen) noexcept
{
    _impl->horizontalCrosshairLine->setPen(pen);
}

void CandleChartWidget::setVerticalLinePen(const QPen &pen) noexcept
{
    _impl->verticalCrosshairLine->setPen(pen);
}

void CandleChartWidget::updateCrossHair(const QPointF &pt) noexcept
{
    const QPointF origin = mapToScene(0, 0);
    const QPointF size   = mapToScene(viewport()->width(), viewport()->height());
    const QPointF pts    = mapToScene(pt.toPoint().x(), pt.toPoint().y());

    const QPointF horzOrigin {
        origin.x(),
        pts.y()
    };
    const QPointF horzFinal {
        size.x() - 70,
        pts.y()
    };

    _impl->horizontalCrosshairLine->setLine({ horzOrigin, horzFinal });

    const QPointF vertOrigin {
        pts.x(),
        origin.y()
    };
    const QPointF vertFinal {
        pts.x(),
        size.y()
    };

    _impl->verticalCrosshairLine->setLine({ vertOrigin, vertFinal });

    /*
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

    _impl->verticalCrosshairLine->setLine({ vertOrigin, vertFinal });*/

    // qDebug() << m_candles.size() << position << timestamp;
}

void CandleChartWidget::setPriceAxisOrientation(Qt::AlignmentFlag orientation)
{
    if (orientation != Qt::AlignmentFlag::AlignRight && orientation != Qt::AlignmentFlag::AlignLeft)
        _impl->scene->setPriceAxisOrientation(Qt::AlignmentFlag::AlignRight);
    else
        _impl->scene->setPriceAxisOrientation(orientation);
}

void CandleChartWidget::resizeEvent(QResizeEvent *event)
{

    QGraphicsView::resizeEvent(event);

    // Notify the scene so it can adjust the axis's
    _impl->scene->onViewRectChange(QSizeF { event->size() });
}

void CandleChartWidget::leaveEvent(QEvent *event)
{
    _impl->verticalCrosshairLine->hide();
    _impl->horizontalCrosshairLine->hide();

    QWidget::leaveEvent(event);
}

void CandleChartWidget::mouseScrollAreaHorizontal(qreal newX) noexcept
{
    auto relative = newX - _impl->movePoint.x();

    if (qFuzzyCompare(relative, 0.0))
        return;

    translate(relative, 0);
}

void CandleChartWidget::mouseScrollAreaVertical(qreal newY) noexcept
{
    auto relative = newY - _impl->movePoint.y();
    if (qFuzzyCompare(relative, 0.0))
        return;

    translate(0, relative);
}

void CandleChartWidget::enterEvent(QEnterEvent *event)
{
    // Restore the visible status of the crosshair
    const auto verticalVisibleStatus   = _impl->verticalCrosshairLine->data(Qt::UserRole + 1).toBool();
    const auto horizontalVisibleStatus = _impl->horizontalCrosshairLine->data(Qt::UserRole + 1).toBool();

    if (verticalVisibleStatus)
        _impl->verticalCrosshairLine->show();

    if (horizontalVisibleStatus)
        _impl->horizontalCrosshairLine->show();

    QWidget::enterEvent(event);
}

void CandleChartWidget::wheelEvent(QWheelEvent *event)
{
    _impl->horizontalCrosshairLine->update();

    // Avoid the default wheelEvent behavior
    // QGraphicsView::wheelEvent(event);
}

void CandleChartWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (!_impl->grabbedAxis)
        {
            mouseScrollAreaHorizontal(event->position().x());
            mouseScrollAreaVertical(event->position().y());

            // Keep the axis's on its place
            _impl->scene->onViewRectChange(QSizeF());
        }
        else
        {
            if (_impl->grabbedAxis == Qt::Orientation::Vertical)
            {
                auto relative = event->position().y() - _impl->movePoint.y();
                if (!qFuzzyCompare(relative, 0.0))
                {

                    _impl->scene->scalePriceAxis(relative);

                    const qreal pY1 = _impl->scene->priceToPoint(24613);
                    const qreal pY2 = _impl->scene->priceToPoint(24000);
                    _impl->rect->setRect(
                        150.0,
                        pY1,
                        20,
                        pY2 - pY1);
                }
            }
            else
            {
            }
        }

        // Prevent proportional movement
        _impl->movePoint = event->position();
    }

    updateCrossHair(event->position());

    QGraphicsView::mouseMoveEvent(event);
}

void CandleChartWidget::mousePressEvent(QMouseEvent *event)
{
    _impl->movePoint   = event->position();
    _impl->grabPoint   = event->pos();
    _impl->grabbedAxis = static_cast<Qt::Orientation>(_impl->scene->isPointInAxis(mapToScene(event->pos())));

    QGraphicsView::mousePressEvent(event);
}

void CandleChartWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}
void CandleChartWidget::onShowAxisPriceTracker(bool show) noexcept
{
    show ? _impl->scene->getPriceAxis()->showTracker() : _impl->scene->getPriceAxis()->hideTracker();
}
bool CandleChartWidget::isPriceTrackerVisible() const noexcept
{
    return _impl->scene->getPriceAxis()->isTrackerVisible();
}

END_CENTAUR_NAMESPACE
