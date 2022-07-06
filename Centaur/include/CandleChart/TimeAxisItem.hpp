/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 05/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_TIMEAXISITEM_HPP
#define CENTAUR_TIMEAXISITEM_HPP

#include "../../Centaur.hpp"
#include <CentaurPlugin.hpp>
#include <QFont>
#include <QGraphicsRectItem>

namespace CENTAUR_NAMESPACE
{
    class TimeAxisItem : public QGraphicsRectItem
    {
        using Timeframe = CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame;

    public:
        /// Labels will be set based on the timeframe
        TimeAxisItem(double height, double candleWidth, double candleSpacing, uint64_t min, uint64_t max, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf, QGraphicsItem *parent = nullptr);
        ~TimeAxisItem() override = default;

    public:
        void setHeight(double height) noexcept;
        void setCandleWidth(double width) noexcept;
        void setCandleSpacing(double spacing) noexcept;
        void setTimestampRange(uint64_t min, uint64_t max) noexcept;
        void setTimeFrame(CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf) noexcept;

    public:
        void mouseInScene(bool scene);

    public:
        /// \brief The idea of fit is to return the number candles that fit between min and max based on the timeframe
        /// and calculating with the candleWidth and the spacing
        /// \return the number of candles that fit in the axis with the corresponding parameters
        uint64_t fit() const;

    public:
        /// \brief Determines if the X point is within one the candle frames and return the center of that frame
        double isXPointInCandleFrame(double X) noexcept;

    public:
        void calculateCandleRects() noexcept;

    public:
        inline double getHeight() const noexcept { return m_height; }

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    protected:
        void recalculateAxisLabels();

    protected:
        double m_height;
        double m_candleWidth;
        double m_spacing;
        uint64_t m_min;
        uint64_t m_max;
        Timeframe m_tf;
        bool m_mouseInScene;

    private:
        QList<QRectF> m_candleRects;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_TIMEAXISITEM_HPP
