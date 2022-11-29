/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 17/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#include <QListView>

#ifndef CENTAUR_WATCHLISTWIDGET_HPP
#define CENTAUR_WATCHLISTWIDGET_HPP

#include "Centaur.hpp"
#include <QListWidgetItem>
#include <QWidget>

BEGIN_CENTAUR_NAMESPACE

class WatchlistWidget : public QListView
{
    Q_OBJECT
public:
    explicit WatchlistWidget(QWidget *parent = nullptr);
    ~WatchlistWidget() override;

public:
    void linkSearchEdit(QLineEdit *edit) noexcept;

public:
    void insertItem(const QPixmap &icon, const QString &symbol, const QString &source, qreal price, qreal diff, qint64 latency) noexcept;

    void updatePrice(const QString &symbol, const QString &source, qreal price) noexcept;
    void updateDifference(const QString &symbol, const QString &source, qreal diff) noexcept;
    void updateLatency(const QString &symbol, const QString &source, qint64 latency) noexcept;
    void updatePriceAndLatency(const QString &symbol, const QString &source, qreal price, qint64 latency) noexcept;

    void removeItem(const QString &symbol, const QString &source) noexcept;

    std::pair<QString, QString> sourceFromPoint(const QPoint &pt);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void itemHover(const QString &symbol, const QString &source);
    void mouseRightClick(const QPoint &pt);
    void mouseLeave();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_WATCHLISTWIDGET_HPP
