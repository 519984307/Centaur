/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef CENTAUR_CANDLECHARTSCENE_HPP
#define CENTAUR_CANDLECHARTSCENE_HPP

#include "Centaur.hpp"
#include "CandleChart/PriceAxisItem.hpp"
#include "CandleChart/TimeAxisItem.hpp"
#include <QElapsedTimer>
#include <QEnterEvent>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>

namespace CENTAUR_NAMESPACE
{
    class CandleChartWidget;
    class CandleChartScene : public QGraphicsScene
    {
        Q_OBJECT
    public:
        explicit CandleChartScene(QObject *parent = nullptr);
        ~CandleChartScene() override = default;

    public:
        /// This will redirect mouse move events to the axis (only Y movements)
        void setPriceAxisItem(PriceAxisItem *item) noexcept;
        void setTimeAxisItem(TimeAxisItem *item) noexcept;

    public slots:
        void onMouseEnter(QEnterEvent *event) noexcept;
        void onMouseLeave(QEvent *event) noexcept;

    public:
        inline PriceAxisItem *getPriceAxis() noexcept { return m_priceAxis; }
        inline TimeAxisItem *getTimeAxis() noexcept { return m_timeAxis; }

    signals:
        void snUpdateCandleMousePosition(quint64 timestamp);

    protected:
        /// \brief This handles the crosshair movement as well of the axis scrolling
        /// \remarks Note that the Vertical Axis is move on a speed basis and the horizontal axis is not
        void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
        void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

        // Mouse tracker
    protected:
        QGraphicsLineItem *m_mouseTrackVert;
        QGraphicsLineItem *m_mouseTrackHorz;
        QGraphicsItemGroup *m_mouseTrack;

        // Price axis indicator
    protected:
        CandleChartWidget *m_chartView;
        PriceAxisItem *m_priceAxis;
        TimeAxisItem *m_timeAxis;
        // QPointF m_capturePoint;
        uint64_t m_timeAxisMin;
        uint64_t m_timeAxisMax;
        uint64_t m_timeSteps;
        double m_priceAxisMin;
        double m_priceAxisMax;
        double m_priceSteps;
        bool m_priceAxisCapture;
        bool m_timeAxisCapture;
        QElapsedTimer m_mouseMoveTime;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTSCENE_HPP
