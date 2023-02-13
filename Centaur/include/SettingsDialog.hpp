////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_SETTINGSDIALOG_HPP
#define CENTAUR_SETTINGSDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>

BEGIN_CENTAUR_NAMESPACE
namespace Ui
{
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    enum class StatusMode
    {
        error,
        ok,
        warning
    };

public:
    enum class Pages
    {
        Session,
        Advanced,
        Plugins
    };

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

public:
    static bool isFirstTimeStarted() noexcept;

public:
    void setPage(Pages page) noexcept;

protected:
    void initializeTree() noexcept;
    void initSessionWidget() noexcept;
    void initAdvancedWidget() noexcept;
    void initPluginsWidget() noexcept;
    void initShortcutsWidget() noexcept;

protected:
    void showWarningLabels() noexcept;
    void hideWarningLabels() noexcept;

protected slots:
    void treeItemPressed(QTreeWidgetItem *item, int column) noexcept;
    void onAddUser() noexcept;
    void onAccept() noexcept;
    void onAllow2FA(bool checked) noexcept;
    void pluginsTableItemChanged(QTableWidgetItem *item) noexcept;
    void installPlugin() noexcept;
    void uninstallPlugin() noexcept;

protected:
    void restoreInterface() noexcept;
    void resetTablesValues() noexcept;

private:
    void openUserInformationData() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::SettingsDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_SETTINGSDIALOG_HPP
