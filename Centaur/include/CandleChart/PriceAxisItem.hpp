/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PRICEAXISITEM_HPP
#define CENTAUR_PRICEAXISITEM_HPP

#include "../../Centaur.hpp"
#include <QBrush>
#include <QFont>
#include <QGraphicsRectItem>
#include <QPen>

namespace CENTAUR_NAMESPACE
{
    class PriceAxisItem : public QGraphicsRectItem
    {
    public:
        PriceAxisItem(double width, double maxLabelHeight, double labelSpacing, double min, double max, QGraphicsItem *parent = nullptr);
        ~PriceAxisItem() override = default;

    public:
        void setWidth(double width) noexcept;
        void setMaxLabelHeight(double height) noexcept;
        void setMinMax(double min, double max) noexcept;
        void setPrecision(int precision) noexcept;

    public:
        inline auto getAxisWidth() const noexcept { return m_width; }
        inline auto getMin() const noexcept { return m_min; }
        inline auto getMax() const noexcept { return m_max; }
        inline auto getPrecision() const noexcept { return m_precision; }

        double priceToAxisPoint(double price);

    public:
        QList<double> getGridLinePositions() noexcept;

    public:
        void mouseInScene(bool scene) noexcept;
        void setYPoint(double point) noexcept;

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    protected:
        double m_width;
        double m_maxLabelHeight;
        double m_min;
        double m_max;
        double m_yPoint;
        double m_maxLabelPosition;

        double m_labelSpacing;

        int m_precision;

        bool m_mouseInScene;

    private:
        QFont m_font;
        QBrush m_barBrush;
        QBrush m_trackFrameBrush;
        QPen m_trackPen;
        QPen m_labelPen;
        QPen m_axisPen;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_PRICEAXISITEM_HPP
