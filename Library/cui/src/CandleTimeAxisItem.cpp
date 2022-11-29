/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 05/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "CandleTimeAxisItem.hpp"
#include <QPainter>

BEGIN_CENTAUR_NAMESPACE

CandleTimeAxisItem::CandleTimeAxisItem(QGraphicsItem *parent) :
    QGraphicsRectItem(parent)
{
}

CandleTimeAxisItem::~CandleTimeAxisItem() = default;

uint64_t CandleTimeAxisItem::timeFrameToMilliseconds(plugin::TimeFrame tf) noexcept
{
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp sec   = 1000;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp min   = sec * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp hr    = min * 60;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp day   = hr * 24;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp week  = day * 7;
    constexpr CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp month = day * 365 / 12;

    switch (tf)
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

void CandleTimeAxisItem::setHeight(double height) noexcept
{
    m_height = height;
    update();
}

void CandleTimeAxisItem::setCandleParameters(double width, double spacing) noexcept
{
    m_candleWidth   = width;
    m_candleSpacing = spacing;

    if (m_candleWidth < 2)
        m_candleWidth = 2;
    if (m_candleSpacing < .05)
        m_candleSpacing = 0;

    if (auto maxLabelWidth = getAxisLabelsWidth(false); m_candleWidth + m_candleSpacing > maxLabelWidth)
    {
        m_candleSpacing = 1;
        m_candleWidth   = maxLabelWidth - 1;
        if (m_candleWidth < 0)
        {
            m_candleWidth = 4;
            qDebug() << "!!!" << m_candleSpacing << m_candleWidth;
        }
    }
    calculateCandleRects();
    recalculateAxisLabels();

    update();
}

void CandleTimeAxisItem::setCandleWidth(double width) noexcept
{
    setCandleParameters(width, getCandleSpacing());
}

void CandleTimeAxisItem::setCandleSpacing(double spacing) noexcept
{
    setCandleParameters(getCandleWidth(), spacing);
}

void CandleTimeAxisItem::setLabelSpacing(double spacing) noexcept
{
    m_labelSpacing = spacing;
    update();
}

void CandleTimeAxisItem::setTimestampRange(uint64_t min, uint64_t max) noexcept
{
    if (min <= m_absoluteMin)
        return;

    m_min = min;
    m_max = max;

    calculateCandleRects();
    recalculateAxisLabels();
    update();
}

void CandleTimeAxisItem::setTimeFrame(plugin::TimeFrame tf) noexcept
{
    m_tf = tf;
    recalculateAxisLabels();
    update();
}

uint64_t CandleTimeAxisItem::fit() const
{
    const QRectF &thisRect = rect();

    // The idea is to calculate, how many candles fit in the axis based on the parameters
    const double maxCandleWidth = m_candleWidth + m_candleSpacing;
    const double dWidth         = thisRect.width();

    return static_cast<uint64_t>(dWidth / maxCandleWidth);
}

void CandleTimeAxisItem::mouseInScene(bool scene)
{
    m_mouseInScene = scene;
    update();
}

QPair<double, uint64_t> CandleTimeAxisItem::isXPointInCandleFrame(double X) noexcept
{
    const QRectF &thisRect = rect();

    for (const auto &[ts, rect] : m_candleRects)
    {
        QPointF testPoint { X, thisRect.top() + thisRect.height() / 2 };
        if (rect.contains(testPoint))
        {
            return { thisRect.left() + rect.left() + rect.width() / 2, ts };
        }
    }

    return { -1, 0 };
}

void CandleTimeAxisItem::calculateCandleRects() noexcept
{
    const QRectF &thisRect = rect();
    m_candleRects.clear();
    const double maxCandleWidth = m_candleWidth + m_candleSpacing;
    const uint64_t timeSteps    = timeFrameToMilliseconds(m_tf);
    for (auto rc = 0ull; rc < fit(); ++rc)
    {
        const double newXPosition = maxCandleWidth * static_cast<double>(rc);
        const QRectF labelRect { newXPosition, thisRect.top(), maxCandleWidth, thisRect.height() };

        m_candleRects.emplace(m_min + (timeSteps * rc), labelRect);
    }
    // m_candleRects.red();

    /// Must recalculate, because this function is called every window resize by CandleChartWidget
    recalculateAxisLabels();
}

void CandleTimeAxisItem::recalculateAxisLabels() noexcept
{
    const auto widestFrame = getAxisLabelsWidth();

    const QRectF &thisRect = rect();
    // Now we can calculate how many label rectangles fit in the axis width
    double maxWidth = static_cast<double>(widestFrame) + m_labelSpacing;

    // By now, m_candleRects, must be set, so we must see, how many of this candle rectangles can fit one of this max width
    using qtSizeType    = decltype(m_candleRects.size());
    qtSizeType rectTest = 1;
    bool canBeFitted    = false;
    while (!canBeFitted && rectTest < m_candleRects.size())
    {
        double size = 0;
        for (qtSizeType i = 0; i < rectTest; ++i)
            size += m_candleRects.begin()->second.width();

        if (size >= maxWidth)
        {
            canBeFitted = true;
            maxWidth    = size; // set maxWidth to this new size. This will make the consequence shift calculation way easier
        }
        else
            ++rectTest;
    }

    // qDebug() << rectTest << canBeFitted;

    if (!canBeFitted)
    {
        qDebug() << "Time axis can not hold the time labels";
        m_labelRects.clear();
        return;
    }

    // Now we have to make sure that, a candle rectangle is centered with a label rectangle, this is to ensure the user that the candle and the time at the cursor position is correct
    // This is easy when the "rectTest" is odd, because, these centers are aligned, the problem arises when "rectTest" is even because, the center of the label is centered in a candle rectangle limit
    // so we must shift the label rectangles to the right to make those centers align
    const double shift = [&rectTest](double candleRectWidth) -> double {
        if ((rectTest % 2) == 0)
        {
            // The shift is simply the half of a candle rect
            return candleRectWidth / 2;
        }
        return 0; // Odd rectTest
    }(m_candleRects.begin()->second.width());

    // Now we have to calculate the axis time shift.
    // This is a problem because the labels are calculated based on m_min, and the first label, in the worst case scenario, is not centered in the first candle
    // In this situation, the first label has not the same shift as the rest of the labels
    auto firstTimeShiftSteps = static_cast<uint64_t>((rectTest / 2));
    auto timeShifts          = static_cast<uint64_t>(rectTest);

    const auto startTime = m_min + (timeFrameToMilliseconds(m_tf) * firstTimeShiftSteps);

    // Now the shift must reduce from the total width of the axis, otherwise, we can be painting outside the axis
    const double maxLabels  = (thisRect.width() - shift) / maxWidth; // Number of labels
    const auto intMaxLabels = static_cast<uint64_t>(maxLabels);

    m_labelRects.clear();
    m_gridLines.clear();
    QList<QPair<QString, QFont *>> labelData;
    getAxisLabelsWidth(true, &labelData, startTime, intMaxLabels, timeShifts);

    if (labelData.isEmpty())
        return;

    for (auto k = 0ull; k < intMaxLabels; ++k)
    {
        const double newXPosition = static_cast<double>(k) * maxWidth + shift;
        const QRectF rc { newXPosition, thisRect.top(), maxWidth, thisRect.height() };

        m_gridLines.emplace_back(newXPosition + (maxWidth / 2));
        m_labelRects.emplace_back(labelData[static_cast<qint64>(k)].first, rc, labelData[static_cast<qint64>(k)].second);
    }
    m_gridLines.shrink_to_fit();
    m_labelRects.shrink_to_fit();

    // qDebug() << ppp << m_labelRects.size();
}

double CandleTimeAxisItem::getAxisLabelsWidth(bool retrievingMode, QList<QPair<QString, QFont *>> *labels, uint64_t startAtTimestamp, uint64_t numberOfLabels, uint64_t shifts) noexcept
{
    QString labelFormat {};

    switch (m_tf)
    {

        case plugin::TimeFrame::nullTime: labelFormat = ""; break;
        case plugin::TimeFrame::Seconds_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_5: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_10: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_30: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_45:
            labelFormat = "mm:ss";
            break;
        case plugin::TimeFrame::Minutes_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_2: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_3: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_5: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_10: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_15: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_30: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_45: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_2: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_4: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_6: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_8: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_12:
            labelFormat = "HH:mm";
            break;
        case plugin::TimeFrame::Days_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Days_3: C_FALLTHROUGH;
        case plugin::TimeFrame::Weeks_1:
            labelFormat = "dd";
            break;
        case plugin::TimeFrame::Months_1:
            labelFormat = "MMM";
            break;
    }

    if (labelFormat.isEmpty())
        return 0;

    // Case exceptions are:
    // IF  labelFormat = "mm:ss" AND timestamp = square minute THEN LabelFormat = "mm";
    // IF labelFormat = "HH:mm" AND (timestamp = square hour OR timestamp = square day OR timestamp = square month) THEN LabelFormat = "HH" FOR HOUR; LabelFormat = "dd" FOR DAY; LabelFormat="MMM" for months;
    // IF labelFormat = "DD" AND (timestamp = square month or timestamp square year THEN LabelFormat = "MMM" for month; LabelFormat="yyyy" for years;
    // IF LabelFormat = "MMM" and (timestamp = square year) THEN LabelFormat = "yyyy"
    // Considering that all exceptions will have a Bold Font Style

    const uint64_t timeSteps = timeFrameToMilliseconds(m_tf);
    const uint64_t loopSteps = !retrievingMode ? timeSteps : 1;

    // Generate a set of strings to determine the largest
    const QString cacheFormat = labelFormat;
    QFont *fontUsed;
    int widestFrame = 0;

    const uint64_t startAt = [&]() {
        if (!retrievingMode)
            return m_min;
        return 0ull;
    }();

    const uint64_t endAt = [&]() {
        if (!retrievingMode)
            return m_max;
        return numberOfLabels;
    }();

    for (auto i = startAt; i < endAt; i += loopSteps)
    {
        const auto ms = static_cast<qint64>(!retrievingMode ? i : startAtTimestamp + (i * timeSteps * shifts));
        QDateTime dt  = QDateTime::fromMSecsSinceEpoch(ms);
        auto time     = dt.time();
        auto date     = dt.date();
        labelFormat   = cacheFormat;
        fontUsed      = &m_normalFont;

        if (labelFormat == "mm:ss" && time.second() == 0)
            labelFormat = "mm";
        else if (labelFormat == "HH:mm")
        {
            if (time.minute() == 0 && time.second() == 0)
            {
                // HH:00:00
                labelFormat = "HH";
            }

            if (time.hour() == 0 && time.minute() == 0 && time.second() == 0)
            {
                // day 00:00:00
                labelFormat = "dd";
            }

            if (date.day() == 1 && time.hour() == 0 && time.minute() == 0 && time.second() == 0)
            {
                // First of each month at 00:00:00
                labelFormat = "MMM";
            }
        }
        else if (labelFormat == "dd")
        {
            if (date.day() == 1 && time.hour() == 0 && time.minute() == 0 && time.second() == 0)
            {
                // First of each month at 00:00:00
                labelFormat = "MMM";
            }

            if (date.month() == 1 && date.day() == 1 && time.hour() == 0 && time.minute() == 0 && time.second() == 0)
            {
                // First of january at 00:00:00
                labelFormat = "yyyy";
            }
        }
        else if (labelFormat == "MMM")
        {
            if (date.month() == 1 && date.day() == 1 && time.hour() == 0 && time.minute() == 0 && time.second() == 0)
            {
                // First of january at 00:00:00
                labelFormat = "yyyy";
            }
        }

        if (labelFormat != cacheFormat)
            fontUsed = &m_boldFont;

        const auto string = dt.toString(labelFormat);

        const QFontMetrics fontMetrics(*fontUsed);
        auto size = fontMetrics.size(Qt::TextSingleLine, string);

        // qDebug() << "###" << size.width() << widestFrame;
        widestFrame = std::max(size.width(), widestFrame); // All frames will have this

        if (retrievingMode)
        {
            labels->emplace_back(string, fontUsed);
        }

        // qDebug() << retrievingMode << string;
    }

    return widestFrame;
}

void CandleTimeAxisItem::setCursorPos(uint64_t timestamp) noexcept
{
    m_cursorIndex = timestamp;

    // This information does not need to be updated on every paint event, since, the Y cursor is always fixed in the center of a candle
    const QRectF &thisRect = rect();
    QFontMetrics metrics(m_normalFont);
    const QString timeFormat = [&]() {
        const uint64_t candleTimeframe = timeFrameToMilliseconds(m_tf);
        if (candleTimeframe < 3'600'000) // 3,600,000 is the number of milliseconds in a minute, thus, if the time is less a minute we must include the seconds on the label
            return "dd-MM-yyyy HH:mm:ss";
        else
            return "dd-MM-yyyy HH:mm";
    }();

    // const auto &time       = m_candleRects.at(m_cursorIndex);
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(timestamp));
    m_infoLabel  = dt.toString(timeFormat);
    auto size    = metrics.size(Qt::TextSingleLine, m_infoLabel);

    const auto width       = static_cast<qreal>(size.width());
    const auto &candleRect = m_candleRects.at(timestamp);
    // Center the label to candle rectangle center
    auto candleCenterXPos = (thisRect.left() + candleRect.left() + candleRect.width() / 2) - (width / 2);

    if (candleCenterXPos < thisRect.left())
        candleCenterXPos = 0; // Prevent hiding the label to the left
    if ((candleCenterXPos + width) > thisRect.width())
        candleCenterXPos = thisRect.width() - width; // Prevent hiding the label to the right

    m_infoRect = QRectF { candleCenterXPos, thisRect.top(), width, thisRect.height() };

    update();
}

