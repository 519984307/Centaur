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

#include "Centaur.hpp"

#include "CandleItem.hpp"
#include "CandlePriceAxisItem.hpp"
#include "CandleTimeAxisItem.hpp"

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsView>

BEGIN_CENTAUR_NAMESPACE

class CandleChartScene;
class CandleChartWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CandleChartWidget(QWidget *parent = nullptr);
    ~CandleChartWidget() override;

public:
    /// \brief Set the chart timeframe
    void setChartTimeFrame(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf);
    /// \brief Updating a non-existent candle will add the candle
    void updateCandle(quint64 timestamp, double open, double close, double high, double low) noexcept;
    /// \brief Adding an existing timestamp will call to updateCandle
    void addCandle(quint64 timestamp, double open, double close, double high, double low) noexcept;

public:
    /// \brief Set the price axis to the left or the right
    /// \param orientation only Qt::AlignmentFlag::AlignRight or Qt::AlignmentFlag::AlignLeft are valid
    /// any other value will set the default, which is Qt::AlignmentFlag::AlignRight
    void setPriceAxisOrientation(Qt::AlignmentFlag orientation);

public:
    void setHorizontalLinePen(const QPen &pen) noexcept;
    void setVerticalLinePen(const QPen &pen) noexcept;

public:
    C_NODISCARD bool isHorizontalLineVisible() const noexcept;
    C_NODISCARD bool isVerticalLineVisible() const noexcept;
    C_NODISCARD bool isPriceTrackerVisible() const noexcept;


public slots:
    void onShowHorizontalLine(bool show) noexcept;
    void onShowVerticalLine(bool show) noexcept;

    // Vertical axis slots
public slots:
    void onShowAxisPriceTracker(bool show) noexcept;

    // Horizontal axis slots
public slots:
    void onChangeCandleWidth(qreal width) noexcept;
    void onChangeCandleSpacing(qreal spacing) noexcept;


signals:
    void snUpdateCandleMousePosition(quint64 timestamp);


protected:
    void updateCrossHair(const QPointF &pt) noexcept;
    /// \brief Uses the previous point to calculate the scroll
    void mouseScrollAreaHorizontal(qreal newX) noexcept;
    /// \brief Uses the previous point to calculate the scroll
    void mouseScrollAreaVertical(qreal newY) noexcept;

    // QGraphicsView reimplementation
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;


private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTINFOWIDGET_HPP
