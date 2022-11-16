/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_DEPTHCHARTDIALOG_HPP
#define CENTAUR_DEPTHCHARTDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class DepthChartDialog;
}

class DepthChartDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DepthChartDialog(const QString &symbol, QWidget *parent = nullptr);
    ~DepthChartDialog() override;

public:
    void setPrice(double price);

protected slots:
    void onCloseButton() noexcept;
public slots:
    void onOrderBookDepth(const QMap<qreal, QPair<qreal, qreal>> &asks, const QMap<qreal, QPair<qreal, qreal>> &bids) noexcept;

protected:
    void restoreInterface() noexcept;

signals:
    void closeButtonPressed();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::DepthChartDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_DEPTHCHARTDIALOG_HPP
