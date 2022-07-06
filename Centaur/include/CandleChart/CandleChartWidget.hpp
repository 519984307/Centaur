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

        void updatePriceAxisMetrics(double open, double close, double high, double low, bool updateScene = false) noexcept;

        /// Show or hide the horizontal grid lines
        void showHorzGridLines(bool show) noexcept;

        /// Show or hide the vertical grid lines
        void showVertGridLines(bool show) noexcept;

    signals:
        void snEnterEvent(QEnterEvent *event);
        void snLeaveEvent(QEvent *event);
        void snUpdateCandleMousePosition(quint64 timestamp, double open, double close, double high, double low, double change, double amplitude);

    protected:
        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;

    protected:
        void fixSize(int width, int height);
        void updatePriceAxisRect(int windowWidth, int windowHeight) noexcept;
        void updateHorzGridLines() noexcept;
        void updateVertGridLines() noexcept;

    protected:
        static QSizeF calculateDoubleToStringMetrics(double val) noexcept;
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
            double min;
            double max;
        } m_priceMinMax {};

        struct PriceAxisSizeParameters
        {
            double priceAxisHeight;
            double priceAxisWidth;
        } m_priceAxisSizeParameters {};

        bool m_showHorzGridLines;
        bool m_showVertGridLines;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTINFOWIDGET_HPP
