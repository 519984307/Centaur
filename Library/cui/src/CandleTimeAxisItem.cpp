/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 05/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "CandleTimeAxisItem.hpp"
#include <QApplication>
#include <QDateTime>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>

BEGIN_CENTAUR_NAMESPACE

enum class TimeframeGroup : int
{
    nullTime,
    Seconds,
    Minutes,
    Hours,
    Days,
    Months
};

static TimeframeGroup groupTimeFrame(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf) noexcept
{
    switch (tf)
    {
        case plugin::TimeFrame::nullTime:
            return TimeframeGroup::nullTime;
        case plugin::TimeFrame::Seconds_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_5: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_10: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_30: C_FALLTHROUGH;
        case plugin::TimeFrame::Seconds_45:
            return TimeframeGroup::Seconds;
        case plugin::TimeFrame::Minutes_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_2: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_3: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_5: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_10: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_15: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_30: C_FALLTHROUGH;
        case plugin::TimeFrame::Minutes_45:
            return TimeframeGroup::Minutes;
        case plugin::TimeFrame::Hours_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_2: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_4: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_6: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_8: C_FALLTHROUGH;
        case plugin::TimeFrame::Hours_12:
            return TimeframeGroup::Hours;
        case plugin::TimeFrame::Days_1: C_FALLTHROUGH;
        case plugin::TimeFrame::Days_3: C_FALLTHROUGH;
        case plugin::TimeFrame::Weeks_1:
            return TimeframeGroup::Days;
        case plugin::TimeFrame::Months_1:
            return TimeframeGroup::Months;
    }
}

struct LabelRect
{
    LabelRect() = default;

    LabelRect(const QRectF &rc, const std::pair<QString, bool> &data, int64_t ts) noexcept :
        rect { rc },
        text { data.first },
        timestamp { ts },
        useBold { data.second }
    {
    }

    template <typename... T>
    void setRect(T &&...args) noexcept
    {
        rect = QRectF { std::forward<T>(args)... };
    }

    explicit operator QRectF() const noexcept { return rect; }
    explicit operator QString() const noexcept { return text; }

    QRectF rect;
    QString text;
    int64_t timestamp { 0 };
    bool useBold { false };
};

struct CandleTimeAxisItem::Impl
{
    Impl() :
        axisFont { QApplication::font() },
        boldAxisFont { axisFont },
        borderPen { QPen(QColor(230, 230, 230), 0.5) },
        tf { CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime },
        labelRealSize { 0 },
        linkTimestamp { 0 },
        candleWidth { 10.0 },
        candleSpacing { 2.0 },
        scaleFactor { 0.0 },
        priceAxisWidth { 0.0 },
        maxTagLabelWidth { CandleTimeAxisItem::longestPossibleTagSize(axisFont) },
        showTracker { true }
    {
        boldAxisFont.setWeight(QFont::Weight::Bold);
    }

    ~Impl() = default;

public:
    QFont axisFont;
    QFont boldAxisFont;
    QPen borderPen;

    LabelRect trackerRect;

    std::vector<QRectF> candleRects;
    std::vector<LabelRect> labelRects;

    QRectF scaledAxis;

    CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf;

    std::size_t labelRealSize;

    int64_t linkTimestamp;

    qreal candleWidth;
    qreal candleSpacing;
    qreal scaleFactor;
    qreal priceAxisWidth;
    qreal maxTagLabelWidth;

    bool showTracker;
};

