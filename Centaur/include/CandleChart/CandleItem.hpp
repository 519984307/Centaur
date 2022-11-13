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
    class CandleChartScene;
    class CandleItem : public QGraphicsRectItem
    {
        enum
        {
            Type = UserType + 1
        };

    public:
        CandleItem(uint64_t timestamp, double open, double close, double high, double low, QGraphicsItem *parent = nullptr);

    public:
        C_NODISCARD inline auto timestamp() const noexcept { return m_timestamp; }
        C_NODISCARD inline auto open() const noexcept { return m_data.open; }
        C_NODISCARD inline auto close() const noexcept { return m_data.close; }
        C_NODISCARD inline auto high() const noexcept { return m_data.high; }
        C_NODISCARD inline auto low() const noexcept { return m_data.low; }

    public:
        void setParameters(double open, double close, double high, double low) noexcept;

    public:
        C_NODISCARD inline bool isCandleBullish() const
        {
            return close() > open();
        }
        C_NODISCARD inline bool isCandleBearish() const
        {
            return !isCandleBullish();
        }

    public:
        C_NODISCARD inline int type() const override { return Type; }

    public:
        void updateCandleRect(double openPoint, double closePoint, double highPoint, double lowPoint) noexcept;

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    protected:
        uint64_t m_timestamp;
        struct CandleData
        {
            double open;
            double close;
            double high;
            double low;
        } m_data;
        QLineF m_high;
        QLineF m_low;
        QRectF m_barRect;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLEITEM_HPP
