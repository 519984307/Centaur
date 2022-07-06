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

#include "../../Centaur.hpp"
#include <QGraphicsItem>

namespace CENTAUR_NAMESPACE
{
    class CandleItem : public QGraphicsItem
    {
    public:
        CandleItem(double open, double close, double high, double low, QGraphicsItem *parent = nullptr);

    public:
        C_NODISCARD inline double open() const { return m_data.open; }
        C_NODISCARD inline double close() const { return m_data.close; }
        C_NODISCARD inline double high() const { return m_data.high; }
        C_NODISCARD inline double low() const { return m_data.low; }

        void setParameters(double open, double close, double high, double low) noexcept;

    public:
        inline bool isCandleBullish() const
        {
            return close() > open();
        }
        inline bool isCandleBearish() const
        {
            return !isCandleBullish();
        } /*

     signals:
         /// \brief Emits when the mouse enter the item
         /// \param item This item, this will be emitted as: emit snMouseHoverItemCandle(this);
         void snMouseHoverItemCandle(CandleItem *item);*/

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) noexcept override;

    protected:
        struct CandleData
        {
            double open;
            double close;
            double high;
            double low;
        } m_data;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLEITEM_HPP
