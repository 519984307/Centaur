/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 26/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#ifndef CENTAUR_PLUGINSDIALOG_HPP
#define CENTAUR_PLUGINSDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class PluginsDialog;
}
class PluginsDialog : public QDialog
{
public:
    explicit PluginsDialog(QWidget *parent = nullptr);
    ~PluginsDialog() override;

protected slots:
    void onAccept() noexcept;

protected:
    void restoreInterface() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::PluginsDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_PLUGINSDIALOG_HPP