void CandleTimeAxisItem::paint(QPainter *painter, C_UNUSED const QStyleOptionGraphicsItem *option, C_UNUSED QWidget *widget)
{
    const QRectF &thisRect = rect();

    painter->setPen(QPen(QColor(255, 0, 0)));
    painter->setBrush(QBrush(QColor(40, 40, 40)));
    painter->drawRect(thisRect);

    if (m_tf == CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime)
    {
        // No paint
        QGraphicsRectItem::paint(painter, option, widget);
        return;
    }
    /*

    for (const auto &rc : m_candleRects)
    {
        painter->setPen(QColor(0, 255, 0));
        painter->drawRect(rc.second);
    }
    */
    /*
        for (const auto &label : m_labelRects)
        {

            const auto &labelText = std::get<0>(label);
            const auto &rect      = std::get<1>(label);
            const auto &font      = std::get<2>(label);
            // painter->drawRect(rect);
            painter->setPen(QPen(QColor(200, 200, 200)));
            painter->setFont(*font);
            painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, labelText);
        }

        if (m_cursorIndex > 0 && m_mouseInScene)
        {
            painter->setPen(QColor(80, 80, 80));
            painter->setBrush(QBrush(QColor(80, 80, 80)));
            painter->drawRect(m_infoRect);
            painter->setPen(QColor(220, 220, 220));
            painter->setFont(m_normalFont);
            painter->drawText(m_infoRect, Qt::AlignHCenter | Qt::AlignVCenter, m_infoLabel);
        }

        painter->setPen(QColor(150, 150, 150));
        painter->drawLine(QLineF { thisRect.left(), thisRect.top(), thisRect.width(), thisRect.top() });
        painter->drawLine(QLineF { thisRect.width(), thisRect.top(), thisRect.width(), thisRect.bottom() });*/
}

QList<double> CandleTimeAxisItem::getGridLinePositions() noexcept
{
    return m_gridLines;
}

QPair<double, bool> CandleTimeAxisItem::getGridFrame(uint64_t timestamp)
{
    auto iter = m_candleRects.find(timestamp);
    if (iter == m_candleRects.end())
        return { 0, false };
    return { iter->second.left() + (iter->second.width() / 2) - (m_candleWidth / 2), true };
}

END_CENTAUR_NAMESPACE
