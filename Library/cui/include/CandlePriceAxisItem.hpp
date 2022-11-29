/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PRICE_AXIS_ITEM_HPP
#define CENTAUR_PRICE_AXIS_ITEM_HPP

#include "Centaur.hpp"
#include <QGraphicsRectItem>

BEGIN_CENTAUR_NAMESPACE

class CandlePriceAxisItem : public QGraphicsRectItem
{

public:
    explicit CandlePriceAxisItem(QGraphicsItem *parent = nullptr);
    ~CandlePriceAxisItem() override = default;

public:
    void setOrientation(Qt::AlignmentFlag orientation) noexcept;
    void centerAt(qreal price) noexcept;
    qreal center() noexcept;
    void scalePrice(qreal factor) noexcept;
    void setPriceStep(qreal log, qreal dec) noexcept;
    void updateTracker(qreal y) noexcept;

public:
    void setBorderPen(const QPen &pen);
    void showTracker() noexcept;
    void hideTracker() noexcept;

public:
    C_NODISCARD Qt::AlignmentFlag orientation() const noexcept;
    C_NODISCARD qreal priceToAxisPoint(qreal price) const noexcept;
    C_NODISCARD qreal axisPointToPrice(qreal point) const noexcept;
    C_NODISCARD bool isTrackerVisible() const noexcept;
    QList<double> getGridLinePositions() noexcept;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public:
    void calculateRectangles() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_PRICE_AXIS_ITEM_HPP
