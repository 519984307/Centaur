/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 12/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_SPLASHDIALOG_HPP
#define CENTAUR_SPLASHDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class SplashDialog;
}
class SplashDialog : public QDialog
{
public:
    explicit SplashDialog(QWidget *parent = nullptr);
    ~SplashDialog() override;

public:
    void setDisplayText(const QString &text);
    void setProgressRange(int min, int max);
    void setProgressPos(int pos);
    void step();
    std::pair<int, int> getProgressRange();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::SplashDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_SPLASHDIALOG_HPP
