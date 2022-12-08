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
        AxisTag() = default;

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
        trackerRect {},
        borderPen { QPen(QColor(230, 230, 230), 0.5) },
        orientation { Qt::AlignmentFlag::AlignLeft },
        maxPrice { 0.0 },
        minPrice { 0.0 },
        scaleFactor { 0.0 },
        fontHeight { -1.0 },
        priceSteps { 0.0 },
        lastSpacingMax { 0.0 },
        lastSpacingMin { 0.0 },
        scaleFrameAdjustment { -1 },
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
    qreal maxPrice;
    qreal minPrice;
    qreal scaleFactor;
    qreal fontHeight;
    qreal priceSteps;

    qreal lastSpacingMax;
    qreal lastSpacingMin;

    int64_t scaleFrameAdjustment;
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

    std::size_t index = 0;
    for (const auto &data : _impl->rectangles)
    {
        if (_impl->rectangles.size() % 2 == 0)
        {
            if (index == _impl->rectangles.size() / 2 || index == ((_impl->rectangles.size() / 2) - 1))
            {
                painter->setBrush(QColor(43, 123, 125, 32));
            }
            else
                painter->setBrush(QColor(123, 43, 125, 32));
        }
        else
        {
            if (index == _impl->rectangles.size() / 2)
            {
                painter->setBrush(QColor(43, 123, 125, 32));
            }
            else
                painter->setBrush(QColor(123, 43, 125, 32));
        }

        ++index;

        if (QRectF(data).bottom() < sceneRect.top() || QRectF(data).top() > sceneRect.bottom())
        {
            // Don't waste time drawing something that is not visible
            continue;
        }

        painter->drawRect(QRectF(data));

        painter->drawText(QRectF(data),
            Qt::AlignVCenter | Qt::AlignHCenter,
            QString(data));
    }
    /*
        if (_impl->showTracker)
        {
            painter->setBrush(QColor(0, 0, 0, 255));
            painter->drawRect(QRectF(_impl->trackerRect));
            painter->drawText(QRectF(_impl->trackerRect),
                Qt::AlignVCenter | Qt::AlignHCenter,
                QString(_impl->trackerRect));
        }*/
}

QList<double> CandlePriceAxisItem::getGridLinePositions() noexcept
{
    QList<double> lines;

    for (const auto &l : _impl->rectangles)
    {
        lines.emplace_back(l.rect.center().y());
    }

    return lines;
}

qreal CandlePriceAxisItem::priceToAxisPoint(qreal price) const noexcept
{
    if (_impl->rectangles.empty())
        return _impl->scaledAxis.top();

    // Calculate the index
    const auto Tt             = static_cast<qreal>(_impl->rectangles.size()) / 2.0;
    const auto Ff             = 1.0; //(_impl->centerPosition - price) / _impl->priceSteps;
    const auto rectangleIndex = static_cast<int64_t>(std::floor(Tt + Ff));

    if (rectangleIndex == 0)
        return _impl->scaledAxis.top();
    else if (rectangleIndex >= static_cast<int64_t>(_impl->rectangles.size()))
        return _impl->scaledAxis.bottom();

    auto &tag = _impl->rectangles[static_cast<std::size_t>(rectangleIndex)];

    const qreal indexPriceMin = tag.price - _impl->priceSteps / 2;
    const qreal indexPriceMax = tag.price + _impl->priceSteps / 2;

    return (((price - indexPriceMax) / (indexPriceMin - indexPriceMax)) * tag.rect.height()) + tag.rect.top();
}

qreal CandlePriceAxisItem::axisPointToPrice(qreal point) const noexcept
{
    if (_impl->rectangles.empty())
        return 0.0;

    const auto closestIndex = static_cast<int64_t>(std::floor((point - _impl->scaledAxis.top()) / (_impl->scaledAxis.height() / static_cast<qreal>(_impl->rectangles.size()))));

    if (closestIndex < 0 || closestIndex >= static_cast<int64_t>(_impl->rectangles.size()))
        return 0.0;

    auto &tag = _impl->rectangles[static_cast<std::size_t>(closestIndex)];

    const qreal indexPriceMin = tag.price - _impl->priceSteps / 2;
    const qreal indexPriceMax = tag.price + _impl->priceSteps / 2;

    return ((((point - tag.rect.top()) * (indexPriceMin - indexPriceMax)) / tag.rect.height()) + indexPriceMax);
}

void CandlePriceAxisItem::setOrientation(Qt::AlignmentFlag orientation) noexcept
{
    _impl->orientation = orientation;
}

Qt::AlignmentFlag CandlePriceAxisItem::orientation() const noexcept
{
    return _impl->orientation;
}

