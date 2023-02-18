/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_SHORTCUTDIALOG_HPP
#define CENTAUR_SHORTCUTDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class ShortcutDialog;
}
class ShortcutDialog : public QDialog
{
public:
    explicit ShortcutDialog(QWidget *parent = nullptr);
    ~ShortcutDialog() override;

public:
    void setShortcutInformation(const QString &text, const QString &shortcut) noexcept;
    QKeySequence getShortcut() noexcept;

protected slots:
    void onAccept() noexcept;

protected:
    void restoreInterface() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::ShortcutDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_SHORTCUTDIALOG_HPP
