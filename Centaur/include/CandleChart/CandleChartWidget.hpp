/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 02/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CANDLECHARTINFOWIDGET_HPP
#define CENTAUR_CANDLECHARTINFOWIDGET_HPP

#include "../../Centaur.hpp"

#include "CandleChart/CandleItem.hpp"
#include "CandleChart/PriceAxisItem.hpp"
#include "CandleChart/TimeAxisItem.hpp"

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsView>

namespace CENTAUR_NAMESPACE
{

    /// \brief This Is the basis for the chart widget
    // Note that scrolling and scaling is not based on moving the or scaling the scene
    //
    // The vertical scrolling is based on moving the axis min and max and adjusting the H/L/O/C points associated to the candle
    // Noting that the vertical scrolling is based on the mouse speed
    //
    // The horizontal axis is based on fragmenting the axis in rectangle that fit a candled calculating based on the candle width and spacing
    // And each one of these rectangles have a timestamp associated.
    //
    // The horizontal scrolling is based on moving the axis min (moving the minimum timestamp) so when the axis is scrolled we only display
    // the candles fit within the min and max timestamps of the view
    //
    // The scaling is done similarly.
    // The horizontal scaling is based on scaling the candle and width and scaling up maintaining the candle viewable when shrinking
    // and not making it too big
    //
    // The vertical scaling is based on moving the min and max closer together when zooming in and make them apart from each other when zooming out

    class CandleChartScene;

    class CandleChartWidget : public QGraphicsView
    {
        Q_OBJECT
    public:
        explicit CandleChartWidget(QWidget *parent = nullptr);
        ~CandleChartWidget() override = default;

    public:
        /// Set the chart timeframe
        void setChartTimeFrame(CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf);
        /// \brief Updating a non-existent candle will add the candle
        void updateCandle(quint64 timestamp, double open, double close, double high, double low) noexcept;
        /// \brief Adding an existing timestamp will call to updateCandle
        /// addCandle will also adjust the priceAxisWidth
        void addCandle(quint64 timestamp, double open, double close, double high, double low) noexcept;
        /// \brief Set vertical min max
        void setPriceMinMax(double min, double max) noexcept;
        /// Set the horizontal axis min or max
        void setTimeMinMax(uint64_t min, uint64_t max) noexcept;

        void updatePriceAxisMetrics(double open, double close, double high, double low, bool updateScene = false) noexcept;

        int getPriceMaxFloatingPrecisionPoint(double price);
        void setLowestLowAndPriceFPPrecision(double low) noexcept;

        void updateCandleRects();

        inline CandleChartScene *getCandleChartScene() noexcept { return m_scene; }

        CandleItem *getCandleItem(uint64_t timestamp) noexcept;

        inline int pricePrecision() const noexcept { return m_pricePrecision; }

    public slots:
        /// Show or hide the horizontal grid lines
        void showHorzGridLines(bool show) noexcept;
        /// Show or hide the vertical grid lines
        void showVertGridLines(bool show) noexcept;

    signals:
        void snEnterEvent(QEnterEvent *event);
        void snLeaveEvent(QEvent *event);

    protected:
        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;

    protected:
        void fixSize(int width, int height);
        void updateAxisRect(int windowWidth, int windowHeight) noexcept;
        void updateHorzGridLines() noexcept;
        void updateVertGridLines() noexcept;
        void updateAxisTimestamp(uint64_t timestamp) noexcept;

    protected:
        static QSizeF calculateDoubleToStringMetrics(double val, int precision) noexcept;
        static double scaleHorizontalPoint(double pt, double min, double max, double maxWidth) noexcept;
        static double scaleVerticalPoint(double pt, double min, double max, double maxHeight) noexcept;

    protected:
        CandleChartScene *m_scene;

    private:
        QMap<quint64, CandleItem *> m_candles;

    protected:
        PriceAxisItem *m_priceAxis;
        TimeAxisItem *m_timeAxis;
        QList<QGraphicsLineItem *> m_horzGridLines;
        QList<QGraphicsLineItem *> m_vertGridLines;
        QGraphicsItemGroup *m_horzGridLinesGroup;
        QGraphicsItemGroup *m_vertGridLinesGroup;

    protected:
        struct PriceMinMax
        {
            double min { 0 };
            double max { 0 };
            double lowestLow { 0 };
            bool lowestLowSet { false };
        } m_priceMinMax {};

        struct PriceAxisSizeParameters
        {
            double priceAxisHeight;
            double priceAxisWidth;
        } m_priceAxisSizeParameters {};

        bool m_showHorzGridLines;
        bool m_showVertGridLines;

        int m_pricePrecision;

        friend class CandleChartScene;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTINFOWIDGET_HPP
