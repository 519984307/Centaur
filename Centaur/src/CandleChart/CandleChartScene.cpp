/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChart/CandleChartScene.hpp"
#include "CandleChart/CandleChartWidget.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

cen::CandleChartScene::CandleChartScene(QObject *parent) :
    QGraphicsScene { parent },
    m_mouseTrackVert { new QGraphicsLineItem },
    m_mouseTrackHorz { new QGraphicsLineItem },
    m_mouseTrack { new QGraphicsItemGroup },
    m_chartView { nullptr },
    m_priceAxis { nullptr },
    m_timeAxis { nullptr },
    m_timeAxisMin { 0 },
    m_timeAxisMax { 0 },
    m_timeSteps { 0 },
    m_priceAxisMin { 0 },
    m_priceAxisMax { 0 },
    m_priceSteps { 0 },
    m_priceAxisCapture { false },
    m_timeAxisCapture { false }
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
    const auto &pos      = mouseEvent->scenePos();
    const auto &lastPost = mouseEvent->lastScenePos();

    if (mouseEvent->buttons() & Qt::LeftButton)
    {
        bool timeContained  = m_timeAxis->rect().contains(pos),
             priceContained = m_priceAxis->rect().contains(pos);

        if (timeContained || m_timeAxisCapture)
        {
            const auto xScale        = lastPost.x() - pos.x();
            const auto scalingFactor = -xScale / 100;

            m_timeAxis->setCandleWidth(m_timeAxis->getCandleWidth() + scalingFactor /*, m_timeAxis->getCandleSpacing() + scalingFactor*/);
        }
        else if (priceContained || m_priceAxisCapture)
        {
            const auto yMove = pos.y() - lastPost.y();

            if (!qFuzzyCompare(yMove, 0))
            {
                const auto mouseElapsed = [&]() {
                    if (m_mouseMoveTime.isValid())
                        return std::max(1ll, m_mouseMoveTime.elapsed()); // avoid dividing by 0!!!
                    return 1ll;
                }();

                const auto dMouseElapsed = static_cast<double>(mouseElapsed) / 1000; // ms to secs
                const auto mouseSpeed    = yMove / dMouseElapsed;                    // pixels/secs (mouse speed);
                const auto scalingFactor = -m_priceSteps * mouseSpeed;

                m_priceAxis->setMinMax(m_priceAxisMin - scalingFactor, m_priceAxisMax + scalingFactor);

                // qDebug() << mouseElapsed << dMouseElapsed << mouseSpeed << scalingFactor;

                m_priceAxisMax = m_priceAxis->getMax();
                m_priceAxisMin = m_priceAxis->getMin();

                m_mouseMoveTime.start();
            }
        }
        else
        {
            if (!m_timeAxisCapture && !m_priceAxisCapture)
            {
                // Scrolling by moving the axis minimums
                const auto xMove = static_cast<int64_t>(pos.x() - lastPost.x());
                if (xMove != 0)
                {
                    /// qDebug() << pos.x() << mouseEvent->lastScenePos().x() << xMove;

                    const int64_t newMin = static_cast<int64_t>(m_timeAxisMin) + (static_cast<int64_t>(m_timeSteps) * -xMove);
                    if (newMin >= 0) // This should protect us from the signed->unsigned conversion of negative numbers
                    {
                        m_timeAxis->setTimestampRange(static_cast<uint64_t>(newMin), m_timeAxisMax);

                        // Update the parameter for an accurate movement
                        m_timeAxisMin = m_timeAxis->getMin();
                    }
                }

                const auto yMove = pos.y() - lastPost.y();

                if (!qFuzzyCompare(yMove, 0))
                {
                    const auto mouseElapsed = [&]() {
                        if (m_mouseMoveTime.isValid())
                            return std::max(1ll, m_mouseMoveTime.elapsed()); // avoid dividing by 0!!!
                        return 1ll;
                    }();

                    m_mouseMoveTime.start();

                    const auto dMouseElapsed = static_cast<double>(mouseElapsed) / 1000; // ms to secs
                    const auto mouseSpeed    = yMove / dMouseElapsed;                    // pixels/secs (mouse speed);
                    const auto movement      = m_priceSteps * mouseSpeed;

                    qDebug() << mouseElapsed << dMouseElapsed << mouseSpeed << m_priceSteps << movement << m_priceAxisMax << m_priceAxisMin;

                    // m_priceAxis->setMinMax(m_priceAxisMin + movement, m_priceAxisMax + movement);

                    // Update the parameters for an accurate movement
                    m_priceAxisMax = m_priceAxis->getMax();
                    m_priceAxisMin = m_priceAxis->getMin();
                }
            }
        }
    }


    auto width     = this->views().first()->width();
    auto height    = this->views().first()->height();

    QPointF size   = this->views().first()->mapToScene(width, height);
    QPointF origin = this->views().first()->mapToScene(0, 0);

    if (m_timeAxis)
    {
        if (auto at = m_timeAxis->isXPointInCandleFrame(pos.x()); at.first >= 0)
        {
            m_mouseTrackVert->setLine(QLineF { at.first, origin.y(), at.first, size.y() });
            m_timeAxis->setCursorPos(at.second);
            emit snUpdateCandleMousePosition(at.second);
        }
    }

    if (pos.y() < m_timeAxis->rect().top())
    {
        m_mouseTrackHorz->setLine(QLineF { origin.x(), pos.y(), size.x(), pos.y() });
        m_priceAxis->setYPoint(pos.y());
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void cen::CandleChartScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    const auto &pos = mouseEvent->scenePos();

    if (mouseEvent->buttons() & Qt::LeftButton)
    {
        if (m_timeAxis->rect().contains(pos))
            m_timeAxisCapture = true;
        if (m_priceAxis->rect().contains(pos))
            m_priceAxisCapture = true;
    }

    if (m_chartView == nullptr)
        m_chartView = qobject_cast<CandleChartWidget *>(this->views().first());

    // Time Axis parameters
    m_timeAxisMax = m_timeAxis->getMax();
    m_timeAxisMin = m_timeAxis->getMin();
    m_timeSteps   = TimeAxisItem::timeFrameToMilliseconds(m_timeAxis->getTimeframe());

    // Price Axis parameters
    m_priceAxisMax = m_priceAxis->getMax();
    m_priceAxisMin = m_priceAxis->getMin();
    m_priceSteps   = std::pow(10, static_cast<double>(-m_priceAxis->getPrecision()));

    m_mouseMoveTime.start();

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void cen::CandleChartScene::mouseReleaseEvent(C_UNUSED QGraphicsSceneMouseEvent *mouseEvent)
{

    m_priceAxisCapture = false;
    m_timeAxisCapture  = false;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void cen::CandleChartScene::setPriceAxisItem(cen::PriceAxisItem *item) noexcept
{
    m_priceAxis = item;
}

void cen::CandleChartScene::setTimeAxisItem(cen::TimeAxisItem *item) noexcept
{
    m_timeAxis = item;
}
