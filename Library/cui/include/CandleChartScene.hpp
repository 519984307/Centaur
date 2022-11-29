/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/07/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef CENTAUR_CANDLECHARTSCENE_HPP
#define CENTAUR_CANDLECHARTSCENE_HPP

#include "CandleTimeAxisItem.hpp"
#include "Centaur.hpp"
#include <QElapsedTimer>
#include <QEnterEvent>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>

BEGIN_CENTAUR_NAMESPACE

class CandleChartWidget;
class CandlePriceAxisItem;
class CandleChartScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CandleChartScene(QObject *parent = nullptr);
    ~CandleChartScene() override;

public:
    CandlePriceAxisItem *getPriceAxis() noexcept;
    CandleTimeAxisItem *getTimeAxis() noexcept;

public:
    /// \brief Determinate if pt is within an axis
    /// \param pt Point to measure
    /// \return Qt::Orientation::Vertical for Y axis (whether its right or left aligned);  Qt::Orientation::Horizontal for the X axis or 0 if pt is not inside the axis
    C_NODISCARD int isPointInAxis(const QPointF &pt) const noexcept;

public:
    void setPriceAxisOrientation(Qt::AlignmentFlag orientation);
    /// \brief Determinate the current max price
    /// \param max determinate if max is the high price so far
    void calculatePriceMax(qreal max);
    /// \brief Centers the price axis at this position
    /// \param price Center of the axis
    void centerPriceAxisAt(qreal price);
    /// \brief Gets the center position
    qreal priceAxisCenterPosition() const noexcept;
    /// \brief Scale the price axis
    /// \param factor factor > 0 to zoom in; factor < 0 to zoom out
    void scalePriceAxis(qreal factor) noexcept;
    // Price to point
    qreal priceToPoint(qreal price) noexcept;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

public slots:
    void onViewRectChange(QSizeF size);
    void onMouseEnter(QEnterEvent *event) noexcept;
    void onMouseLeave(QEvent *event) noexcept;

signals:
    void snUpdateCandleMousePosition(quint64 timestamp);

protected:
    void updatePriceAxisRect() noexcept;
    void updateTimeAxisRect() noexcept;

protected:
    static int getPriceMaxFloatingPrecisionPoint(double price) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTSCENE_HPP
