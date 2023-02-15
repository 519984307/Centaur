/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "../ui/ui_SettingsDialog.h"
#include "Globals.hpp"
#include "SettingsDialog.hpp"

#include "Logger.hpp"
#include <QAction>
#include <QDir>
#include <QDirIterator>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStandardPaths>

#include <rapidjson/prettywriter.h>
#include <rapidjson/schema.h>

BEGIN_CENTAUR_NAMESPACE

/**
 * \brief In order to have translations on the Shortcuts
 * all the json files are indexes
 */

void SettingsDialog::initShortcutsWidget() noexcept
{
    namespace json      = rapidjson;
    _sctImpl->itemModel = new QStandardItemModel(0, 2, ui()->shortcutsPage);
    auto proxyModel     = new QSortFilterProxyModel(this);

    proxyModel->setFilterKeyColumn(0);
    proxyModel->setSourceModel(_sctImpl->itemModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui()->shortcutsTree->setModel(proxyModel);

    connect(ui()->shortcutSearch, &QLineEdit::textChanged, proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    _sctImpl->itemModel->setHorizontalHeaderLabels({ tr("Editor Action"), tr("Shortcut") });



    // Load the schema file
    const QString schemaJSONKeymap = g_globals->paths.resPath + "/Schema/keymap.schema.json";
    QFile file(schemaJSONKeymap);
    if (!file.open(QIODevice::ReadOnly))
    {
        const QString message = tr("An internal file is missing for the keyboard shortcuts. Reinstalling the application might solve the problem");

        logError("initShortcutsWidget", message);

        QMessageBox::critical(this,
            tr("Missing file"),
            message,
            QMessageBox::Ok);
        return;
    }

    QTextStream textStream(&file);
    json::Document schemaJSONDoc;
    schemaJSONDoc.Parse(textStream.readAll().toUtf8().constData());
    if (schemaJSONDoc.HasParseError())
    {
        const QString message = tr("An internal file is corrupted for the keyboard shortcuts. Reinstalling the application might solve the problem");

        logError("initShortcutsWidget", message);

        QMessageBox::critical(this,
            tr("Corrupted file"),
            message,
            QMessageBox::Ok);

        return;
    }

    json::SchemaDocument schemaDoc(schemaJSONDoc);
    json::SchemaValidator schemaValidator(schemaDoc);

    // Read the json files
    QStringList searchPaths {
        g_globals->paths.resPath + "/Keymap/",                                              // internal files
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/keymap/" // user defined
    };

    for (const auto &path : searchPaths)
    {
        QDirIterator it(path, QDirIterator::NoIteratorFlags);
        while (it.hasNext())
        {
            QFileInfo nfo(it.next());
            if (nfo.completeSuffix() != "keymap.json")
            {
                logWarn("initShortcutsWidget", tr("An extraneous file (%1) in a keymap path (%2) was found").arg(nfo.fileName()).arg(path));
                continue;
            }

            QFile jsonKeymap(nfo.absoluteFilePath());
            if (!jsonKeymap.open(QIODevice::ReadOnly))
            {
                logWarn("initShortcutsWidget", tr("File %1 in the keymap path (%2) could not be opened").arg(nfo.fileName()).arg(path));
                continue;
            }

            QTextStream jsonFileData(&jsonKeymap);

            json::Document jsonDoc;
            jsonDoc.Parse(jsonFileData.readAll().toUtf8().constData());

            if (jsonDoc.HasParseError())
            {
                logWarn("initShortcutsWidget", tr("File %1 in the keymap path (%2) is not a valid JSON file").arg(nfo.fileName()).arg(path));
                continue;
            }

            if (!jsonDoc.Accept(schemaValidator))
            {
                logWarn("initShortcutsWidget", tr("File %1 in the keymap path (%2) is not a valid keymap json file").arg(nfo.fileName()).arg(path));
                continue;
            }

            const QString keymapName { jsonDoc["name"].GetString() };
            const QString keymapBase { jsonDoc["base"].GetString() };

            int idx = ui()->shortcutsComboBox->findText(keymapName);
            if (idx >= 0)
            {
                logWarn("initShortcutsWidget", tr("File %1 in the keymap path (%2) implements an already implemented keymap by name").arg(nfo.fileName()).arg(path));
                continue;
            }

            // Currently for easy of saving the document
            const int currentItem = ui()->shortcutsComboBox->count();

            ui()->shortcutsComboBox->insertItem(currentItem, keymapName);
            ui()->shortcutsComboBox->setItemData(currentItem, keymapBase, Qt::UserRole + 1);
            ui()->shortcutsComboBox->setItemData(currentItem, nfo.absoluteFilePath(), Qt::UserRole + 2);

            _sctImpl->docKeymap.emplace(keymapName, nfo.absoluteFilePath());
            _sctImpl->docKeymap.at(keymapName)().Swap(jsonDoc);
        }

        ui()->shortcutsComboBox->insertSeparator(ui()->shortcutsComboBox->count());
    }
    // Remove the last separator
    ui()->shortcutsComboBox->removeItem(ui()->shortcutsComboBox->count() - 1);

    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("Settings.Keymap");
    auto savedKeymap = settings.value("keymap", ".").toString();
    settings.endGroup();



    connect(ui()->shortcutsComboBox, &QComboBox::currentIndexChanged, this, &SettingsDialog::shortcutSelectionChanged);

    if (savedKeymap != ".")
    {
        int savedIndex = ui()->shortcutsComboBox->findText(savedKeymap);
        if (savedIndex != -1)
            ui()->shortcutsComboBox->setCurrentIndex(savedIndex);
    }

    // Init the combobox

    _sctImpl->duplicate = new QAction(tr("Duplicate..."), this);
    _sctImpl->rename    = new QAction(tr("Rename..."), this);
    _sctImpl->restore   = new QAction(tr("Restore defaults"), this);
    _sctImpl->del       = new QAction(tr("Delete..."), this);

    connect(_sctImpl->duplicate, &QAction::triggered, this, &SettingsDialog::duplicateKeymap);
    connect(_sctImpl->rename, &QAction::triggered, this, &SettingsDialog::renameKeymap);
    connect(_sctImpl->restore, &QAction::triggered, this, &SettingsDialog::restoreDefaultKeymap);
    connect(_sctImpl->del, &QAction::triggered, this, &SettingsDialog::removeKeymap);

    connect(ui()->shortcutsPushButton, &QToolButton::pressed, this, [&]() {
        const auto idx = ui()->shortcutsComboBox->currentIndex();
        if (idx == -1)
            return;

        const auto geo = ui()->shortcutsPushButton->rect();
        auto point     = ui()->shortcutsPushButton->mapToGlobal(QPoint { geo.left(), geo.bottom() });

        const QString base = ui()->shortcutsComboBox->itemData(idx, Qt::UserRole + 1).toString();

        QMenu contextMenu("Context menu", this);

        contextMenu.addAction(_sctImpl->duplicate);

        if (base != "internal")
        {
            contextMenu.addAction(_sctImpl->rename);
            contextMenu.addAction(_sctImpl->restore);
            contextMenu.addAction(_sctImpl->del);
        }

        contextMenu.exec(point);
    });
}

void SettingsDialog::duplicateKeymap(C_UNUSED bool triggered) noexcept
{
    namespace json = rapidjson;

    const auto currentIndex             = ui()->shortcutsComboBox->currentIndex();
    const QString keymapFile            = ui()->shortcutsComboBox->itemData(currentIndex, Qt::UserRole + 2).toString();
    const QString userDefinedKeymapPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/keymap/";

    QFileInfo nfo(userDefinedKeymapPath);
    QDir dir(nfo.path());
    if (!dir.exists())
    {
        logInfo("duplicateKeymap", tr("User defined keymap path created"));
        if (!dir.mkpath(userDefinedKeymapPath))
        {
            const QString errorMessage = tr("Could not create the user defined keymap. The user defined keymap paths could not be created");
            logError("duplicateKeymap", errorMessage);
            QMessageBox::critical(this,
                tr("Critical"),
                errorMessage,
                QMessageBox::Ok);
            return;
        }
    }

    // Copy the file
    const auto newFileName    = QString::fromStdString(AESSym::createUniqueId(500, 16)) + ".keymap.json";
    const QString newJsonFile = userDefinedKeymapPath + newFileName;
    if (!QFile::copy(keymapFile, newJsonFile))
    {
        const QString errorMessage = tr("Could not copy the keymap. The keymap can not be duplicated");
        logError("duplicateKeymap", errorMessage);
        QMessageBox::critical(this,
            tr("Critical"),
            errorMessage,
            QMessageBox::Ok);
        return;
    }

    // Load the file
    QFile jsonKeymap(newJsonFile);
    if (!jsonKeymap.open(QIODevice::ReadOnly))
    {
        const QString errorMessage = tr("The keymap could not be opened");
        logError("initShortcutsWidget", errorMessage);
        QMessageBox::critical(this,
            tr("Critical"),
            errorMessage,
            QMessageBox::Ok);
        // Remove the copied file
        QFile::remove(newJsonFile);
        return;
    }

    QTextStream jsonFileData(&jsonKeymap);

    json::Document jsonDoc;
    jsonDoc.Parse(jsonFileData.readAll().toUtf8().constData());

    if (jsonDoc.HasParseError())
    {
        const QString errorMessage = tr("The keymap file was copied with errors");
        logError("initShortcutsWidget", errorMessage);
        QMessageBox::critical(this,
            tr("Critical"),
            errorMessage,
            QMessageBox::Ok);
        // Remove the copied file
        QFile::remove(newJsonFile);
        return;
    }

    const int newItemIndex = ui()->shortcutsComboBox->count();

    // Rename the file
    const auto currentKeymapName = ui()->shortcutsComboBox->itemText(currentIndex);
    QString newKeymapName        = "Copy of " + currentKeymapName;
    int copies                   = 1;

    // Avoid duplicated names
    while (ui()->shortcutsComboBox->findText(newKeymapName) != -1)
    {
        while (newKeymapName.back().isDigit())
            newKeymapName.remove(newKeymapName.size() - 1, 1);

        newKeymapName = newKeymapName.trimmed() + (QString(" ") + QString::number(++copies));
    }

    ui()->shortcutsComboBox->insertItem(newItemIndex, newKeymapName);
    // Set the base
    ui()->shortcutsComboBox->setItemData(newItemIndex, currentKeymapName, Qt::UserRole + 1);
    // Set the file
    ui()->shortcutsComboBox->setItemData(newItemIndex, newJsonFile, Qt::UserRole + 2);

    qDebug() << QString(jsonDoc["name"].GetString());

    _sctImpl->docKeymap.emplace(newKeymapName, newJsonFile);
    _sctImpl->docKeymap.at(newKeymapName)().Swap(jsonDoc);

    _sctImpl->docKeymap.at(newKeymapName)()["base"].SetString(_sctImpl->docKeymap.at(newKeymapName)()["name"].GetString(), _sctImpl->docKeymap.at(newKeymapName)().GetAllocator());
    _sctImpl->docKeymap.at(newKeymapName)()["name"].SetString(newKeymapName.toUtf8().constData(), _sctImpl->docKeymap.at(newKeymapName)().GetAllocator());
}

void SettingsDialog::renameKeymap(C_UNUSED bool triggered) noexcept
{
    const auto currentIndex   = ui()->shortcutsComboBox->currentIndex();
    const QString currentText = ui()->shortcutsComboBox->currentText();

    bool ok;
    const QString newText = QInputDialog::getText(
        this,
        tr("Rename keymap"),
        tr("New keymap new"), QLineEdit::Normal,
        currentText, &ok);

    if (!ok || newText.isEmpty() || currentText == newText)
        return;

    if (ui()->shortcutsComboBox->findText(newText) != -1)
    {
        QMessageBox::information(this,
            tr("Repeated name"),
            tr("There is already a keymap with that name"),
            QMessageBox::Ok);

        return;
    }

    ui()->shortcutsComboBox->setItemText(currentIndex, newText);

    // Create a new key and swap the json dom tree
    _sctImpl->docKeymap.emplace(newText, _sctImpl->docKeymap.at(currentText).file);
    _sctImpl->docKeymap.at(newText)().Swap(_sctImpl->docKeymap.at(currentText)());
    _sctImpl->docKeymap.at(newText)()["name"].SetString(newText.toUtf8().constData(), _sctImpl->docKeymap.at(newText)().GetAllocator());

    // remove the previous key
    _sctImpl->docKeymap.erase(currentText);
}

void SettingsDialog::restoreDefaultKeymap(C_UNUSED bool triggered) noexcept
{
    const auto currentIndex   = ui()->shortcutsComboBox->currentIndex();
    const QString currentText = ui()->shortcutsComboBox->currentText();

    auto response = QMessageBox::question(this,
        tr("Delete keymap"),
        tr("Are you sure you want to restore the defaults for %1").arg(currentText),
        QMessageBox::Yes | QMessageBox::No);

    if (response == QMessageBox::No)
        return;
}

void SettingsDialog::removeKeymap(C_UNUSED bool triggered) noexcept
{
    const auto currentIndex   = ui()->shortcutsComboBox->currentIndex();
    const QString currentText = ui()->shortcutsComboBox->currentText();

    auto response = QMessageBox::question(this,
        tr("Delete keymap"),
        tr("Are you sure you want to delete %1").arg(currentText),
        QMessageBox::Yes | QMessageBox::No);

    if (response == QMessageBox::No)
        return;

    const QString file = ui()->shortcutsComboBox->itemData(currentIndex, Qt::UserRole + 2).toString();

    if (!QFile::remove(file))
    {
        const QString errorMessage = tr("The keymap file was not removed");
        logError("initShortcutsWidget", errorMessage);
        QMessageBox::critical(this,
            tr("Critical"),
            errorMessage,
            QMessageBox::Ok);
    }

    // Remove the list
    ui()->shortcutsComboBox->removeItem(currentIndex);

    // Remove from the item
    _sctImpl->docKeymap.at(currentText).writeAtDestruction = false; // Prevent the call to write when the object is destructed
    _sctImpl->docKeymap.erase(currentText);

    logInfo("removeKeymap", tr("Keymap %1 was removed").arg(currentText));
}

void SettingsDialog::shortcutSelectionChanged(int index) noexcept
{
    const auto currentComboText = ui()->shortcutsComboBox->currentText();

    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("Settings.Keymap");
    settings.setValue("keymap", currentComboText);
    settings.endGroup();
}

END_CENTAUR_NAMESPACE
