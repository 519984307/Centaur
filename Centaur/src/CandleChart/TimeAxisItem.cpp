/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 05/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "CandleChart/TimeAxisItem.hpp"
#include <QPainter>

namespace
{

    uint64_t timeFrameToMilliseconds(cen::plugin::ICandleView::TimeFrame tf) noexcept
    {
        constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp sec   = 1000;
        constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp min   = sec * 60;
        constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp hr    = min * 60;
        constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp day   = hr * 24;
        constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp week  = day * 7;
        constexpr CENTAUR_PLUGIN_NAMESPACE::ICandleView::Timestamp month = day * 365 / 12;

        switch (tf)
        {
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::nullTime: return 0;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_1: return sec;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_5: return sec * 5;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_10: return sec * 10;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_30: return sec * 30;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_45: return sec * 45;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_1: return min;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_2: return min * 2;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_3: return min * 3;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_5: return min * 5;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_10: return min * 10;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_15: return min * 15;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_30: return min * 30;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_45: return min * 45;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_1: return hr;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_2: return hr * 2;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_4: return hr * 4;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_6: return hr * 6;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_8: return hr * 8;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_12: return hr * 12;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_1: return day;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_3: return day * 3;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Weeks_1: return week;
            case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Months_1: return month;
        }
    }

} // namespace

cen::TimeAxisItem::TimeAxisItem(double height, double candleWidth, double spacing, uint64_t min, uint64_t max, cen::plugin::ICandleView::TimeFrame tf, QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    m_height { height },
    m_candleWidth { candleWidth },
    m_spacing { spacing },
    m_min { min },
    m_max { max },
    m_tf { tf },
    m_mouseInScene { false }

{
}

void cen::TimeAxisItem::setHeight(double height) noexcept
{
    m_height = height;
    update();
}

void cen::TimeAxisItem::setCandleWidth(double width) noexcept
{
    m_candleWidth = width;
    calculateCandleRects();
    update();
}

void cen::TimeAxisItem::setCandleSpacing(double spacing) noexcept
{
    m_spacing = spacing;
    calculateCandleRects();
    update();
}

void cen::TimeAxisItem::setTimestampRange(uint64_t min, uint64_t max) noexcept
{
    m_min = min;
    m_max = max;
    recalculateAxisLabels();
    update();
}

void cen::TimeAxisItem::setTimeFrame(cen::plugin::ICandleView::TimeFrame tf) noexcept
{
    m_tf = tf;
    recalculateAxisLabels();
    update();
}

void cen::TimeAxisItem::paint(QPainter *painter, C_UNUSED const QStyleOptionGraphicsItem *option, C_UNUSED QWidget *widget)
{
    const QRectF &thisRect = rect();

    if (m_tf == CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::nullTime)
    {
        // No paint
        QGraphicsRectItem::paint(painter, option, widget);
        return;
    }

    painter->setPen(QPen(QColor(255, 0, 0)));
    painter->setBrush(QBrush(QColor(40, 40, 40)));
    painter->drawRect(thisRect);

    for (const auto &rc : m_candleRects)
    {
        painter->setPen(QColor(0, 255, 0));

        painter->drawRect(rc);
    }
}

uint64_t cen::TimeAxisItem::fit() const
{
    const QRectF &thisRect = rect();

    // The idea is to calculate, how many candles fit in the axis based on the parameters
    const double maxCandleWidth = m_candleWidth + m_spacing;
    const double dWidth         = thisRect.width();

    return static_cast<uint64_t>(dWidth / maxCandleWidth);
}

void cen::TimeAxisItem::mouseInScene(bool scene)
{
    m_mouseInScene = scene;
    update();
}

double cen::TimeAxisItem::isXPointInCandleFrame(double X) noexcept
{
    const QRectF &thisRect = rect();

    for (const auto &rc : m_candleRects)
    {
        QPointF testPoint { X, thisRect.top() + thisRect.height() / 2 };
        if (rc.contains(testPoint))
        {
            return thisRect.left() + rc.left() + rc.width() / 2;
        }
    }

    return -1;
}

void cen::TimeAxisItem::calculateCandleRects() noexcept
{
    const QRectF &thisRect = rect();
    m_candleRects.clear();
    const double maxCandleWidth = m_candleWidth + m_spacing;
    for (auto rc = 0ull; rc < fit(); ++rc)
    {
        const double newXPosition = maxCandleWidth * static_cast<double>(rc);
        const QRectF labelRect { newXPosition, thisRect.top(), maxCandleWidth, thisRect.height() };

        m_candleRects.append(labelRect);
    }
    m_candleRects.shrink_to_fit();

    /// Must recalculate, because this function is called every window resize by CandleChartWidget
    recalculateAxisLabels();
}

void cen::TimeAxisItem::recalculateAxisLabels()
{
    QString labelFormat {};

    switch (m_tf)
    {

        case plugin::ICandleView::TimeFrame::nullTime: labelFormat; break;
        case plugin::ICandleView::TimeFrame::Seconds_1: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Seconds_5: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Seconds_10: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Seconds_30: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Seconds_45:
            labelFormat = "mm:ss";
            break;
        case plugin::ICandleView::TimeFrame::Minutes_1: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Minutes_2: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Minutes_3: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Minutes_5: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Minutes_10: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Minutes_15: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Minutes_30: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Minutes_45: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Hours_1: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Hours_2: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Hours_4: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Hours_6: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Hours_8: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Hours_12:
            labelFormat = "HH:mm";
            break;
        case plugin::ICandleView::TimeFrame::Days_1: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Days_3: C_FALLTHROUGH;
        case plugin::ICandleView::TimeFrame::Weeks_1:
            labelFormat = "dd";
            break;
        case plugin::ICandleView::TimeFrame::Months_1:
            labelFormat = "MMM";
            break;
    }

    // Case exceptions are:
    // IF  labelFormat = "mm:ss" AND timestamp = square minute THEN LabelFormat = "mm";
    // IF labelFormat = "HH:mm" AND (timestamp = square hour OR timestamp = square day OR timestamp = square month) THEN LabelFormat = "HH" FOR HOUR; LabelFormat = "dd" FOR DAY; LabelFormat="MMM" for months;
    // IF labelFormat = "DD" AND (timestamp = square month or timestamp square year THEN LabelFormat = "MMM" for month; LabelFormat="yyyy" for years;
    // IF LabelFormat = "MMM" and (timestamp = square year) THEN LabelFormat = "yyyy"
    // Considering that all exceptions will have a Bold Font Style

    const uint64_t timeRange = (m_max - m_min);
    const uint64_t timeSteps = timeFrameToMilliseconds(m_tf);

    // Generate a set of string to determine the largest

    for( uint64_t )
}