CandleTimeAxisItem::CandleTimeAxisItem(QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    _impl { new Impl }
{
}

CandleTimeAxisItem::~CandleTimeAxisItem() = default;

int64_t CandleTimeAxisItem::timeFrameToMilliseconds(plugin::TimeFrame tf) noexcept
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

auto CandleTimeAxisItem::getTimeframe() const noexcept -> CENTAUR_PLUGIN_NAMESPACE::TimeFrame
{
    return _impl->tf;
}

auto CandleTimeAxisItem::getCandleWidth() const noexcept -> qreal
{
    if (_impl->candleRects.empty())
        return _impl->candleWidth;
    else
        return _impl->candleRects[0].width() - _impl->candleSpacing;
}

auto CandleTimeAxisItem::getCandleSpacing() const noexcept -> qreal
{
    return _impl->candleSpacing;
}

void CandleTimeAxisItem::setCandleParameters(double width, double spacing) noexcept
{
    _impl->candleWidth   = width;
    _impl->candleSpacing = spacing;

    if (_impl->candleWidth < 2)
        _impl->candleWidth = 2;
    else if (_impl->candleWidth > 30)
        _impl->candleWidth = 30;

    if (_impl->candleSpacing < 0)
        _impl->candleSpacing = 0;
    else if (_impl->candleSpacing > 30)
        _impl->candleSpacing = 30;

    calculateRectangles();

    update();
}

void CandleTimeAxisItem::setCandleWidth(qreal width) noexcept
{
    setCandleParameters(width, getCandleSpacing());
}

void CandleTimeAxisItem::setCandleSpacing(qreal spacing) noexcept
{
    setCandleParameters(getCandleWidth(), spacing);
}

void CandleTimeAxisItem::setTimeFrame(plugin::TimeFrame tf) noexcept
{
    _impl->tf = tf;
    calculateRectangles();
    update();
}

std::pair<QString, bool> CandleTimeAxisItem::labelFromTimestamp(int64_t timestamp) noexcept
{
    bool useBold = false;
    QString labelFormat {};

    switch (groupTimeFrame(_impl->tf))
    {
        case TimeframeGroup::nullTime:
            labelFormat = "";
            break;
        case TimeframeGroup::Seconds:
            labelFormat = "mm:ss";
            break;
        case TimeframeGroup::Minutes:
            C_FALLTHROUGH;
        case TimeframeGroup::Hours:
            labelFormat = "HH:mm";
            break;
        case TimeframeGroup::Days:
            labelFormat = "dd";
            break;
        case TimeframeGroup::Months:
            labelFormat = "MMM";
            break;
    }

    const QDateTime dt = QDateTime::fromMSecsSinceEpoch(timestamp);
    const QTime tm     = dt.time();
    const QDate da     = dt.date();

    if (tm.hour() == 0 && tm.second() == 0 && tm.minute() == 0)
    {
        useBold = true;
        if (da.day() == 1)
        {
            if (da.month() == 1)
                labelFormat = "yyyy";
            else
                labelFormat = "MMM";
        }
        else
            labelFormat = "d";
    }

    if (labelFormat.isEmpty())
        return { QString(), false };

    // qDebug() << dt << useBold << dt.toMSecsSinceEpoch() << timestamp;
    return { dt.toString(labelFormat), useBold };
}

void CandleTimeAxisItem::paint(QPainter *painter, C_UNUSED const QStyleOptionGraphicsItem *option, C_UNUSED QWidget *widget)
{
    const QRectF &thisRect = rect();

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(25, 25, 25));
    painter->drawRect(thisRect);

    // Draw the border
    painter->setPen(_impl->borderPen);
    painter->drawLine(QLineF { 0, thisRect.top(), thisRect.right() - _impl->priceAxisWidth, thisRect.top() });

    if (_impl->tf == CENTAUR_PLUGIN_NAMESPACE::TimeFrame::nullTime)
    {
        // No paint
        QGraphicsRectItem::paint(painter, option, widget);
        return;
    }

    for (const auto &rc : _impl->candleRects)
    {
        painter->drawRect(rc);
    }

    painter->setPen(QColor(200, 255, 255));

    for (const auto &item : _impl->labelRects)
    {
        if (item.rect.right() > _impl->scaledAxis.right() || item.rect.left() < _impl->scaledAxis.left())
            continue;

        if (item.useBold)
            painter->setBrush(QColor(255, 0, 255, 120));
        else
            painter->setBrush(QColor(23, 105, 123, 200));

        painter->drawRect(QRectF(item));

        if (item.useBold)
        {
            painter->save();
            painter->setFont(_impl->boldAxisFont);
        }
        painter->drawText(
            QRectF(item),
            QString(item),
            Qt::AlignVCenter | Qt::AlignHCenter);

        if (item.useBold)
            painter->restore();
    }

    if (_impl->showTracker)
    {
        painter->setBrush(QColor(80, 80, 85));
        painter->drawRect(QRectF(_impl->trackerRect));
        painter->drawText(QRectF(_impl->trackerRect), QString(_impl->trackerRect), Qt::AlignVCenter | Qt::AlignHCenter);
    }
}

void CandleTimeAxisItem::scaleTime(qreal factor) noexcept
{
    _impl->scaleFactor = factor > 0 ? _impl->scaleFactor + 5.0 : _impl->scaleFactor - 5.0;

    if (_impl->scaleFactor < 1)
        _impl->scaleFactor = 1;
    if (_impl->scaleFactor > 500)
        _impl->scaleFactor = 500;

    calculateRectangles();

    update();
}

