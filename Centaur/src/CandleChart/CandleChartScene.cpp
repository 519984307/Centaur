/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChart/CandleChartScene.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

cen::CandleChartScene::CandleChartScene(QObject *parent) :
    QGraphicsScene { parent },
    m_mouseTrackVert { new QGraphicsLineItem },
    m_mouseTrackHorz { new QGraphicsLineItem },
    m_mouseTrack { new QGraphicsItemGroup },
    m_priceAxis { nullptr },
    m_timeAxis { nullptr }
{
    assert(m_mouseTrackVert != nullptr);
    assert(m_mouseTrackHorz != nullptr);
    assert(m_mouseTrack != nullptr);

    // Mouse tracking
    m_mouseTrack->addToGroup(m_mouseTrackVert);
    m_mouseTrack->addToGroup(m_mouseTrackHorz);

    m_mouseTrackVert->setPen(QPen(QColor(255, 255, 255), 1, Qt::PenStyle::DashLine, Qt::RoundCap));
    m_mouseTrackHorz->setPen(QPen(QColor(255, 255, 255), 1, Qt::PenStyle::DashLine, Qt::RoundCap));

    addItem(m_mouseTrack);

    m_mouseTrack->hide();
}

void cen::CandleChartScene::onMouseEnter(C_UNUSED QEnterEvent *event) noexcept
{
    m_mouseTrack->show();
    if (m_priceAxis)
        m_priceAxis->mouseInScene(true);
    if (m_timeAxis)
        m_timeAxis->mouseInScene(true);
}

void cen::CandleChartScene::onMouseLeave(C_UNUSED QEvent *event) noexcept
{
    m_mouseTrack->hide();

    if (m_priceAxis)
        m_priceAxis->mouseInScene(false);
    if (m_timeAxis)
        m_timeAxis->mouseInScene(false);
}

void cen::CandleChartScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    auto width     = this->views().first()->width();
    auto height    = this->views().first()->height();
    auto pos       = mouseEvent->scenePos();

    QPointF size   = this->views().first()->mapToScene(width, height);
    QPointF origin = this->views().first()->mapToScene(0, 0);

    if (m_timeAxis)
    {
        if (auto at = m_timeAxis->isXPointInCandleFrame(pos.x()); at >= 0)
            m_mouseTrackVert->setLine(QLineF { at, origin.y(), at, size.y() });
    }

    m_mouseTrackHorz->setLine(QLineF { origin.x(), pos.y(), size.x(), pos.y() });
    m_priceAxis->setYPoint(pos.y());

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
void cen::CandleChartScene::setPriceAxisItem(cen::PriceAxisItem *item) noexcept
{
    m_priceAxis = item;
}

void cen::CandleChartScene::setTimeAxisItem(cen::TimeAxisItem *item) noexcept
{
    m_timeAxis = item;
}
