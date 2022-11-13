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

#include "Centaur.hpp"
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
        TimeAxisItem(double height, double candleWidth, double candleSpacing, double labelSpacing, uint64_t min, uint64_t max, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf, QGraphicsItem *parent = nullptr);
        ~TimeAxisItem() override = default;

    public:
        void setHeight(double height) noexcept;
        void setCandleWidth(double width) noexcept;
        void setCandleSpacing(double spacing) noexcept;
        void setCandleParameters(double width, double spacing) noexcept;
        void setLabelSpacing(double spacing) noexcept;
        void setTimestampRange(uint64_t min, uint64_t max) noexcept;
        void setTimeFrame(CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf) noexcept;
        /// The timestamp can be the uint64_t parameter returned by isXPointInCandleFrame
        void setCursorPos(uint64_t timestamp) noexcept;

    public:
        C_NODISCARD inline auto getMin() const noexcept { return m_min; }
        C_NODISCARD inline auto getMax() const noexcept { return m_max; }
        C_NODISCARD inline auto getTimeframe() const noexcept { return m_tf; }
        C_NODISCARD inline auto getCandleWidth() const noexcept { return m_candleWidth; }
        C_NODISCARD inline auto getCandleSpacing() const noexcept { return m_candleSpacing; }

    public:
        void mouseInScene(bool scene);

    public:
        /// \brief The idea of fit is to return the number candles that fit between min and max based on the timeframe
        /// and calculating with the candleWidth and the spacing
        /// \return the number of candles that fit in the axis with the corresponding parameters
        uint64_t fit() const;

    public:
        /// \brief Determines if the X point is within one the candle frames and return the center of that frame
        /// \return The double parameter is the X position relative to the origin and uint64_t parameter is the timestamp of the candle and can be used to set the cursor pos
        QPair<double, uint64_t> isXPointInCandleFrame(double X) noexcept;

        QList<double> getGridLinePositions() noexcept;

        QPair<double, bool> getGridFrame(uint64_t timestamp);

    public:
        void calculateCandleRects() noexcept;
        void recalculateAxisLabels() noexcept;

    public:
        static uint64_t timeFrameToMilliseconds(cen::plugin::ICandleView::TimeFrame tf) noexcept;

    public:
        inline double getHeight() const noexcept { return m_height; }

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    protected:
        double getAxisLabelsWidth(bool retrievingMode = false, QList<QPair<QString, QFont *>> *labels = nullptr, uint64_t startAtTimestamp = 0, uint64_t numberOfLabels = 0, uint64_t shifts = 0) noexcept;

    protected:
        double m_height;
        double m_candleWidth;
        double m_candleSpacing;
        double m_labelSpacing;
        uint64_t m_min;
        uint64_t m_max;
        uint64_t m_absoluteMin;
        Timeframe m_tf;
        uint64_t m_cursorIndex;
        bool m_mouseInScene;

    private:
        QString m_infoLabel;
        QRectF m_infoRect;

    private:
        QList<double> m_gridLines;
        std::map<uint64_t, QRectF> m_candleRects;
        // QList<QPair<QRectF, uint64_t>> m_candleRects;
        QList<std::tuple<QString, QRectF, QFont *>> m_labelRects;

    private:
        QFont m_normalFont;
        QFont m_boldFont;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_TIMEAXISITEM_HPP
