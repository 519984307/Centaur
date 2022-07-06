/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "CandleChart/CandleItem.hpp"

CENTAUR_NAMESPACE::CandleItem::CandleItem(double open, double close, double high, double low, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_data { open, close, high, low }

{
}

void CENTAUR_NAMESPACE::CandleItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) noexcept
{
    // emit snMouseHoverItemCandle(this);
}

void CENTAUR_NAMESPACE::CandleItem::setParameters(double open, double close, double high, double low) noexcept
{
    m_data = { open, close, high, low };
    
    update();
}