void CandlePriceAxisItem::setBorderPen(const QPen &pen)
{
    _impl->borderPen = pen;
}

qreal CandlePriceAxisItem::center() noexcept
{
    const QRectF &thisRect = rect();
    return thisRect.center().y();
}

void CandlePriceAxisItem::scalePrice(qreal factor) noexcept
{ /*
     const qreal growingRate
         = _impl->rectangles.size() % 2
               ? 60
               : 15;*/

    const qreal growingRate
        = _impl->rectangles.size() % 2
              ? 20
              : 5;

    factor > 0
        ? _impl->scaleFactor += growingRate
        : _impl->scaleFactor -= growingRate;

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
            _impl->precision = 3; // Keep precision of 2 for prices less than 10
        else
            _impl->precision = 2; // Keep precision of 1 for prices higher than 100

        const qreal step  = log * _impl->fontHeight;
        const qreal mod   = static_cast<int>(step) % 5; // Keep the increments in multiples of 5
        _impl->priceSteps = step - mod;
    }

    _impl->priceSteps = 0.0;
}

void CandlePriceAxisItem::updateTracker(qreal y) noexcept
{
    const auto price = axisPointToPrice(y);

    _impl->trackerRect.setText(QString("%1").arg(
        price,
        0,
        'f',
        _impl->precision));

    const auto halfHeight = _impl->fontHeight / 2.0;
    QRectF &rect          = _impl->trackerRect.rect;
    rect                  = _impl->scaledAxis;
    rect.setTop(y - halfHeight);
    rect.setHeight(_impl->fontHeight);
    rect.setLeft(rect.left() + 20);

    update();
}

