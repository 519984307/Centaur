/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChart/CandleItem.hpp"
#include "CandleChart/CandleChartScene.hpp"

#include <QPainter>

CENTAUR_NAMESPACE::CandleItem::CandleItem(uint64_t timestamp, double open, double close, double high, double low, QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    m_timestamp { timestamp },
    m_data { open, close, high, low },
    m_scene { nullptr }

{
    setAcceptHoverEvents(true);
}

void CENTAUR_NAMESPACE::CandleItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) noexcept
{
    // emit snMouseHoverItemCandle(this);
}

void CENTAUR_NAMESPACE::CandleItem::setParameters(double open, double close, double high, double low) noexcept
{
    m_data = { open, close, high, low };

    update();
}

void cen::CandleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    const QRectF &thisRect = rect();

    if (isCandleBullish())
        painter->setBrush(QBrush(QColor(0, 255, 0)));
    else
        painter->setBrush(QBrush(QColor(255, 0, 0)));

    painter->drawRect(m_barRect);
    /*
        painter->setPen(QPen(QColor(0, 0, 255)));
        painter->drawLine(m_high);
        painter->setPen(QPen(QColor(255, 0, 255)));
        painter->drawLine(m_low);*/
    painter->drawLines({ m_high, m_low });
    //     QGraphicsRectItem::paint(painter, option, widget);
}

void cen::CandleItem::updateRect() noexcept
{
    auto timeAxis                    = qobject_cast<CandleChartScene *>(scene())->getTimeAxis();
    auto priceAxis                   = qobject_cast<CandleChartScene *>(scene())->getPriceAxis();
    const auto &[xPosition, visible] = timeAxis->getGridFrame(m_timestamp);

    // qDebug() << visible << m_timestamp;
    if (!visible)
    {
        // Simply hide
        updateVisibility(false);
        return;
    }

    const auto candleWidth = timeAxis->getCandleWidth();

    const auto top         = priceAxis->priceToAxisPoint(m_data.high);
    const auto bottom      = priceAxis->priceToAxisPoint(m_data.low);

    const QRectF thisRect  = {
         xPosition,
         top,
         candleWidth,
         bottom - top
    };

    const auto barTop    = priceAxis->priceToAxisPoint(m_data.open);
    const auto barBottom = priceAxis->priceToAxisPoint(m_data.close);

    m_barRect            = QRectF {
        xPosition,
        barTop,
        candleWidth,
        barBottom - barTop
    };

    const auto lineX = xPosition + (candleWidth / 2);

    m_high           = QLineF { lineX,
        top,
        lineX,
        std::min(barTop, barBottom) };

    m_low            = QLineF {
        lineX,
        bottom,
        lineX,
        std::max(barTop, barBottom)
    };

    prepareGeometryChange();
    setRect(thisRect);
    updateVisibility(true);
}

void cen::CandleItem::updateVisibility(bool visible) noexcept
{
    if (!isVisible() && visible)
        show();
    else if (isVisible() && !visible)
        hide();
}