void CandleTimeAxisItem::calculateRectangles()
{
    // The idea is, to have two set of rectangles
    // One with labels and the other with the candles
    // Being the set of labels totally dependant of the set of candle rects
    const QRectF &thisRect = rect();
    _impl->scaledAxis      = thisRect;
    _impl->scaledAxis.setRight(thisRect.right() - _impl->priceAxisWidth);

    if (_impl->scaledAxis.right() < _impl->scaledAxis.left())
        return;

    const qreal totalWidth = _impl->candleWidth + _impl->candleSpacing;

    _impl->scaledAxis.setLeft(_impl->scaledAxis.left() - _impl->scaleFactor * totalWidth);

    // Round to the lowest to prevent clipping rects
    const auto axisTotalPossibleTags = static_cast<uint64_t>(std::floor((thisRect.width() - _impl->priceAxisWidth) / totalWidth));
    const auto dTotalTags            = static_cast<qreal>(axisTotalPossibleTags);

    if (!axisTotalPossibleTags)
        return;

    if (_impl->candleRects.size() != axisTotalPossibleTags)
    {
        _impl->candleRects.clear();
        _impl->candleRects.reserve(axisTotalPossibleTags);
        for (auto i = 0ull; i < axisTotalPossibleTags; ++i)
            _impl->candleRects.emplace_back(0, 0, 0, 0);
    }

    const qreal actualWidth         = _impl->scaledAxis.width() / dTotalTags;
    const qreal relativeMiddlePoint = actualWidth / 2.0;
    qreal left                      = _impl->scaledAxis.left();
    const qreal leftLabelShift      = _impl->maxTagLabelWidth / 2.0;

    _impl->labelRects.clear();

    QList<qreal> anchors;
    for (auto i = 0ull; i < axisTotalPossibleTags; ++i)
    {
        _impl->candleRects[i].setRect(left, _impl->scaledAxis.top(), actualWidth, _impl->scaledAxis.height());
        if (isTimestampAnchor(timestampFromPoint(left + relativeMiddlePoint)))
            anchors.emplace_back(left + relativeMiddlePoint);

        left += actualWidth;
    }

    if (anchors.empty())
    {
        const auto axisMiddlePoint = _impl->scaledAxis.center().x();
        if (const auto index = indexFromPoint(axisMiddlePoint); index >= 0)
            anchors.emplaceBack(_impl->candleRects[static_cast<std::size_t>(index)].center().x());
    }

    const qreal distance = [&]() {
        if (anchors.size() > 1)
        {
            // distance is constant across all anchors
            return std::abs(anchors[0] - anchors[1]);
        }
        else
        {
            return (_impl->scaledAxis.right() + leftLabelShift) - anchors[0];
        }
    }();

    _impl->labelRects.clear();

    // set the anchors in the edges
    anchors.push_back(anchors.last() + distance);
    anchors.push_front(anchors.front() - distance);

    QList<QRectF> rects;
    rects.emplace_back(
        anchors.front() - leftLabelShift,
        _impl->scaledAxis.top(),
        _impl->maxTagLabelWidth,
        _impl->scaledAxis.height());
    anchors.pop_front();

    while (!anchors.empty())
    {

        QRectF savedRect {
            anchors.front() - leftLabelShift,
            _impl->scaledAxis.top(),
            _impl->maxTagLabelWidth,
            _impl->scaledAxis.height()
        };
        rects.emplace_back(savedRect);
        anchors.pop_front();

        while (!rects.empty())
        {
            qreal currentDistance = std::abs((rects.back().left() - rects[rects.size() - 2].left())) / 2.0;

            QRectF newRect {
                rects.back().left() - currentDistance,
                _impl->scaledAxis.top(),
                _impl->maxTagLabelWidth,
                _impl->scaledAxis.height()
            };

            const qreal middleNewRect  = newRect.center().x();
            const auto candleRectIndex = indexFromPoint(middleNewRect);
            if (candleRectIndex >= 0)
            {
                const auto &candleRect       = _impl->candleRects[static_cast<std::size_t>(candleRectIndex)];
                const auto candleMiddlePoint = candleRect.center().x();

                const auto diff = std::abs(middleNewRect - candleMiddlePoint);
                if (!qFuzzyCompare(diff, 0))
                {
                    // Centers are not aligned
                    if (middleNewRect > candleMiddlePoint)
                    {
                        // The new rect center is displaced towards the left of the middle candle rect
                        newRect.moveLeft(newRect.left() - diff);
                    }
                    else
                    {
                        // The new rect center is displaced towards the right of the middle candle rect
                        newRect.moveLeft(newRect.left() + diff);
                    }
                }
            }

            bool intersects = false;

            std::for_each(rects.begin(), rects.end(), [&](const auto &rc) {
                if (rc.intersects(newRect))
                    intersects = true;
            });

            if (!intersects)
                rects.insert(rects.size() - 1, newRect);
            else
            {
                const int64_t timestamp = timestampFromPoint(rects.back().left() + leftLabelShift);
                _impl->labelRects.emplace_back(rects.back(), labelFromTimestamp(timestamp), timestamp);
                rects.pop_back();
            }

            if (rects.size() == 1)
            {
                const int64_t timestamp = timestampFromPoint(rects.back().left() + leftLabelShift);
                _impl->labelRects.emplace_back(rects.back(), labelFromTimestamp(timestamp), timestamp);
                rects.pop_back();
            }
        }
        // At this point what we really need is the saved rect
        rects.push_back(savedRect);
    }
}

