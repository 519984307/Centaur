/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 24/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CANDLECHARTDIALOG_HPP
#define CENTAUR_CANDLECHARTDIALOG_HPP

#include "Centaur.hpp"
#include <CentaurPlugin.hpp>
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class CandleChartDialog;
}
class CandleChartDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CandleChartDialog(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf, const QString &symbol, const QString &source, QWidget *parent = nullptr);
    ~CandleChartDialog() override;

protected slots:
    void onCloseButton() noexcept;

protected:
    void restoreInterface() noexcept;

signals:
    void closeButtonPressed();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::CandleChartDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_CANDLECHARTDIALOG_HPP
