/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChartWidget.hpp"
#include "CandleChartScene.hpp"
#include "CandleItem.hpp"
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

        horizontalGridLinesGroup = scene->createItemGroup({});
        verticalGridLinesGroup   = scene->createItemGroup({});
    }
    ~Impl()
    {
        scene->destroyItemGroup(horizontalGridLinesGroup);
        scene->destroyItemGroup(verticalGridLinesGroup);
    }

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
    QGraphicsItemGroup *horizontalGridLinesGroup { nullptr };
    std::vector<QGraphicsLineItem *> horizontalGridLines;

    QGraphicsItemGroup *verticalGridLinesGroup { nullptr };
    std::vector<QGraphicsLineItem *> verticalGridLines;

public:
    CandleChartScene *scene;

    // Tracking lines (Crosshair)
public:
    QGraphicsLineItem *verticalCrosshairLine { nullptr };
    QGraphicsLineItem *horizontalCrosshairLine { nullptr };

public:
    QPointF movePoint;

public:
    Qt::Orientation grabbedAxis;
    QGraphicsRectItem *rect { nullptr };

public:
    CENTAUR_PLUGIN_NAMESPACE::TimeFrame chartTimeFrame { CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime };

public:
    std::unordered_map<int64_t, CandleItem *> chartCandles;

public:
    bool magnetizeTime { true };
};

CandleChartWidget::CandleChartWidget(QWidget *parent) :
    QGraphicsView(parent),
    _impl { new CandleChartWidget::Impl(this) }
{
    assert(_impl->scene != nullptr);

    setScene(_impl->scene);

    // Set this rect by default
    _impl->scene->setSceneRect(QRectF(0, 0, 2058, 2046));

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

void CandleChartWidget::addCandle(int64_t timestamp, double open, double close, double high, double low) noexcept
{
    // Call to updateCandle will add the candle.
    centerOn(0, _impl->scene->getPriceAxis()->center());

    _impl->scene->calculatePriceMax(high);

    _impl->scene->onViewRectChange(QSizeF());

    auto iter = _impl->chartCandles.find(timestamp);
    if (iter == _impl->chartCandles.end())
    {
        auto newChartItem              = new CandleItem(timestamp, open, close, high, low);
        _impl->chartCandles[timestamp] = newChartItem;
        _impl->scene->addItem(newChartItem);
        newChartItem->updateCandleRect();
    }
    else
    {
        iter->second->setParameters(open, close, high, low);
        iter->second->updateCandleRect();
    }
}

void CandleChartWidget::updateCandle(int64_t timestamp, double open, double close, double high, double low) noexcept
{
    assert(_impl->chartTimeFrame != CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime);

    auto iter = _impl->chartCandles.find(timestamp);
    if (iter != _impl->chartCandles.end())
    {
        _impl->scene->calculatePriceMax(high);
        iter->second->setParameters(open, close, high, low);
        iter->second->updateCandleRect();
    }
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
        size.x(),
        pts.y()
    };

    _impl->horizontalCrosshairLine->setLine({ horzOrigin, horzFinal });

    const QPointF vertOrigin {
        _impl->magnetizeTime ? _impl->scene->getTimeAxis()->middle(pts.x()) : pts.x(),
        origin.y()
    };
    const QPointF vertFinal {
        _impl->magnetizeTime ? _impl->scene->getTimeAxis()->middle(pts.x()) : pts.x(),
        size.y()
    };

    _impl->verticalCrosshairLine->setLine({ vertOrigin, vertFinal });
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
    updateHorizontalGridLines();
    updateVerticalGridLines();
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
            bool updateItemsRect = false;
            if (_impl->grabbedAxis == Qt::Orientation::Vertical)
            {
                auto relative = event->position().y() - _impl->movePoint.y();
                if (!qFuzzyCompare(relative, 0.0))
                {
                    _impl->scene->scalePriceAxis(-relative);
                    updateHorizontalGridLines();
                    updateItemsRect = true;
                }
            }
            else if (_impl->grabbedAxis == Qt::Orientation::Horizontal)
            {
                auto relative = event->position().x() - _impl->movePoint.x();
                if (!qFuzzyCompare(relative, 0.0))
                {
                    _impl->scene->scaleTimeAxis(-relative);
                    updateVerticalGridLines();
                    updateItemsRect = true;
                }
            }

            if (updateItemsRect)
                updateItemRects();
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
    _impl->grabbedAxis = static_cast<Qt::Orientation>(_impl->scene->isPointInAxis(mapToScene(event->pos())));

    QGraphicsView::mousePressEvent(event);
}

void CandleChartWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void CandleChartWidget::onShowPriceAxisTracker(bool show) noexcept
{
    show ? _impl->scene->getPriceAxis()->showTracker() : _impl->scene->getPriceAxis()->hideTracker();
}

bool CandleChartWidget::isPriceTrackerVisible() const noexcept
{
    return _impl->scene->getPriceAxis()->isTrackerVisible();
}

void CandleChartWidget::onChangeCandleWidth(qreal width) noexcept
{
    _impl->scene->getTimeAxis()->setCandleWidth(width);
}

void CandleChartWidget::onChangeCandleSpacing(qreal spacing) noexcept
{
    _impl->scene->getTimeAxis()->setCandleSpacing(spacing);
}

bool CandleChartWidget::isHorizontalGridLineVisible() const noexcept
{
    return _impl->horizontalGridLinesGroup->isVisible();
}

void CandleChartWidget::onShowHorizontalGridLines(bool show) noexcept
{
    show ? _impl->horizontalGridLinesGroup->show() : _impl->horizontalGridLinesGroup->hide();
}

