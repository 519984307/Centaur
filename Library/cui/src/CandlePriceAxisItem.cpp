/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandlePriceAxisItem.hpp"
#include <QApplication>
#include <QGraphicsScene>
#include <QLocale>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

BEGIN_CENTAUR_NAMESPACE

static constexpr qreal _fontFactor = 1.5;

namespace
{
    struct AxisTag
    {
        AxisTag(qreal x, qreal y, qreal w, qreal h, QString tag) :
            rect(x, y, w, h),
            text { std::move(tag) },
            price { 0.0 } { }

        void setText(const QString &str) noexcept
        {
            text = str;
        }

        template <typename... T>
        void setRect(T &&...args) noexcept
        {
            rect = QRectF { std::forward<T>(args)... };
        }

        explicit operator QRectF() const noexcept
        {
            return rect;
        }

        explicit operator QString() const noexcept
        {
            return text;
        }

        void setPrice(qreal prc) noexcept
        {
            price = prc;
        }

        QRectF rect;
        QString text;
        qreal price;
    };
} // namespace
struct CandlePriceAxisItem::Impl
{
    Impl() :
        trackerRect { 0, 0, 0, 0, QString() },
        borderPen { QPen(QColor(230, 230, 230), 0.5) },
        orientation { Qt::AlignmentFlag::AlignLeft },
        centerPosition { 0.0 },
        scaleFactor { 1.0 },
        fontHeight { -1.0 },
        priceSteps { 0.0 },
        precision { 3 },
        showTracker { true }
    {
        QFontMetrics metrics { QApplication::font() };
        fontHeight = metrics.height() * _fontFactor;
    }

public:
    std::vector<AxisTag> rectangles;
    AxisTag trackerRect;
    QRectF scaledAxis;
    QPen borderPen;
    Qt::AlignmentFlag orientation;
    qreal centerPosition;
    qreal scaleFactor;
    qreal fontHeight;
    qreal priceSteps;
    int precision;
    bool showTracker;
};

CandlePriceAxisItem::CandlePriceAxisItem(QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    _impl { new CandlePriceAxisItem::Impl }
{
}

void CandlePriceAxisItem::paint(QPainter *painter, C_UNUSED const QStyleOptionGraphicsItem *option, C_UNUSED QWidget *widget)
{
    const QRectF &thisRect  = rect();
    const QRectF &sceneRect = scene()->sceneRect();

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(25, 25, 25));
    painter->drawRect(thisRect);

    // Draw the border
    painter->setPen(_impl->borderPen);
    const auto borderX = orientation() == Qt::AlignRight
                             ? thisRect.left()
                             : thisRect.left() + thisRect.width();
    painter->drawLine(QLineF { borderX, thisRect.top(), borderX, thisRect.height() });

    painter->setBrush(QColor(123, 43, 125, 32));

    for (const auto &data : _impl->rectangles)
    {
        if (QRectF(data).bottom() < sceneRect.top() || QRectF(data).top() > sceneRect.bottom())
        {
            // Don't waste time drawing something that is not visible
            continue;
        }

        //  painter->drawRect(QRectF(data));

        painter->drawText(QRectF(data),
            Qt::AlignVCenter | Qt::AlignHCenter,
            QString(data));
    }

    if (_impl->showTracker)
    {
        painter->setBrush(QColor(0, 0, 0, 255));
        painter->drawRect(QRectF(_impl->trackerRect));
        painter->drawText(QRectF(_impl->trackerRect),
            Qt::AlignVCenter | Qt::AlignHCenter,
            QString(_impl->trackerRect));
    }
}

QList<double> CandlePriceAxisItem::getGridLinePositions() noexcept
{
    QList<double> lines;

    /*   const QRectF &thisRect = rect();

       const double dHeight = thisRect.height();
       const double factor  = m_maxLabelHeight + m_labelSpacing;
       const double labels  = dHeight / factor; // Number of labels (with spacing included) that can fit in the item height

       for (int i = 0; i < static_cast<int>(labels); ++i)
       {
           const double newYPosition = factor * static_cast<double>(i); // New position based on the label index
           lines.emplace_back(thisRect.top() + newYPosition + (m_maxLabelHeight / 2));
       }
   */
    return lines;
}

