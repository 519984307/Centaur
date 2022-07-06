/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef CENTAUR_CANDLECHARTSCENE_HPP
#define CENTAUR_CANDLECHARTSCENE_HPP

#include "../../Centaur.hpp"
#include "CandleChart/PriceAxisItem.hpp"
#include "CandleChart/TimeAxisItem.hpp"
#include <QEnterEvent>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>

namespace CENTAUR_NAMESPACE
{
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

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

        // Mouse tracker
    protected:
        QGraphicsLineItem *m_mouseTrackVert;
        QGraphicsLineItem *m_mouseTrackHorz;
        QGraphicsItemGroup *m_mouseTrack;

        // Price axis indicator
    protected:
        PriceAxisItem *m_priceAxis;
        TimeAxisItem *m_timeAxis;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTSCENE_HPP
