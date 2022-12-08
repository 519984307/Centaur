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

    class CandleTimeAxisItem : public QGraphicsRectItem
    {
    public:
        /// Labels will be set based on the timeframe
        CandleTimeAxisItem(QGraphicsItem *parent = nullptr);
        ~CandleTimeAxisItem() override;

    public:
        void setTimeFrame(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf) noexcept;
        void setCandleWidth(qreal width) noexcept;
        void setCandleSpacing(qreal spacing) noexcept;
        void setCandleParameters(double width, double spacing) noexcept;
        void setTimestamp(int64_t timestamp) noexcept;
        void setAxisFont(const QFont &font) noexcept;
        void updateTracker(qreal x) noexcept;

    public:
        void hideTracker() noexcept;
        void showTracker() noexcept;

    public:
        C_NODISCARD auto getTimeframe() const noexcept -> CENTAUR_PLUGIN_NAMESPACE::TimeFrame;
        C_NODISCARD auto getCandleWidth() const noexcept -> qreal;
        C_NODISCARD auto getCandleSpacing() const noexcept -> qreal;
        C_NODISCARD auto getGridLinePositions() const noexcept -> QList<qreal>;

    public:
        C_NODISCARD auto timestampFromPoint(qreal point) const noexcept -> int64_t;
        C_NODISCARD auto middle(qreal point) const noexcept -> qreal;
        C_NODISCARD auto pointFromTimestamp(int64_t timestamp) const noexcept -> qreal;

    public:
        C_NODISCARD auto isTrackerVisible() const noexcept -> bool;

    protected:
        C_NODISCARD auto indexFromPoint(qreal point) const noexcept -> std::int64_t;

    public:
        void scaleTime(qreal factor) noexcept;

    public:
        void updatePriceAxisWidth(qreal width) noexcept;
        void calculateRectangles();

    protected:
        static int64_t timeFrameToMilliseconds(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf) noexcept;
        static qreal longestPossibleTagSize(const QFont &font) noexcept;

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    protected:
        C_NODISCARD std::pair<QString, bool> labelFromTimestamp(int64_t timestamp) noexcept;
        C_NODISCARD bool isTimestampAnchor(int64_t timestamp) noexcept;

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_TIMEAXISITEM_HPP
