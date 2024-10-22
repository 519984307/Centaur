/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "../ui/ui_SettingsDialog.h"
#include "Globals.hpp"
#include "SettingsDialog.hpp"
#include "ShortcutDialog.hpp"

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

SettingsDialog::ShortcutsImpl::KeymapFileNameMap::KeymapFileNameMap()
{
    keyNameMap["Window"]            = tr("Main window");
    keyNameMap["Tools"]             = tr("Main tool windows");
    keyNameMap["close-window"]      = tr("Close main window");
    keyNameMap["open-settings"]     = tr("Open settings dialog");
    keyNameMap["open-plugins-info"] = tr("Open plugins information");
    keyNameMap["open-log-window"]   = tr("Open log dialog");
}

void SettingsDialog::initShortcutsWidget() noexcept
{
    namespace json      = rapidjson;
    _sctImpl->itemModel = new QStandardItemModel(0, 2, ui()->shortcutsPage);
    auto proxyModel     = new QSortFilterProxyModel(this);

    proxyModel->setFilterKeyColumn(0);
    proxyModel->setSourceModel(_sctImpl->itemModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setRecursiveFilteringEnabled(true);

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
        if (savedIndex > 0)
            ui()->shortcutsComboBox->setCurrentIndex(savedIndex);
        else
        {
            if (savedIndex == 0)
                shortcutSelectionChanged(0);
        }
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

    connect(ui()->shortcutsTree, &QTreeView::doubleClicked, this, &SettingsDialog::shortcutDoubleClick);
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
        tr("Restore keymap"),
        tr("Are you sure you want to restore the defaults for %1").arg(currentText),
        QMessageBox::Yes | QMessageBox::No);

    if (response == QMessageBox::No)
        return;

    const auto baseString = ui()->shortcutsComboBox->itemData(currentIndex, Qt::UserRole + 1).toString();

    auto baseIter = _sctImpl->docKeymap.find(baseString);

    if (baseIter == _sctImpl->docKeymap.end())
    {
        QMessageBox::information(this,
            tr("Restore keymap"),
            tr("Origin '%1' of the current key is not present").arg(currentText),
            QMessageBox::Yes);

        return;
    }

    const auto &originDoc = baseIter->second();
    auto &currentDoc      = _sctImpl->docKeymap.at(currentText)()["keymap"];

    currentDoc.CopyFrom(originDoc["keymap"], _sctImpl->docKeymap.at(currentText)().GetAllocator());

    shortcutSelectionChanged(currentIndex);
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
    const auto currentComboText = ui()->shortcutsComboBox->itemText(index);

    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("Settings.Keymap");
    settings.setValue("keymap", currentComboText);
    settings.endGroup();

    const auto &jsonDoc = _sctImpl->docKeymap.at(currentComboText)()["keymap"];
    const auto &trMap   = _sctImpl->shortcutNameMap.keyNameMap;

    _sctImpl->itemModel->removeRows(0, _sctImpl->itemModel->rowCount());

    for (const auto &topLevel : jsonDoc.GetObject())
    {
        auto topLevelIter = trMap.find(QString(topLevel.name.GetString()));
        if (topLevelIter == trMap.end())
        {
            logWarn("shortcut", "found an invalid key");
            continue;
        }

        QStandardItem *item = new QStandardItem(topLevelIter->second);
        _sctImpl->itemModel->insertRow(_sctImpl->itemModel->rowCount(), item);

        QString jsonTopLevel { topLevel.name.GetString() };

        for (auto &innerLevel : topLevel.value.GetArray())
        {
            const QString id { innerLevel["id"].GetString() };
            const QString shortcut { innerLevel["shortcut"].GetString() };

            auto innerLevelIter = trMap.find(id);
            if (innerLevelIter == trMap.end())
            {
                logWarn("shortcut", tr("found an invalid key in the inner levels %1").arg(id));
                continue;
            }

            QStandardItem *innerName     = new QStandardItem(innerLevelIter->second);
            QStandardItem *innerShortcut = new QStandardItem(QKeySequence::fromString(shortcut, QKeySequence::PortableText).toString(QKeySequence::NativeText));

            innerShortcut->setData(jsonTopLevel, Qt::UserRole + 1);
            innerShortcut->setData(innerLevel["id"].GetString(), Qt::UserRole + 2);

            item->insertRow(0, { innerName, innerShortcut });
        }
    }

    ui()->shortcutsTree->expandAll();
}

void SettingsDialog::shortcutDoubleClick(const QModelIndex &index) noexcept
{
    if (!index.isValid())
        return;

    if (!index.parent().isValid())
    {
        QMessageBox::information(this,
            tr("Wrong index"),
            tr("Top level items are not modifiable"),
            QMessageBox::Ok);

        return;
    }

    auto proxyModel = qobject_cast<QSortFilterProxyModel *>(ui()->shortcutsTree->model());

    int selectionRow = index.row();

    auto nameIndex     = proxyModel->mapToSource(proxyModel->index(selectionRow, 0, index.parent()));
    auto shortcutIndex = proxyModel->mapToSource(proxyModel->index(selectionRow, 1, index.parent()));

    if (!nameIndex.isValid() || !shortcutIndex.isValid())
        return;

    ShortcutDialog dlg(this);

    dlg.setShortcutInformation(nameIndex.data().toString(), shortcutIndex.data().toString());

    if (dlg.exec() == QDialog::Accepted)
    {
        const auto &topLevel   = shortcutIndex.data(Qt::UserRole + 1).toString();
        const auto &innerLevel = shortcutIndex.data(Qt::UserRole + 2).toString();

        const auto currentIndex  = ui()->shortcutsComboBox->currentText();
        const auto &jsonDocArray = _sctImpl->docKeymap.at(currentIndex)()["keymap"][topLevel.toUtf8().constData()].GetArray();

        const auto shortcutString = dlg.getShortcut();

        for (auto &doc : jsonDocArray)
        {
            if (innerLevel == doc["id"].GetString())
            {
                doc["shortcut"].SetString(shortcutString.toString(QKeySequence::PortableText).toUtf8().constData(), _sctImpl->docKeymap.at(currentIndex)().GetAllocator());
                break;
            }
        }

        _sctImpl->itemModel->setData(shortcutIndex, shortcutString.toString(QKeySequence::NativeText), Qt::DisplayRole);
    }
}

END_CENTAUR_NAMESPACE
