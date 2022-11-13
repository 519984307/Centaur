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

#include "Centaur.hpp"

#include "CandleChart/CandleItem.hpp"
#include "CandleChart/PriceAxisItem.hpp"
#include "CandleChart/TimeAxisItem.hpp"

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsView>

namespace CENTAUR_NAMESPACE
{
    class CandleChartScene;
    class CandleChartWidget : public QGraphicsView
    {
        Q_OBJECT
    public:
        explicit CandleChartWidget(QWidget *parent = nullptr);
        ~CandleChartWidget() override = default;

    public:
        /// \brief Set the chart timeframe
        void setChartTimeFrame(CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf);
        /// \brief Updating a non-existent candle will add the candle
        void updateCandle(quint64 timestamp, double open, double close, double high, double low) noexcept;
        /// \brief Adding an existing timestamp will call to updateCandle
        void addCandle(quint64 timestamp, double open, double close, double high, double low) noexcept;
        /// \brief Get an approximate of decimals based on a price. This is the first precision point that is made
        int getPriceMaxFloatingPrecisionPoint(double price) noexcept;
        /// \brief Set vertical min max
        /// This function will never change
        void setPriceMinMax(double min, double max, bool preventUpdate = false) noexcept;
        /// \brief Set price Min. Calls setPriceMinMax internally
        void setPriceMin(double min) noexcept;
        /// \brief Set price Min. Calls setPriceMinMax internally
        void setPriceMax(double max) noexcept;
        /// \brief Set timestamp Min. Calls setTimestampMinMax internally
        void setTimestampMin(uint64_t min) noexcept;
        /// \brief Set timestamp Max. Calls setTimestampMinMax internally
        void setTimestampMax(uint64_t max) noexcept;
        /// \brief Set the horizontal axis min or max
        void setTimestampMinMax(uint64_t min, uint64_t max, bool preventUpdate = false) noexcept;
        /// \brief Sets the precision point. Overrides the actions of getPriceMaxFloatingPrecisionPoint at the start. Set to -1 to go to the default
        void setPricePrecision(int precision) noexcept;
        /// \brief Set Candle dimensions
        void setCandleParameters(double width, double spacing) noexcept;
        /// \brief Set candle width
        void setCandleWidth(double width) noexcept;
        /// \brief Set Candle Spacing
        void setCandleSpacing(double spacing) noexcept;

    public:
        void hoverItem(uint64_t timestamp) noexcept;

    public:
        /// \brief Get a candle based on timestamp
        C_NODISCARD CandleItem *getCandleItem(uint64_t timestamp) noexcept;
        C_NODISCARD uint64_t timeFrameMilliseconds() noexcept;
        C_NODISCARD inline auto getPricePrecision() const noexcept { return m_pricePrecision; }
        C_NODISCARD inline auto getCandleWidth() const noexcept { return m_candleDimensions.width; }
        C_NODISCARD inline auto getCandleSpacing() const noexcept { return m_candleDimensions.spacing; }
        C_NODISCARD inline auto getMinPrice() const noexcept { return m_chartParameters.priceMin; }
        C_NODISCARD inline auto getMaxPrice() const noexcept { return m_chartParameters.priceMax; }
        C_NODISCARD inline auto getMinTimestamp() const noexcept { return m_chartParameters.minTimestamp; }
        C_NODISCARD inline auto getMaxTimestamp() const noexcept { return m_chartParameters.maxTimestamp; }

    public slots:
        /// \brief Show or hide the horizontal grid lines
        void showHorzGridLines(bool show) noexcept;
        /// \brief Show or hide the vertical grid lines
        void showVertGridLines(bool show) noexcept;

    signals:
        void snUpdateCandleMousePosition(quint64 timestamp);

    protected:
        void updateItemRectangle(CandleItem *item) noexcept;
        void updateAllItemRectangles() noexcept;
        void updateCrossHair(const QPointF &pt) noexcept;
        void updateVCrossHair(uint64_t timestamp) noexcept;

    protected:
        C_NODISCARD static QSizeF calculateDoubleToStringMetrics(double val, int precision) noexcept;

        // QScrollArea reimplementation
    protected:
        void resizeEvent(QResizeEvent *event) override;
        // QGraphicsView reimplementations
    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void keyReleaseEvent(QKeyEvent *event) override;
        void mouseDoubleClickEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void wheelEvent(QWheelEvent *event) override;

    protected:
        void scrollAreaHorizontal(double newX) noexcept;
        void scrollAreaVertical(double newY) noexcept;

    protected:
        QGraphicsScene *m_scene;

    private:
        QHash<quint64, CandleItem *> m_candles;

    protected:
        double m_scalingFactor;

        int m_pricePrecision;

        CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame m_tf;

        bool m_showHorzGridLines;
        bool m_showVertGridLines;
        bool m_precisionOverride;

        struct CandleChartParameters
        {
            double priceMin { 0 };
            double priceMax { 0 };
            uint64_t minTimestamp { 0 };
            uint64_t maxTimestamp { 0 };
            bool minTimestampSet { false };
            bool priceMinSet { false };
        } m_chartParameters;

        struct CandleDimensions
        {
            double width { 15 };
            double spacing { 8 };
        } m_candleDimensions;

        struct OriginParameters
        {
            uint64_t timeDiff { 0 };
            uint64_t timestamp { 0 };
            double minPrice { 0 };
            double maxPrice { 0 };
            double X { 0 };
            double Y { 0 };
        } m_originParameters;

        struct ScrollPosition
        {
            double prevXPoint { 0 };
            double prevYPoint { 0 };
        } m_scrollPosition;

        struct CrossHairItems
        {
            QGraphicsLineItem *vertical { nullptr };
            QGraphicsLineItem *horizontal { nullptr };
        } m_crossHairItems;

        inline QGraphicsLineItem *getVCrossHair() noexcept { return m_crossHairItems.vertical; }
        inline QGraphicsLineItem *getHCrossHair() noexcept { return m_crossHairItems.horizontal; }
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTINFOWIDGET_HPP