qreal CandlePriceAxisItem::priceToAxisPoint(qreal price) const noexcept
{
    if (_impl->rectangles.empty())
        return _impl->scaledAxis.top();

    const auto size = static_cast<int64_t>(_impl->rectangles.size());

    int64_t rectangleIndex;
    auto &rects = _impl->rectangles;

    // Calculate the index
    const auto Tt             = static_cast<qreal>(_impl->rectangles.size()) / 2.0;
    const auto Ff             = (_impl->centerPosition - price) / _impl->priceSteps;
    const auto plausibleIndex = static_cast<int64_t>(std::floor(Tt + Ff));

    if (plausibleIndex == 0)
        return _impl->scaledAxis.top();
    else if (plausibleIndex >= static_cast<int64_t>(rects.size()))
        return _impl->scaledAxis.bottom();

    // Check that the index actually is correct
    if (price < rects[static_cast<std::size_t>(plausibleIndex)].price && rects[static_cast<std::size_t>(plausibleIndex - 1)].price >= price)
    {
        // Best case scenario: One simple calculation
        rectangleIndex = plausibleIndex;
        //   qDebug() << plausibleIndex << plausibleIndex - 1 << rectangleIndex;
    }
    else
    {
        qDebug() << "AXIS: WORST CASE";
        // Worst case scenario: plausibleIndex calculation was not calculated well
        if (price >= _impl->centerPosition)
        {
            for (rectangleIndex = size - 2; rectangleIndex >= 0; --rectangleIndex)
            {
                const auto &currentRect = rects[static_cast<std::size_t>(rectangleIndex)];
                if (qFuzzyCompare(price, currentRect.price))
                {
                    return currentRect.rect.top() + currentRect.rect.height() / 2;
                }

                if (rectangleIndex < size)
                {
                    if (currentRect.price >= price && price < rects[static_cast<std::size_t>(rectangleIndex + 1)].price)
                        break;
                }
            }

            if (rectangleIndex < 0)
                return _impl->scaledAxis.bottom();
        }
        else
        {
            for (rectangleIndex = 0ll; rectangleIndex < size - 2; ++rectangleIndex)
            {
                const auto &currentRect = rects[static_cast<std::size_t>(rectangleIndex)];
                if (qFuzzyCompare(price, currentRect.price))
                {
                    return currentRect.rect.top() + currentRect.rect.height() / 2;
                }
                if (rectangleIndex < size)
                {
                    if (currentRect.price >= price && price < rects[static_cast<std::size_t>(rectangleIndex + 1)].price)
                        break;
                }
            }

            if (rectangleIndex > size)
                return _impl->scaledAxis.top();
        }

        rectangleIndex = rectangleIndex + 1;
    }

    const auto &maxRect = rects[static_cast<std::size_t>(rectangleIndex - 1)];
    const auto &minRect = rects[static_cast<std::size_t>(rectangleIndex)];

    const auto maxMid = maxRect.rect.top() + maxRect.rect.height() / 2;
    const auto midMid = minRect.rect.top() + minRect.rect.height() / 2;

    const auto height = midMid - maxMid;

    const qreal pMax = maxRect.price;
    const qreal pMin = minRect.price;

    return (((price - pMax) / (pMin - pMax)) * height) + maxMid;
}

qreal CandlePriceAxisItem::axisPointToPrice(qreal point) const noexcept
{
    const auto closestIndex = static_cast<int64_t>(std::floor((point - _impl->scaledAxis.top()) / (_impl->scaledAxis.height() / static_cast<qreal>(_impl->rectangles.size()))));

    if (closestIndex < 0 || closestIndex >= static_cast<int64_t>(_impl->rectangles.size()))
        return 0.0;

    const auto maxRect = _impl->rectangles[static_cast<std::size_t>(closestIndex)].rect;
    const auto minRect = _impl->rectangles[static_cast<std::size_t>(closestIndex + 1)].rect;

    const auto maxMid = maxRect.top() + maxRect.height() / 2;
    const auto midMid = minRect.top() + minRect.height() / 2;

    const auto height = midMid - maxMid;

    const qreal pMax = _impl->rectangles[static_cast<std::size_t>(closestIndex)].price;
    const qreal pMin = _impl->rectangles[static_cast<std::size_t>(closestIndex + 1)].price;

    return ((((point - maxMid) * (pMin - pMax)) / height) + pMax);
}

void CandlePriceAxisItem::setOrientation(Qt::AlignmentFlag orientation) noexcept
{
    _impl->orientation = orientation;
}

Qt::AlignmentFlag CandlePriceAxisItem::orientation() const noexcept
{
    return _impl->orientation;
}

void CandlePriceAxisItem::centerAt(qreal price) noexcept
{
    _impl->centerPosition = price;
}

void CandlePriceAxisItem::setBorderPen(const QPen &pen)
{
    _impl->borderPen = pen;
}

qreal CandlePriceAxisItem::center() noexcept
{
    const QRectF &thisRect = rect();
    return thisRect.top() + thisRect.height() / 2.0;
}

void CandlePriceAxisItem::scalePrice(qreal factor) noexcept
{
    _impl->scaleFactor = factor > 0 ? _impl->scaleFactor + 2.5 : _impl->scaleFactor -= 2.5;

    calculateRectangles();

    update();
}

