/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 11/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_SQUARIFYWIDGET_HPP
#define CENTAUR_SQUARIFYWIDGET_HPP

#include "Centaur.hpp"
#include <QGraphicsView>

BEGIN_CENTAUR_NAMESPACE

class SquarifyWidget : public QGraphicsView
{
public:
    explicit SquarifyWidget(QWidget *parent = nullptr);
    ~SquarifyWidget() override;

public:
    /// \brief Add not normalized data
    void setData(const QList<std::pair<qreal, QString>> &data, const QBrush &negative, const QBrush &positive);

protected:
    void resizeEvent(QResizeEvent *event) override;

protected slots:
    void recalculate(const QRectF &rect) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_SQUARIFYWIDGET_HPP
