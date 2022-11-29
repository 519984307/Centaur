/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleItem.hpp"
#include "CandleChartWidget.hpp"
#include <QPainter>

CENTAUR_NAMESPACE::CandleItem::CandleItem(uint64_t timestamp, double open, double close, double high, double low, QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    m_timestamp { timestamp },
    m_data { open, close, high, low }
{
    setAcceptHoverEvents(true);
    //  setFlag(ItemIsMovable, true);
}

void CENTAUR_NAMESPACE::CandleItem::hoverEnterEvent(C_UNUSED QGraphicsSceneHoverEvent *event)
{
    auto view = qobject_cast<CandleChartWidget *>(scene()->views().first());
    assert(view != nullptr);
    //    view->hoverItem(timestamp());

    // qDebug() << event;
}

void CENTAUR_NAMESPACE::CandleItem::setParameters(double open, double close, double high, double low) noexcept
{
    m_data = { open, close, high, low };

    update();
}

void cen::CandleItem::paint(QPainter *painter, C_UNUSED const QStyleOptionGraphicsItem *option, C_UNUSED QWidget *widget)
{
    const QRectF &thisRect = rect();
    painter->setBrush(QBrush(QColor(50, 50, 50)));
    // painter->drawRect(thisRect);

    // painter->drawLines({ m_high, m_low });

    painter->setPen(QPen(QColor(0, 0, 255)));
    painter->drawLine(m_high);
    painter->setPen(QPen(QColor(255, 0, 255)));
    painter->drawLine(m_low);

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

    painter->drawRect(m_barRect);

    //     QGraphicsRectItem::paint(painter, option, widget);
}

void cen::CandleItem::updateCandleRect(double openPoint, double closePoint, double highPoint, double lowPoint) noexcept
{ /*
     const QRectF &itemRect = rect();

     auto view = qobject_cast<CandleChartWidget *>(scene()->views().first());
     assert(view != nullptr);

     const auto &candleWidth = view->getCandleWidth() + view->getCandleSpacing();
     const auto barLeft      = itemRect.left() + (view->getCandleSpacing() / 2);

     m_barRect = QRectF {
         barLeft,
         std::min(openPoint, closePoint),
         view->getCandleWidth(),
         std::abs(openPoint - closePoint)

     };

     const auto top    = std::min(highPoint, lowPoint);
     const auto bottom = std::max(highPoint, lowPoint);

     const auto barTop    = m_barRect.top();
     const auto barBottom = m_barRect.bottom();

     const auto lineX = itemRect.left() + (candleWidth / 2);

     m_high = QLineF { lineX,
         top,
         lineX,
         std::min(barTop, barBottom) };

     m_low = QLineF {
         lineX,
         bottom,
         lineX,
         std::max(barTop, barBottom)
     };*/
}

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
