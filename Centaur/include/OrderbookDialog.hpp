/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_ORDERBOOKDIALOG_HPP
#define CENTAUR_ORDERBOOKDIALOG_HPP

#include "Centaur.hpp"
#include "CentaurPlugin.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class OrderbookDialog;
}
class DepthChartDialog;
class OrderbookDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OrderbookDialog(QString symbol, CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange, QWidget *parent = nullptr);
    ~OrderbookDialog() override;

public:
    void setPrice(double price);

protected slots:
    void onCloseButton() noexcept;
    void onOrderbookUpdate(const QString &source, const QString &symbol, quint64 receivedTime, const QMap<qreal, QPair<qreal, qreal>> &bids, const QMap<qreal, QPair<qreal, qreal>> &asks) noexcept;

protected:
    void restoreInterface() noexcept;

signals:
    void closeButtonPressed();
    void redirectOrderbook(const QMap<qreal, QPair<qreal, qreal>> &asks, const QMap<qreal, QPair<qreal, qreal>> &bids);

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::OrderbookDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_ORDERBOOKDIALOG_HPP
