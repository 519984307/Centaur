/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_LOGDIALOG_HPP
#define CENTAUR_LOGDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>
#include <QTableWidget>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class LogDialog;
}
class LogDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LogDialog(QWidget *parent = nullptr);
    ~LogDialog() override;

protected slots:
    void onLog(unsigned long long date, int session, int level, const QString &usr, const QString &source, const QString &msg) noexcept;

public:
    QTableWidget *tableWidget();

protected:
    void restoreInterface() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::LogDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_LOGDIALOG_HPP