void CandlePriceAxisItem::calculateRectangles() noexcept
{
    QRectF thisRect = rect();
    const auto cent = thisRect.center().y();
    thisRect.setTop(cent - 48);
    thisRect.setBottom(cent + 48);

    const auto borderX = orientation() == Qt::AlignRight
                             ? thisRect.left()
                             : thisRect.left() + thisRect.width();

    // Left position all labels
    const auto labelsLeft = orientation() == Qt::AlignRight
                                ? borderX
                                : thisRect.left();

    const auto labelsWidth = thisRect.width();
    const auto &fontHeight = _impl->fontHeight;

    // _TotalTags is the total tags
    // The resizing changes depending on the oddity of the number
    auto dTotalTags = std::ceil(thisRect.height() / fontHeight);
    if (!qFuzzyCompare(fmod(dTotalTags, 2), 0))
        dTotalTags += 1; // Always start at an even number of tags
    const qreal _TotalTags = dTotalTags;
    auto uTotalTags        = static_cast<std::size_t>(dTotalTags);

    // priceSpacing must be constant
    const auto priceFactor = ((_impl->maxPrice - _impl->minPrice) / dTotalTags);

    if (!uTotalTags)
        return;

    if (_impl->scaleFrameAdjustment == -1)
    {
        //  _impl->scaleFrameAdjustment
        // Will hold 0 when uTotalTags is zero and 1 otherwise
        // This variable will reference the actual resizing step
        _impl->scaleFrameAdjustment = uTotalTags % 2 ? 1 : 0;
    }

    // maxAxisSizeEven is the maximum size that the axis can grow when
    // we are scaling an even number of tags.
    // When scaledAxis.height() reaches this point we must add a new number of tags
    const qreal maxAxisSizeEven = dTotalTags * 2 * fontHeight;
    // maxTagsAfterScalingEven is the number of tags that will be added after the height of the scaled axis
    // reaches maxAxisSizeEven
    const qreal maxOddTagsNumber = dTotalTags + (dTotalTags - 1);
    // In order to keep all the tags (maxTagsAfterScalingEven) this must be the axis size to keep the aspect ratio unchanged of the
    // tags
    const qreal actualEvenAxisSizeAfterEvenResizing = maxOddTagsNumber * fontHeight;

    // axisHeightForOddTagNumber
    // When the axis height reach this number
    // We must reset the number all the parameters to an axis of even-number of tags
    const qreal axisHeightForOddTagNumber = maxOddTagsNumber * 2 * fontHeight;

    bool adjustPrices = false;
    if (_impl->scaleFrameAdjustment == 0)
    {
        _impl->scaledAxis = thisRect;
        _impl->scaledAxis.setTop(_impl->scaledAxis.top() - _impl->scaleFactor);
        _impl->scaledAxis.setBottom(_impl->scaledAxis.bottom() + _impl->scaleFactor);

        if (_impl->scaledAxis.height() >= maxAxisSizeEven)
        {
            // qDebug() << _impl->scaledAxis.height() << maxAxisSizeEven << _impl->scaleFactor << (maxAxisSizeEven - thisRect.height()) / 2;

            dTotalTags = maxOddTagsNumber;
            uTotalTags = static_cast<std::size_t>(dTotalTags);

            // We are now in the mode of an odd-number of tags
            _impl->scaleFrameAdjustment = 1;

            // At this point the base size of axis rect must change to actualEvenAxisSizeAfterEvenResizing
            const qreal frameVerticalCenter = thisRect.center().y();
            const qreal midGrow             = actualEvenAxisSizeAfterEvenResizing / 2;
            _impl->scaledAxis.setTop(frameVerticalCenter - midGrow);
            _impl->scaledAxis.setBottom(frameVerticalCenter + midGrow);

            // And reset the scaleFactor
            _impl->scaleFactor = 0.0;

            _impl->priceSteps += 1.0;
        }
        else if (_impl->scaledAxis.height() < thisRect.height())
        {
            // Set the axis as if it were the max height in odd-number tag

            // Main constants
            const qreal frameVerticalCenter = thisRect.center().y();
            const qreal midGrow             = actualEvenAxisSizeAfterEvenResizing / 2;
            const qreal growRate            = (dTotalTags - 1) / dTotalTags;

            // Condition of when the scaleFactor is maximum
            _impl->scaleFactor = (growRate * (axisHeightForOddTagNumber - actualEvenAxisSizeAfterEvenResizing)) / (1 + std::pow(growRate, 2));

            // Set the tags to the odd-number mode
            dTotalTags = maxOddTagsNumber;
            uTotalTags = static_cast<std::size_t>(dTotalTags);

            // Set the axis max size
            _impl->scaledAxis.setTop(frameVerticalCenter - midGrow - (_impl->scaleFactor * growRate) - fontHeight /* -3091.7891389028882 */);
            _impl->scaledAxis.setBottom(frameVerticalCenter + midGrow + (_impl->scaleFactor * 1 / growRate) /* 5'162.1286232062 */);

            // Set the odd-number mode
            _impl->scaleFrameAdjustment = 1;
        }
    }
    else
    {
        dTotalTags = maxOddTagsNumber;
        uTotalTags = static_cast<std::size_t>(dTotalTags);

        // At this point the base size of axis rect must change to actualEvenAxisSizeAfterEvenResizing
        const qreal frameVerticalCenter = thisRect.center().y();
        const qreal midGrow             = actualEvenAxisSizeAfterEvenResizing / 2;

        // however, when resizing the axis with
        // an odd-number of tags, the top-point of the middle frame
        // must remain unchanged, so after careful analysis,
        // to fulfill this condition, no matter what the size of the viewport is
        // or the height of each frame, the axis top-point must grow at  [(dTotalTags - 1)/dTotalTags] of that
        // of the bottom-point. And the bottom-point must grow at the inverse [dTotalTags/(dTotalTags - 1)] rate

        const qreal growRate = (dTotalTags - 1) / dTotalTags;

        // The main difference from the _impl->scaleFrameAdjustment == 0 is that we must take into account the _impl->scaleFactor
        _impl->scaledAxis.setTop(frameVerticalCenter - midGrow - (_impl->scaleFactor * growRate));
        _impl->scaledAxis.setBottom(frameVerticalCenter + midGrow + (_impl->scaleFactor * 1 / growRate));

        // qDebug() << _impl->scaledAxis.height() << axisHeightForOddTagNumber;
        if (_impl->scaledAxis.height() >= (axisHeightForOddTagNumber - fontHeight))
        {
            dTotalTags = std::ceil(thisRect.height() / fontHeight);
            if (!qFuzzyCompare(fmod(dTotalTags, 2), 0))
                dTotalTags += 1; // Always start at an even number of tags
            uTotalTags = static_cast<std::size_t>(dTotalTags);

            // We are now in the mode of an even-number of tags
            _impl->scaleFrameAdjustment = 0;

            // At this point the base size of axis rect must change to actualEvenAxisSizeAfterEvenResizing
            _impl->scaledAxis = thisRect;
            _impl->scaledAxis.setTop(_impl->scaledAxis.top());
            _impl->scaledAxis.setBottom(_impl->scaledAxis.bottom());

            // And reset the scaleFactor
            _impl->scaleFactor = 0.0;

            _impl->priceSteps += 1.0;

            adjustPrices = true;
        }
        else if (_impl->scaledAxis.height() < maxAxisSizeEven)
        {
            // Max scale factor when the tags are even
            _impl->scaleFactor = (maxAxisSizeEven - thisRect.height()) / 2;

            // Set the tags to the odd-number mode
            dTotalTags = std::ceil(thisRect.height() / fontHeight);
            if (!qFuzzyCompare(fmod(dTotalTags, 2), 0))
                dTotalTags += 1; // Always start at an even number of tags
            uTotalTags = static_cast<std::size_t>(dTotalTags);

            // Adjust the axis
            _impl->scaledAxis = thisRect;
            _impl->scaledAxis.setTop(_impl->scaledAxis.top() - _impl->scaleFactor);
            _impl->scaledAxis.setBottom(_impl->scaledAxis.bottom() + _impl->scaleFactor);

            // Set the even-number mode
            _impl->scaleFrameAdjustment = 0;
        }
    }

    qreal priceSpacing = 0;
    qreal actualMax    = _impl->lastSpacingMax;
    if (_impl->priceSteps > 0)
    {
        if (_impl->scaleFrameAdjustment)
        {
            // Expansion was set to a
            const auto normalPriceSpacing = (_impl->maxPrice - _impl->minPrice) / (dTotalTags - 1);

            const qreal maxPrice = _impl->maxPrice + (normalPriceSpacing * (_impl->priceSteps - 1));
            const qreal minPrice = _impl->minPrice - (normalPriceSpacing * (_impl->priceSteps - 1));
            priceSpacing         = (maxPrice - minPrice) / (dTotalTags - 1);
        }
        else
        {

            qDebug() << "T:"<< _impl->lastSpacingMax << _impl->lastSpacingMin;

            const auto normalPriceSpacing = (_impl->lastSpacingMax - _impl->lastSpacingMin) / (_TotalTags - 1);

            const qreal maxPrice = _impl->lastSpacingMax - (normalPriceSpacing * (_impl->priceSteps - 2));
            const qreal minPrice = _impl->lastSpacingMin + (normalPriceSpacing * (_impl->priceSteps - 2));

            priceSpacing = (maxPrice - minPrice) / (_TotalTags - 1);

            actualMax = maxPrice;

            if (adjustPrices)
            {
                _impl->lastSpacingMax = maxPrice;
                _impl->lastSpacingMin = minPrice;

                qDebug() << _impl->lastSpacingMax << _impl->lastSpacingMin;
            }
        }
    }
    else
    {
        priceSpacing = (_impl->maxPrice - _impl->minPrice) / (_TotalTags - 1);
    }

    /*
        auto priceSpacing = qFuzzyCompare(totalTagsByExpansions, 0)
                                ? (_impl->maxPrice - _impl->minPrice) / (_TotalTags - 1)
                                : (_impl->maxPrice - _impl->minPrice) / (totalTagsByExpansions); //+ ( priceFactor * _impl->priceSteps );
    */
    //   const auto indexingFactor = std::floor(qFuzzyCompare(totalTagsByExpansions, 0) ? 0 : std::max(0.0, totalTagsFactorByExpansions - dTotalTags / (_impl->priceSteps + 1)));

    //   qDebug() << priceFactor << priceSpacing << _TotalTags << totalTagsFactorByExpansions << _impl->priceSteps << totalTagsByExpansions << indexingFactor;

    bool reparseString = false;
    if (_impl->rectangles.size() != uTotalTags)
    {
        reparseString = true;
        _impl->rectangles.clear();
        _impl->rectangles.reserve(uTotalTags);
        for (std::remove_cv<decltype(uTotalTags)>::type k = 0; k < uTotalTags; ++k)
            _impl->rectangles.emplace_back();
    }

    const qreal actualFrameHeight = _impl->scaledAxis.height() / dTotalTags;

    qreal top = _impl->scaledAxis.top();
    for (auto i = 0ull; i < uTotalTags; ++i)
    {

        const auto idx = static_cast<qreal>(i) /*+ indexingFactor */;
        _impl->rectangles[i].setRect(labelsLeft, top, labelsWidth, actualFrameHeight);

        const auto price = actualMax - priceSpacing * idx;

        if (reparseString || !qFuzzyCompare(price, _impl->rectangles[i].price))
        {
            _impl->rectangles[i].setPrice(price);
            _impl->rectangles[i].setText(QString("%1").arg(
                price,
                0,
                'f',
                _impl->precision));
        }

        top += actualFrameHeight;
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

void CandlePriceAxisItem::setAxisMaxPrice(qreal max) noexcept
{
    setAxisMinMaxPrice(_impl->minPrice, max);
}

void CandlePriceAxisItem::setAxisMinPrice(qreal min) noexcept
{
    setAxisMinMaxPrice(min, _impl->maxPrice);
}

void CandlePriceAxisItem::setAxisMinMaxPrice(qreal min, qreal max) noexcept
{
    if (min > max)
        return;
    _impl->maxPrice = max;
    _impl->minPrice = min;

    _impl->lastSpacingMin = min;
    _impl->lastSpacingMax = max;

    calculateRectangles();
}

END_CENTAUR_NAMESPACE
