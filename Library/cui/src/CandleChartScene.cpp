/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChartScene.hpp"
#include "CandleChartWidget.hpp"
#include "CandlePriceAxisItem.hpp"
#include "CandleTimeAxisItem.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

BEGIN_CENTAUR_NAMESPACE

struct CandleChartScene::Impl
{
    explicit Impl(CandleChartScene *scene) :
        priceAxis { new CandlePriceAxisItem },
        timeAxis { new CandleTimeAxisItem }
    {
        scene->addItem(priceAxis);
        scene->addItem(timeAxis);

        priceAxis->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        priceAxis->setZValue(100);

        timeAxis->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        timeAxis->setZValue(100);
    }

public:
    CandlePriceAxisItem *priceAxis;
    CandleTimeAxisItem *timeAxis;

public:
    qreal priceMax { -1.0 };
};

CandleChartScene::CandleChartScene(QObject *parent) :
    QGraphicsScene { parent },
    _impl { new CandleChartScene::Impl(this) }
{
    setPriceAxisOrientation(Qt::AlignRight);
}

CandleChartScene::~CandleChartScene() = default;

int CandleChartScene::getPriceMaxFloatingPrecisionPoint(double price) noexcept
{
    constexpr int maxDecimals = 8; // Maximum number of decimals

    /// This function calculates where is the first significant decimal and then add two more decimals to be displayed
    /// For example
    ///     0.001
    ///     -> 0.001*10 = 0.01 to int -> 0 thus ++significant;
    ///     -> 0.01*10  = 0.1 to int -> 0 thus ++significant
    ///     -> 0.1*10   = 1 to int -> 1 thus significant = 2 then significant = min(significant + 3, maxDecimals)
    /// To string: "0.00100"

    int significant = 0;
    while (static_cast<int64_t>(price *= 10) == 0 && significant <= maxDecimals) ++significant;
    significant = std::min(maxDecimals, significant + 2);

    return significant;
}

CandlePriceAxisItem *CandleChartScene::getPriceAxis() noexcept
{
    return _impl->priceAxis;
}

void CandleChartScene::calculatePriceMax(qreal max)
{
    _impl->priceMax = std::max(max, _impl->priceMax);
    // Calculate the price stepping based on the number of units a number has
    // if the price is less than 1.0 the log will be zero or negative, this case will use the number of decimal positions
    _impl->priceAxis->setPriceStep(
        std::floor(std::log10(_impl->priceMax)),
        static_cast<qreal>(getPriceMaxFloatingPrecisionPoint(_impl->priceMax)));
    updatePriceAxisRect();
}

void CandleChartScene::setPriceAxisOrientation(Qt::AlignmentFlag orientation)
{
    assert(orientation == Qt::AlignRight || orientation == Qt::AlignLeft);

    _impl->priceAxis->setOrientation(orientation);
    updatePriceAxisRect();
}

void CandleChartScene::updatePriceAxisRect() noexcept
{
    auto viewList = views();
    if (viewList.empty())
        return;

    auto view = viewList[0];

    auto viewRect = view->frameRect();

    QFontMetrics metrics { font() };
    const QString string = QString("%1").arg(
        _impl->priceMax,
        0,
        'f',
        CandleChartScene::getPriceMaxFloatingPrecisionPoint(_impl->priceMax));

    const auto rectWidth = static_cast<qreal>(metrics.horizontalAdvance(string)) + 20;
    const auto posX      = _impl->priceAxis->orientation() == Qt::AlignLeft
                               ? viewRect.x()
                               : viewRect.right() - rectWidth;

    const auto mappedOrigin = view->mapToScene(static_cast<int>(posX), viewRect.y());

    _impl->priceAxis->setRect(
        mappedOrigin.x() - 1,
        sceneRect().top(),
        rectWidth,
        sceneRect().height());
}

void CandleChartScene::updateTimeAxisRect() noexcept
{
    auto viewList = views();
    if (viewList.empty())
        return;

    auto view = viewList[0];

    auto viewRect = view->frameRect();

    QFontMetrics metrics { font() };

    const auto rectHeight = metrics.height() + 10;
    const auto posY       = viewRect.bottom() - rectHeight;

    const auto mappedOrigin = view->mapToScene(viewRect.x(), static_cast<int>(posY));

    _impl->timeAxis->setRect(
        sceneRect().left(),
        mappedOrigin.y() - 1,
        sceneRect().width(),
        rectHeight);
}

void CandleChartScene::onViewRectChange(C_UNUSED QSizeF size)
{
    updateTimeAxisRect();
    updatePriceAxisRect();

    _impl->priceAxis->calculateRectangles();
}

void CandleChartScene::centerPriceAxisAt(qreal price)
{
    _impl->priceAxis->centerAt(price);
}

qreal CandleChartScene::priceAxisCenterPosition() const noexcept
{
    return _impl->priceAxis->center();
}

int CandleChartScene::isPointInAxis(const QPointF &pt) const noexcept
{
    auto viewList = views();
    if (viewList.empty())
        return 0;

    auto view = viewList[0];

    auto item = itemAt(pt, view->transform());

    if (item == _impl->priceAxis)
        return Qt::Orientation::Vertical;

    return 0;
}

void CandleChartScene::onMouseEnter(C_UNUSED QEnterEvent *event) noexcept
{
}

void CandleChartScene::onMouseLeave(C_UNUSED QEvent *event) noexcept
{
}

void CandleChartScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    _impl->priceAxis->updateTracker(mouseEvent->scenePos().y());

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CandleChartScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void CandleChartScene::mouseReleaseEvent(C_UNUSED QGraphicsSceneMouseEvent *mouseEvent)
{

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void CandleChartScene::scalePriceAxis(qreal factor) noexcept
{
    return _impl->priceAxis->scalePrice(factor);
}

qreal CandleChartScene::priceToPoint(qreal price) noexcept
{
    return _impl->priceAxis->priceToAxisPoint(price);
}

CandleTimeAxisItem *CandleChartScene::getTimeAxis() noexcept
{
    return _impl->timeAxis;
}

END_CENTAUR_NAMESPACE
