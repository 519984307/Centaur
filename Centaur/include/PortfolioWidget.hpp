/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PORTFOLIOWIDGET_HPP
#define CENTAUR_PORTFOLIOWIDGET_HPP

#include "Centaur.hpp"
#include <QListView>

BEGIN_CENTAUR_NAMESPACE

class PortfolioWidget : public QListView
{
    Q_OBJECT
public:
    explicit PortfolioWidget(QWidget *parent = nullptr);
    ~PortfolioWidget() override;

public:
    void linkSearchEdit(QLineEdit *edit) noexcept;

public:
    void insertItem(const QPixmap &icon, const QString &symbol, const QString &source, const QString &shortName, const QString &link, const QString &balance) noexcept;

    void updateBalance(const QString &symbol, const QString &source, const QString &balance) noexcept;

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

#endif // CENTAUR_PORTFOLIOWIDGET_HPP
