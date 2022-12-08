/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleItem.hpp"
#include "CandleChartScene.hpp"
#include "CandleChartWidget.hpp"
#include <QPainter>

BEGIN_CENTAUR_NAMESPACE

struct CandleItem::Impl
{
    Impl(int64_t ts, qreal op, qreal cs, qreal hg, qreal lw) :
        timestamp { ts },
        open { op },
        close { cs },
        high { hg },
        low { lw } { }

    QLineF highLine;
    QLineF lowLine;
    QRectF barRect;
    int64_t timestamp;
    qreal open;
    qreal close;
    qreal high;
    qreal low;
};

CandleItem::CandleItem(int64_t timestamp, qreal open, qreal close, qreal high, qreal low, QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    _impl { new CandleItem::Impl(timestamp, open, close, high, low) }
{
    setAcceptHoverEvents(true);
}

void CandleItem::hoverEnterEvent(C_UNUSED QGraphicsSceneHoverEvent *event)
{
    auto view = qobject_cast<CandleChartWidget *>(scene()->views().first());
    assert(view != nullptr);
    //    view->hoverItem(timestamp());

    // qDebug() << event;
}

void CandleItem::setParameters(double open, double close, double high, double low) noexcept
{
    _impl->open  = open;
    _impl->close = close;
    _impl->high  = high;
    _impl->low   = low;

    updateCandleRect();
}

void CandleItem::paint(QPainter *painter, C_UNUSED const QStyleOptionGraphicsItem *option, C_UNUSED QWidget *widget)
{ /*
     const QRectF &thisRect = rect();
     painter->setBrush(QBrush(QColor(50, 50, 50)));
     // painter->drawRect(thisRect);

     // painter->drawLines({ m_high, m_low });

     painter->setPen(QPen(QColor(0, 0, 255)));
     painter->drawLine(_impl->highLine);
     painter->setPen(QPen(QColor(255, 0, 255)));
     painter->drawLine(_impl->lowLine);

     if (isCandleBullish())
     {
         painter->setBrush(QBrush(QColor(0, 255, 0)));
         painter->setPen(QPen(QColor(0, 255, 0)));
     }
     else
     {
         painter->setBrush(QBrush(QColor(255, 0, 0)));
         painter->setPen(QPen(QColor(255, 0, 0)));
     }

     painter->drawRect(_impl->barRect);*/

    //     QGraphicsRectItem::paint(painter, option, widget);
}

void CandleItem::updateCandleRect() noexcept
{
    const QRectF &itemRect = rect();

    auto view = qobject_cast<CandleChartWidget *>(scene()->views().first());
    assert(view != nullptr);

    const auto candleWidth   = view->ChartScene()->getTimeAxis()->getCandleWidth();
    const auto candleSpacing = view->ChartScene()->getTimeAxis()->getCandleSpacing();
    const auto candleX       = view->ChartScene()->getTimeAxis()->pointFromTimestamp(_impl->timestamp);
    const auto candleLow     = view->ChartScene()->getPriceAxis()->priceToAxisPoint(_impl->low);
    const auto candleHigh    = view->ChartScene()->getPriceAxis()->priceToAxisPoint(_impl->high);
    const auto candleOpen    = view->ChartScene()->getPriceAxis()->priceToAxisPoint(_impl->open);
    const auto candleClose   = view->ChartScene()->getPriceAxis()->priceToAxisPoint(_impl->close);

    if (candleX < 0)
    {
        hide();
        return;
    }
    else
    {
        if (!isVisible())
            show();
    }

    const auto barLeft = candleX - (candleWidth / 2);

    const QRectF candleRect {
        barLeft - candleSpacing / 2.0,
        candleHigh,
        candleWidth + candleSpacing / 2.0,
        candleLow - candleHigh
    };

    _impl->barRect.setRect(
        barLeft,
        std::min(candleOpen, candleClose),
        candleWidth,
        std::abs(candleOpen - candleClose));

    // Top line
    _impl->highLine.setLine(
        candleX,
        candleHigh,
        candleX,
        _impl->barRect.top());

    _impl->lowLine.setLine(
        candleX,
        _impl->barRect.bottom(),
        candleX,
        candleLow);

    setRect(candleRect);
}

auto CandleItem::timestamp() const noexcept -> int64_t
{
    return _impl->timestamp;
}

auto CandleItem::open() const noexcept -> qreal
{
    return _impl->open;
}

auto CandleItem::close() const noexcept -> qreal
{
    return _impl->close;
}

auto CandleItem::high() const noexcept -> qreal
{
    return _impl->high;
}

auto CandleItem::low() const noexcept -> qreal
{
    return _impl->low;
}

auto CandleItem::isCandleBullish() const -> bool
{
    return _impl->close > _impl->open;
}

auto CandleItem::isCandleBearish() const -> bool
{
    return !isCandleBullish();
}

int CandleItem::type() const
{
    return ChartType::Candle;
}

END_CENTAUR_NAMESPACE

/*
m_barRect               = {
                  barLeft,
    isCandleBearish() ? closePoint : openPoint,
                  view->getCandleWidth(),
                  std::abs(closePoint - openPoint)

};


const auto top       = itemRect.top();
const auto bottom    = itemRect.bottom();

const auto barTop    = m_barRect.top();
const auto barBottom = m_barRect.bottom();

const auto lineX     = itemRect.left() + (candleWidth / 2);


m_high               = QLineF { lineX,
top,
lineX,
std::min(barTop, barBottom) };

m_low                = QLineF {
lineX,
bottom,
lineX,
std::max(barTop, barBottom)
};*/