void CandleChartWidget::updateHorizontalGridLines() noexcept
{
    if (!_impl->horizontalGridLinesGroup->isVisible())
        return;

    for (const auto &lines : _impl->horizontalGridLines)
        _impl->horizontalGridLinesGroup->removeFromGroup(lines);

    auto gridLines = _impl->scene->getPriceAxis()->getGridLinePositions();

    if (_impl->horizontalGridLines.size() < static_cast<std::size_t>(gridLines.size()))
    {
        _impl->horizontalGridLines.reserve(static_cast<std::size_t>(gridLines.size()));
        auto difference = static_cast<std::size_t>(gridLines.size()) - _impl->horizontalGridLines.size();
        for (decltype(difference) i = 0; i < difference; ++i)
        {
            auto item = new QGraphicsLineItem;
            item->setPen(QPen(QColor(255, 255, 255, 25), 1.0));
            item->setZValue(0);
            _impl->horizontalGridLines.emplace_back(item);
        }
    }
    else if (static_cast<std::size_t>(gridLines.size()) < _impl->horizontalGridLines.size() && !gridLines.empty())
    {
        for ( auto i = static_cast<std::size_t>(gridLines.size()); i < _impl->horizontalGridLines.size(); ++i )
            _impl->horizontalGridLines[i]->hide();
    }

    _impl->horizontalGridLinesGroup->hide();
    for (auto i = 0ll; i < gridLines.size(); ++i)
    {
        const QPointF horzOrigin {
            0,
            gridLines[i]
        };
        const QPointF horzFinal {
            scene()->sceneRect().width(),
            gridLines[i]
        };

        _impl->horizontalGridLines[static_cast<std::size_t>(i)]->setLine({ horzOrigin, horzFinal });
        _impl->horizontalGridLinesGroup->addToGroup(_impl->horizontalGridLines[static_cast<std::size_t>(i)]);
        _impl->horizontalGridLines[static_cast<std::size_t>(i)]->show();
    }

    _impl->horizontalGridLinesGroup->show();
}

void CandleChartWidget::setLinkTimestamp(int64_t timestamp) noexcept
{
    _impl->scene->getTimeAxis()->setTimestamp(timestamp);
}

bool CandleChartWidget::isTimeCrosshairMagnetize() const noexcept
{
    return _impl->magnetizeTime;
}

void CandleChartWidget::onMagnetizeTimeCrosshair(bool mag) noexcept
{
    _impl->magnetizeTime = mag;
}

bool CandleChartWidget::isTimeTrackerVisible() const noexcept
{
    return _impl->scene->getTimeAxis()->isTrackerVisible();
}

bool CandleChartWidget::isVerticalGridLineVisible() const noexcept
{
    return _impl->verticalGridLinesGroup->isVisible();
}

void CandleChartWidget::onShowVerticalGridLines(bool show) noexcept
{
    show ? _impl->verticalGridLinesGroup->show() : _impl->verticalGridLinesGroup->hide();
}

void CandleChartWidget::onShowTimeAxisTracker(bool show) noexcept
{
    show ? _impl->scene->getTimeAxis()->showTracker() : _impl->scene->getTimeAxis()->hideTracker();
}

void CandleChartWidget::updateVerticalGridLines() noexcept
{
    if (!_impl->verticalGridLinesGroup->isVisible())
        return;

    auto gridLines = _impl->scene->getTimeAxis()->getGridLinePositions();

    for (const auto &lines : _impl->verticalGridLines)
    {
        lines->hide();
        _impl->verticalGridLinesGroup->removeFromGroup(lines);
    }

    if (_impl->verticalGridLines.size() < static_cast<std::size_t>(gridLines.size()))
    {
        _impl->verticalGridLines.reserve(static_cast<std::size_t>(gridLines.size()));
        auto difference = static_cast<std::size_t>(gridLines.size()) - _impl->verticalGridLines.size();
        for (decltype(difference) i = 0; i < difference; ++i)
        {
            auto item = new QGraphicsLineItem;
            item->setPen(QPen(QColor(255, 255, 255, 25), 1.0));
            item->setZValue(0);
            _impl->verticalGridLines.emplace_back(item);
        }
    }

    _impl->verticalGridLinesGroup->hide();
    for (auto i = 0ll; i < gridLines.size(); ++i)
    {
        const QPointF vertOrigin {
            gridLines[i],
            0.0
        };
        const QPointF vertFinal {
            gridLines[i],
            scene()->sceneRect().height()
        };

        _impl->verticalGridLines[static_cast<std::size_t>(i)]->setLine({ vertOrigin, vertFinal });
        _impl->verticalGridLinesGroup->addToGroup(_impl->verticalGridLines[static_cast<std::size_t>(i)]);
        _impl->verticalGridLines[static_cast<std::size_t>(i)]->show();
    }

    _impl->verticalGridLinesGroup->show();
}

CandleChartScene *CandleChartWidget::ChartScene() noexcept
{
    return _impl->scene;
}

void CandleChartWidget::updateItemRects() noexcept
{

    QList<QGraphicsItem *> items = _impl->scene->items(viewport()->rect());
    for (auto &[ts, item] : _impl->chartCandles)
    {
        if (item->type() == ChartType::Candle)
        {
            // auto candle = qgraphicsitem_cast<CandleItem *>(item);
            // if (candle != nullptr)
            item->updateCandleRect();
        }
    }
}

void CandleChartWidget::onSetMinMaxPrice(qreal min, qreal max) noexcept
{
    _impl->scene->getPriceAxis()->setAxisMinMaxPrice(min, max);
    _impl->scene->getPriceAxis()->update();
    updateItemRects();
}

END_CENTAUR_NAMESPACE