void CandleTimeAxisItem::updatePriceAxisWidth(qreal width) noexcept
{
    _impl->priceAxisWidth = width;
}

void CandleTimeAxisItem::setTimestamp(int64_t timestamp) noexcept
{
    _impl->linkTimestamp = timestamp;
}

auto CandleTimeAxisItem::timestampFromPoint(qreal point) const noexcept -> int64_t
{
    const auto index = indexFromPoint(point);
    if (index < 0)
        return 0.0;

    const auto axisMilliseconds = CandleTimeAxisItem::timeFrameToMilliseconds(_impl->tf);

    const auto total = _impl->linkTimestamp - (static_cast<int64_t>(_impl->candleRects.size()) - index) * axisMilliseconds;

    return total;
}

auto CandleTimeAxisItem::middle(qreal point) const noexcept -> qreal
{
    const auto index = indexFromPoint(point);
    if (index < 0)
        return 0.0;
    return _impl->candleRects[static_cast<std::size_t>(index)].left() + _impl->candleRects[static_cast<std::size_t>(index)].width() / 2;
}

auto CandleTimeAxisItem::indexFromPoint(qreal point) const noexcept -> int64_t
{
    const auto closestIndex = static_cast<int64_t>(std::floor((point - _impl->scaledAxis.left()) / (_impl->scaledAxis.width() / static_cast<qreal>(_impl->candleRects.size()))));
    if (closestIndex < 0 || closestIndex >= static_cast<int64_t>(_impl->candleRects.size()))
        return -1;
    return closestIndex;
}

qreal CandleTimeAxisItem::longestPossibleTagSize(const QFont &font) noexcept
{
    // Even in seconds scale this will be the longest time format
    QString time { "HH:mm" };
    QString month { "MMM" };
    QString year { "yyyy" };

    QFont ft = font;
    ft.setWeight(QFont::Weight::Bold);
    QFontMetrics fontMetrics { ft };

    return std::max({
        fontMetrics.horizontalAdvance(time),
        fontMetrics.horizontalAdvance(month),
        fontMetrics.horizontalAdvance(year),
    });
}

void CandleTimeAxisItem::setAxisFont(const QFont &font) noexcept
{
    _impl->axisFont         = font;
    _impl->maxTagLabelWidth = longestPossibleTagSize(_impl->axisFont);
}

