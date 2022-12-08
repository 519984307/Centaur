/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 02/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CANDLEITEM_HPP
#define CENTAUR_CANDLEITEM_HPP

#include "Centaur.hpp"
#include <QGraphicsItem>

namespace CENTAUR_NAMESPACE
{
    enum ChartType : int
    {
        Candle
    };
    class CandleChartScene;
    class CandleItem : public QGraphicsRectItem
    {
    public:
        CandleItem(int64_t timestamp, qreal open, qreal close, qreal high, qreal low, QGraphicsItem *parent = nullptr);

    public:
        C_NODISCARD auto timestamp() const noexcept -> int64_t;
        C_NODISCARD auto open() const noexcept -> qreal;
        C_NODISCARD auto close() const noexcept -> qreal;
        C_NODISCARD auto high() const noexcept -> qreal;
        C_NODISCARD auto low() const noexcept -> qreal;

    public:
        void setCandleTimestamp(int64_t timestamp) noexcept;
        void setParameters(qreal open, qreal close, qreal high, qreal low) noexcept;

    public:
        C_NODISCARD auto isCandleBullish() const -> bool;
        C_NODISCARD auto isCandleBearish() const -> bool;

    public:
        int type() const override;

    public:
        void updateCandleRect() noexcept;

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLEITEM_HPP
