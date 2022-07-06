/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChart/PriceAxisItem.hpp"
#include <QLocale>
#include <QPainter>

cen::PriceAxisItem::PriceAxisItem(double width, double maxLabelHeight, double min, double max, QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    m_width { width },
    m_maxLabelHeight { maxLabelHeight },
    m_min { min },
    m_max { max },
    m_yPoint { 0 },
    m_maxLabelPosition { 0 },
    m_precision { 3 },
    m_mouseInScene { false },
    m_font { "Roboto", 12 }

{

    m_labelSpacing = 10;

    m_barBrush.setColor(QColor(40, 40, 40));
    m_barBrush.setStyle(Qt::SolidPattern);

    m_trackFrameBrush.setColor(QColor(80, 80, 80));
    m_trackFrameBrush.setStyle(Qt::SolidPattern);

    m_trackPen.setColor(QColor(220, 220, 200));
    m_axisPen.setColor(QColor(150, 150, 150));
    m_labelPen.setColor(QColor(220, 220, 200));
    // Avoid scaling
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

void cen::PriceAxisItem::paint(QPainter *painter, C_UNUSED const QStyleOptionGraphicsItem *option, C_UNUSED QWidget *widget)
{
    painter->setFont(m_font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_barBrush);

    const QRectF &thisRect = rect();
    painter->drawRect(thisRect);

    const double dHeight    = thisRect.height();
    const double factor     = m_maxLabelHeight + m_labelSpacing;
    const double labels     = dHeight / factor;                                                    // Number of labels (with spacing included) that can fit in the item height
    const double increment  = (m_max - m_min) / static_cast<double>(static_cast<int>(labels - 1)); // Increment to m_min or m_max so the first label is m_min and the m_max

    double currentAxisValue = m_max;

    for (int i = 0; i < static_cast<int>(labels); ++i)
    {
        const double newYPosition = factor * static_cast<double>(i); // New position based on the label index
        const QString text        = QString("%1").arg(QLocale(QLocale::English).toString(currentAxisValue, 'f', m_precision));
        const QRectF textRect { thisRect.left(), thisRect.top() + newYPosition, thisRect.width(), m_maxLabelHeight };

        /// painter->setBrush(QBrush(QColor(255, 0, 255, 120)));
        /// painter->drawRect(textRect);

        painter->setPen(m_labelPen);
        painter->drawText(textRect, Qt::AlignVCenter, text);

        currentAxisValue -= increment;

        m_maxLabelPosition = thisRect.top() + newYPosition + (m_maxLabelHeight / 2);
    }

    painter->setPen(m_axisPen);
    painter->drawLine(QLineF { thisRect.left(), thisRect.top(), thisRect.left(), dHeight });

    if (m_mouseInScene)
    {
        // Draw the price
        const double half          = m_maxLabelHeight / 2.0;
        const double minHeight     = thisRect.top() + half;
        const double rangeFactor   = (m_max - m_min);
        const double invertedPoint = m_maxLabelPosition + minHeight - m_yPoint; // As Y grows the price grows, however, the axis is inverted, so, we must invert the Y value

        const double priceValue    = (rangeFactor * (invertedPoint - minHeight) / (m_maxLabelPosition - minHeight)) + m_min;

        const QRectF rc { thisRect.left(), m_yPoint - half, thisRect.width(), m_maxLabelHeight };

        painter->setPen(Qt::NoPen); // No frame rectangle
        painter->setBrush(m_trackFrameBrush);

        painter->drawRect(rc);

        painter->setPen(m_trackPen);

        const QString priceText = QString("%1").arg(QLocale(QLocale::English).toString(priceValue, 'f', m_precision));
        painter->drawText(rc, priceText);
    }

    //   QGraphicsRectItem::paint(painter, option, widget);
}

void cen::PriceAxisItem::setWidth(double width) noexcept
{
    m_width       = width;
    auto axisRect = rect();
    axisRect.setWidth(m_width);
    setRect(axisRect);
}

void cen::PriceAxisItem::setMaxLabelHeight(double height) noexcept
{
    m_maxLabelHeight = height;
    update();
}

void cen::PriceAxisItem::setMinMax(double min, double max) noexcept
{
    m_min = min;
    m_max = max;
    update();
}

void cen::PriceAxisItem::setPrecision(int precision) noexcept
{
    m_precision = precision;
    update();
}

void cen::PriceAxisItem::mouseInScene(bool scene) noexcept
{
    m_mouseInScene = scene;
    update();
}

void cen::PriceAxisItem::setYPoint(double point) noexcept
{
    m_yPoint = point;
    update();
}

QList<double> cen::PriceAxisItem::getGridLinePositions() noexcept
{
    QList<double> lines;
    const QRectF &thisRect = rect();

    const double dHeight   = thisRect.height();
    const double factor    = m_maxLabelHeight + m_labelSpacing;
    const double labels    = dHeight / factor; // Number of labels (with spacing included) that can fit in the item height

    for (int i = 0; i < static_cast<int>(labels); ++i)
    {
        const double newYPosition = factor * static_cast<double>(i); // New position based on the label index
        lines.emplace_back(thisRect.top() + newYPosition + (m_maxLabelHeight / 2));
    }

    return lines;
}
double cen::PriceAxisItem::priceToAxisPoint(double price)
{
    double point;

    const QRectF &thisRect   = rect();
    const double half        = m_maxLabelHeight / 2.0;
    const double minHeight   = thisRect.top() + half;
    const double rangeFactor = (m_max - m_min);
    // const double invertedPoint = m_maxLabelPosition + minHeight - m_yPoint; // As Y grows the price grows, however, the axis is inverted, so, we must invert the Y value

    /// const double priceValue    = (rangeFactor * (invertedPoint - minHeight) / (m_maxLabelPosition - minHeight)) + m_min;

    point = -((((price - m_min) * (m_maxLabelPosition - minHeight)) / rangeFactor) - m_maxLabelPosition);

    return point;
}