void CandleTimeAxisItem::updateTracker(qreal x) noexcept
{
    if (!isTrackerVisible())
        return;

    QString labelFormat;
    switch (groupTimeFrame(_impl->tf))
    {
        case TimeframeGroup::nullTime: labelFormat = ""; break;
        case TimeframeGroup::Seconds:
            labelFormat = "ddd d MMMM yy HH:mm:ss";
            break;
        case TimeframeGroup::Minutes:
            C_FALLTHROUGH;
        case TimeframeGroup::Hours:
            labelFormat = "ddd d MMMM yy HH:mm";
            break;
        case TimeframeGroup::Days:
            C_FALLTHROUGH;
        case TimeframeGroup::Months:
            labelFormat = "ddd d MMMM yy";
            break;
    }

    auto index              = indexFromPoint(x);
    QDateTime dt            = QDateTime::fromMSecsSinceEpoch(timestampFromPoint(x));
    _impl->trackerRect.text = dt.toString(labelFormat);

    QFontMetrics metric { _impl->axisFont };
    const auto advance = static_cast<qreal>(metric.horizontalAdvance(_impl->trackerRect.text)) + 10.0;

    _impl->trackerRect.rect.setRect(
        (index < 0
                ? x
                : _impl->candleRects[static_cast<std::size_t>(index)].center().x())
            - advance / 2,
        _impl->scaledAxis.top(),
        advance,
        _impl->scaledAxis.height());

    const auto views = scene()->views();
    if (!views.empty())
    {
        const auto view   = views[0];
        const auto point0 = view->mapFromScene(_impl->trackerRect.rect.left(), 0.0);
        const auto point1 = view->mapFromScene(_impl->trackerRect.rect.right(), 0.0);

        if (point0.x() < 0)
            _impl->trackerRect.rect.moveLeft(view->mapToScene(0, 0).x());

        if (point1.x() > view->rect().right() - _impl->priceAxisWidth)
            _impl->trackerRect.rect.moveLeft(
                view->mapToScene(
                        view->rect().right() - static_cast<int>(_impl->priceAxisWidth + _impl->trackerRect.rect.width()), 0)
                    .x());
    }

    update();
}

void CandleTimeAxisItem::hideTracker() noexcept
{
    _impl->showTracker = false;
}

void CandleTimeAxisItem::showTracker() noexcept
{
    _impl->showTracker = true;
}

auto CandleTimeAxisItem::isTrackerVisible() const noexcept -> bool
{
    return _impl->showTracker;
}

bool CandleTimeAxisItem::isTimestampAnchor(int64_t timestamp) noexcept
{
    const QDateTime dt = QDateTime::fromMSecsSinceEpoch(timestamp);

    QTime t = dt.time();
    // Anchor every day
    if (t.second() == 0 && t.minute() == 0 && t.hour() == 0)
        return true;
    /*

        switch (groupTimeFrame(_impl->tf))
        {
            case TimeframeGroup::nullTime:
                return false;
            case TimeframeGroup::Seconds:
                {
                    // Anchor is a whole minute
                    QTime t = dt.time();
                    if (t.second() == 0)
                        return true;
                }
                break;
            case TimeframeGroup::Minutes:
                {
                    // Anchor is every hour
                    QTime t = dt.time();
                    if (t.second() == 0 && t.minute() == 0 && t.hour() % 2 == 0)
                        return true;
                }
                break;
            case TimeframeGroup::Hours:
                {
                    QTime t = dt.time();
                    // Anchor every day
                    if (t.second() == 0 && t.minute() == 0 && t.hour() == 0)
                        return true;
                }
                break;
            case TimeframeGroup::Days:
                {
                    QTime t = dt.time();
                    QDate d = dt.date();

                    // Anchor every month
                    if (d.day() == 1 && t.second() == 0 && t.minute() == 0 && t.hour() == 0)
                        return true;
                }

                break;
            case TimeframeGroup::Months:
                {
                    QTime t = dt.time();
                    QDate d = dt.date();
                    // Anchor every year
                    if (d.month() == 1 && d.day() == 1 && t.second() == 0 && t.minute() == 0 && t.hour() == 0)
                        return true;
                }
                break;
        }*/
    return false;
}

auto CandleTimeAxisItem::getGridLinePositions() const noexcept -> QList<qreal>
{
    QList<double> lines;

    for (const auto &l : _impl->labelRects)
    {
        lines.emplace_back(l.rect.center().x());
    }

    return lines;
}

auto CandleTimeAxisItem::pointFromTimestamp(int64_t timestamp) const noexcept -> qreal
{
    const auto axisMilliseconds = static_cast<qreal>(CandleTimeAxisItem::timeFrameToMilliseconds(_impl->tf));

    const auto sizeReal = static_cast<qreal>(_impl->candleRects.size());
    const auto timeDiff = static_cast<qreal>(timestamp - _impl->linkTimestamp);
    const auto sizeRel  = _impl->scaledAxis.width() / sizeReal;

    const auto index = std::floor((timeDiff / axisMilliseconds) + sizeReal);
    if (index < 0 || static_cast<std::size_t>(index) >= _impl->candleRects.size())
        return -1;

    return _impl->candleRects[static_cast<std::size_t>(index)].center().x();
}

END_CENTAUR_NAMESPACE