void CandlePriceAxisItem::setPriceStep(qreal log, qreal dec) noexcept
{
    if (log < 1)
    {
        _impl->precision  = static_cast<int>(dec);
        _impl->priceSteps = std::pow(10, -(dec + 1)) * _impl->fontHeight;
    }
    else
    {
        if (qFuzzyCompare(log, 1))
            _impl->precision = 2; // Keep precision of 2 for prices less than 10
        else
            _impl->precision = 1; // Keep precision of 1 for prices higher than 100

        const qreal step  = log * _impl->fontHeight;
        const qreal mod   = static_cast<int>(step) % 5; // Keep the increments in multiples of 5
        _impl->priceSteps = step - mod;
    }
}

void CandlePriceAxisItem::updateTracker(qreal y) noexcept
{
    const auto price = axisPointToPrice(y);

    _impl->trackerRect.setText(QString("%1").arg(
        price,
        0,
        'f',
        _impl->precision));
    /*
        qDebug() << QString("%1").arg(
            price,
            0,
            'f',
            _impl->precision);*/

    const auto halfHeight = _impl->fontHeight / 2.0;
    QRectF &rect          = _impl->trackerRect.rect;
    rect                  = _impl->scaledAxis;
    rect.setTop(y - halfHeight);
    rect.setHeight(_impl->fontHeight);

    update();
}

void CandlePriceAxisItem::calculateRectangles() noexcept
{
    const QRectF &thisRect = rect();

    const auto borderX = orientation() == Qt::AlignRight
                             ? thisRect.left()
                             : thisRect.left() + thisRect.width();

    // Left position all labels
    const auto labelsLeft  = orientation() == Qt::AlignRight
                                 ? borderX
                                 : thisRect.left();
    const auto labelsWidth = thisRect.width();
    const auto &fontHeight = _impl->fontHeight;

    // Determinate the center
    const auto totalTags = static_cast<std::size_t>(std::ceil(thisRect.height() / fontHeight));

    bool reparseString = false;

    if (!totalTags)
        return;

    if (_impl->rectangles.size() < totalTags)
    {
        reparseString = true;
        _impl->rectangles.reserve(totalTags);

        auto insert = std::max(static_cast<std::size_t>(0), totalTags - _impl->rectangles.size());
        for (decltype(insert) k = 0; k < insert; ++k)
            _impl->rectangles.emplace_back(0, 0, 0, 0, QString());
    }

    if (_impl->scaleFactor < 1)
    {
        // Top scaling factor is:
        // ((Ts - H) / f) + 1
        // Where:
        //      s = fontHeight * 3
        //      T = total amount of frames
        //      H = Window height
        // It needs calculation because it depends on H which is dynamic
        const auto T  = static_cast<qreal>(_impl->rectangles.size());
        const auto H  = thisRect.height();
        const auto &f = _impl->fontHeight;
        const auto s  = f * 2;

        _impl->scaleFactor = 1 + ((T * s - H) / f);
        _impl->priceSteps *= 2;
        reparseString = true;
    }

    // The zooming will have the effect that the Window Height is expanding
    // We'll expand the top and the bottom equally and to preserve the zoom factor we'll divide by two
    _impl->scaledAxis = thisRect;
    _impl->scaledAxis.setTop(_impl->scaledAxis.top() - fontHeight * (_impl->scaleFactor - 1) / 2);
    _impl->scaledAxis.setBottom(_impl->scaledAxis.bottom() + fontHeight * (_impl->scaleFactor - 1) / 2);

    auto spacing = _impl->scaledAxis.height() / static_cast<qreal>(totalTags);

    if (spacing > fontHeight * 2)
    {
        _impl->scaleFactor = 1.0;

        _impl->priceSteps /= 2;

        // Recalculate
        _impl->scaledAxis.setTop(_impl->scaledAxis.top() - fontHeight * (_impl->scaleFactor - 1) / 2);
        _impl->scaledAxis.setBottom(_impl->scaledAxis.bottom() + fontHeight * (_impl->scaleFactor - 1) / 2);
        spacing       = _impl->scaledAxis.height() / static_cast<qreal>(totalTags);
        reparseString = true;
    }

    const auto centerPoint = totalTags / 2;

    for (auto i = 0ull; i < totalTags; ++i)
    {
        const auto idx = static_cast<qreal>(i);

        const auto top = _impl->scaledAxis.top() + idx * spacing;

        _impl->rectangles[i].setRect(labelsLeft, top, labelsWidth, fontHeight);

        if (reparseString)
        {
            _impl->rectangles[i].setPrice(_impl->centerPosition - static_cast<qreal>(static_cast<int64_t>(i) - static_cast<int64_t>(centerPoint)) * _impl->priceSteps);
            _impl->rectangles[i].setText(QString("%1").arg(
                _impl->rectangles[i].price,
                0,
                'f',
                _impl->precision));
        }
    }
}

void CandlePriceAxisItem::showTracker() noexcept
{
    _impl->showTracker = true;
    update();
}

void CandlePriceAxisItem::hideTracker() noexcept
{
    _impl->showTracker = false;
    update();
}
bool CandlePriceAxisItem::isTrackerVisible() const noexcept
{
    return _impl->showTracker;
}

END_CENTAUR_NAMESPACE
