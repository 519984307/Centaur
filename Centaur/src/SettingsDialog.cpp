////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "SettingsDialog.hpp"
#include "../ui/ui_SettingsDialog.h"
#include "CentaurApp.hpp"
#include "Logger.hpp"

#include <QCheckBox>

#include <QDir>
#include <QMessageBox>
#include <QPainter>
#include <QStandardPaths>

#include <QSettings>

#include <QUuid>

BEGIN_CENTAUR_NAMESPACE

SettingsDialog::Impl::Impl() :
    ui { new Ui::SettingsDialog }
{
}

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog { parent },
    _impl { new Impl },
    _sesImpl { new SessionImpl }
{
    ui()->setupUi(this);
    connect(ui()->acceptButton, &QPushButton::released, this, &SettingsDialog::onAccept);
    connect(ui()->treeWidget, &QTreeWidget::itemPressed, this, &SettingsDialog::treeItemPressed);

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir directory(path);
    if (!directory.exists())
    {
        logInfo("settings", "Application path does not exist.");
        if (directory.mkpath(path))
            logInfo("settings", "Application path was created");
        else
            logError("settings", "Application path was not created");
    }

    ui()->titleFrame->overrideParent(this);
    ui()->mainSettingsFrame->overrideParent(this);

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    ui()->closeButton->setButtonClass(SystemPushButton::ButtonClass::override);
    connect(ui()->closeButton, &SystemPushButton::systemPressed, this, [&]() {
        reject();
    });

    ui()->warnLabel->setVisible(false);
    ui()->warnIconLabel->setVisible(false);

    initializeTree();
    initSessionWidget();
    initAdvancedWidget();
    initPluginsWidget();
    initShortcutsWidget();

    restoreInterface();

    ui()->treeWidget->setCurrentItem(_impl->sessionItem);
    setPage(Pages::Session);
}

SettingsDialog::~SettingsDialog() = default;

Ui::SettingsDialog *SettingsDialog::ui()
{
    return _impl->ui.get();
}

bool SettingsDialog::isFirstTimeStarted() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("__Session__");
    const auto value = settings.value("__time__").toBool();
    settings.endGroup();
    return !value;
}

void SettingsDialog::onAccept() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("SettingsDialog");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter", ui()->splitter->saveState());
    settings.endGroup();

    settings.beginGroup("infoUserTable");
    settings.setValue("geometry", ui()->userTable->saveGeometry());
    settings.setValue("h-geometry", ui()->userTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->userTable->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("memTableWidget");
    settings.setValue("geometry", ui()->memTableView->saveGeometry());
    settings.setValue("h-geometry", ui()->memTableView->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->memTableView->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("pluginsTableWidget");
    settings.setValue("geometry", ui()->pluginsTableWidget->saveGeometry());
    settings.setValue("h-geometry", ui()->pluginsTableWidget->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->pluginsTableWidget->horizontalHeader()->saveState());
    settings.endGroup();

    accept();
}

void SettingsDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("SettingsDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    ui()->splitter->restoreState(settings.value("splitter").toByteArray());
    settings.endGroup();

    settings.beginGroup("infoUserTable");
    ui()->userTable->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->userTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->userTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("memTableWidget");
    ui()->memTableView->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->memTableView->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->memTableView->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("pluginsTableWidget");
    ui()->pluginsTableWidget->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->pluginsTableWidget->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->pluginsTableWidget->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void SettingsDialog::initializeTree() noexcept
{
    ui()->treeWidget->setIndentation(20);
    ui()->treeWidget->setColumnCount(1);

    _impl->sessionItem   = new QTreeWidgetItem({ tr("Session") });
    _impl->advancedItem  = new QTreeWidgetItem({ tr("Advanced") });
    _impl->pluginsItem   = new QTreeWidgetItem({ tr("Plugins") });
    _impl->shortcutsItem = new QTreeWidgetItem({ tr("Shortcuts") });

    ui()->treeWidget->insertTopLevelItems(0, { _impl->sessionItem, _impl->shortcutsItem, _impl->advancedItem, _impl->pluginsItem });
    ui()->treeWidget->expandItem(_impl->sessionItem);
    ui()->treeWidget->expandItem(_impl->advancedItem);
    ui()->treeWidget->expandItem(_impl->pluginsItem);
    ui()->treeWidget->expandItem(_impl->shortcutsItem);
}

void SettingsDialog::treeItemPressed(QTreeWidgetItem *item, C_UNUSED int column) noexcept
{
    if (item == _impl->sessionItem)
    {
        setPage(Pages::Session);
    }
    else if (item == _impl->advancedItem)
    {
        setPage(Pages::Advanced);
    }
    else if (item == _impl->pluginsItem)
    {
        setPage(Pages::Plugins);
    }
    else if (item == _impl->shortcutsItem)
    {
        setPage(Pages::Shortcuts);
    }
}

void SettingsDialog::setPage(SettingsDialog::Pages page) noexcept
{
    switch (page)
    {
        case Pages::Session: ui()->stackedWidget->setCurrentWidget(ui()->sessionPage); break;
        case Pages::Advanced: ui()->stackedWidget->setCurrentWidget(ui()->advancedPage); break;
        case Pages::Plugins: ui()->stackedWidget->setCurrentWidget(ui()->pluginsPage); break;
        case Pages::Shortcuts: ui()->stackedWidget->setCurrentWidget(ui()->shortcutsPage); break;
    }
}

END_CENTAUR_NAMESPACE
