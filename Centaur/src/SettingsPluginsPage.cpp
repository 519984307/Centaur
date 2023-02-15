/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "../ui/ui_SettingsDialog.h"
#include "CentaurApp.hpp"
#include "DAL.hpp"
#include "SettingsDialog.hpp"

#include <QCryptographicHash>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QProgressDialog>
#include <QSettings>
#include <QStyledItemDelegate>
#include <QtConcurrent>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/schema.h>

#include <zip.h>

namespace
{
    constexpr int _settings_plugin_enable_magic = 0x70454545;

    class DelegateItem : public QStyledItemDelegate
    {
    public:
        explicit DelegateItem(QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option4, const QModelIndex &index) const override
        {
            QStyleOptionViewItem options { option4 };
            initStyleOption(&options, index);

            if (options.state & QStyle::State_Selected)
                QStyledItemDelegate::paint(painter, options, index);
            else
            {
                painter->save();
                options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

                painter->restore();
            }
        }
    };
} // namespace

DelegateItem::DelegateItem(QObject *parent) :
    QStyledItemDelegate { parent }
{
}

BEGIN_CENTAUR_NAMESPACE

void SettingsDialog::showWarningLabels() noexcept
{
    ui()->warnLabel->setVisible(true);
    ui()->warnIconLabel->setVisible(true);
}

void SettingsDialog::hideWarningLabels() noexcept
{
    ui()->warnLabel->setVisible(false);
    ui()->warnIconLabel->setVisible(false);
}

void SettingsDialog::initPluginsWidget() noexcept
{
    using namespace dal;

#ifdef NO_PLUGIN_CHECKSUM_CHECK
    ui()->warningChecksumIconLabel->setVisible(true);
    ui()->warningChecksumLabel->setVisible(true);
#else
    ui()->warningChecksumIconLabel->setVisible(false);
    ui()->warningChecksumLabel->setVisible(false);
#endif

    ui()->pluginsTableWidget->setColumnCount(7);

    ui()->pluginsTableWidget->setItemDelegate(new DelegateItem);

    ui()->pluginsTableWidget->setHorizontalHeaderLabels(
        { QString(tr("Name")),
            QString(tr("Version")),
            QString(tr("Manufacturer")),
            QString(tr("UI Min. Version")),
            QString(tr("Identification")),
            QString(tr("Protected")),
            QString(tr("Loaded")) });

    ui()->pluginsTableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    auto plInfo = DataAccess::pluginInformation();

    if (!plInfo.has_value())
    {
        QMessageBox::critical(this, tr("Error"),
            QString(tr("The plugin information could not be retrieved")),
            QMessageBox::Ok);
    }
    else
    {
        for (const auto &data : *plInfo)
        {
            auto row = ui()->pluginsTableWidget->rowCount();

            ui()->pluginsTableWidget->insertRow(row);

            auto *itemName      = new QTableWidgetItem(QIcon(":/ui/settings/plugins/plugin"), data.name);
            auto *itemVersion   = new QTableWidgetItem(data.version);
            auto *itemManu      = new QTableWidgetItem(data.man);
            auto *itemUIVer     = new QTableWidgetItem(data.centaur_uuid);
            auto *itemUUID      = new QTableWidgetItem(data.uuid);
            auto *itemProtected = new QTableWidgetItem(data.protect ? "Yes" : "No");
            auto *itemLoaded    = new QTableWidgetItem(tr("Not Loaded"));

            itemName->setBackground(QColor(255, 0, 0, 25));
            itemVersion->setBackground(QColor(255, 0, 0, 25));
            itemManu->setBackground(QColor(255, 0, 0, 25));
            itemUIVer->setBackground(QColor(255, 0, 0, 25));
            itemUUID->setBackground(QColor(255, 0, 0, 25));
            itemProtected->setBackground(QColor(255, 0, 0, 25));
            itemLoaded->setBackground(QColor(255, 0, 0, 25));

            itemName->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            itemVersion->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemManu->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUIVer->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUUID->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemProtected->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemLoaded->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            itemName->setCheckState(data.enabled ? Qt::Checked : Qt::Unchecked);

            itemName->setData(Qt::UserRole + 1, _settings_plugin_enable_magic);
            itemName->setData(Qt::UserRole + 2, data.uuid);

            itemLoaded->setData(Qt::UserRole + 1, 0);

            itemName->setToolTip(tr("Check for enable the plugin\nUncheck for disabling the plugin"));

            for (auto &loadedPlugins : g_app->getPluginBase())
            {
                if (loadedPlugins->getPluginUUID() != data.uuid)
                {
                    itemLoaded->setText(tr("Loaded"));
                    itemLoaded->setData(Qt::UserRole + 1, 1);

                    itemName->setBackground(QColor(0, 255, 0, 25));
                    itemVersion->setBackground(QColor(0, 255, 0, 25));
                    itemManu->setBackground(QColor(0, 255, 0, 25));
                    itemUIVer->setBackground(QColor(0, 255, 0, 25));
                    itemUUID->setBackground(QColor(0, 255, 0, 25));
                    itemProtected->setBackground(QColor(0, 255, 0, 25));
                    itemLoaded->setBackground(QColor(0, 255, 0, 25));

                    break;
                }
            }

            ui()->pluginsTableWidget->setItem(row, 0, itemName);
            ui()->pluginsTableWidget->setItem(row, 1, itemVersion);
            ui()->pluginsTableWidget->setItem(row, 2, itemManu);
            ui()->pluginsTableWidget->setItem(row, 3, itemUIVer);
            ui()->pluginsTableWidget->setItem(row, 4, itemUUID);
            ui()->pluginsTableWidget->setItem(row, 5, itemProtected);
            ui()->pluginsTableWidget->setItem(row, 6, itemLoaded);
        }
    }

    connect(ui()->pluginsTableWidget, &QTableWidget::itemSelectionChanged, this, [this]() {
        if (ui()->pluginsTableWidget->selectedItems().size() != 1)
        {
            ui()->uninstallButton->setDisabled(true);
            return;
        }
        ui()->uninstallButton->setDisabled(false);
    });

    connect(ui()->pluginsTableWidget, &QTableWidget::itemChanged, this, &SettingsDialog::pluginsTableItemChanged);

    connect(ui()->installButton, &QPushButton::released, this, &SettingsDialog::installPlugin);
    connect(ui()->uninstallButton, &QPushButton::released, this, &SettingsDialog::uninstallPlugin);
}

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_PUSH()
CENTAUR_WARN_OFF("-Wnullable-to-nonnull-conversion")
#endif

void cen::SettingsDialog::installPlugin() noexcept
{
    using namespace dal;

    QFileDialog dlg(this);

    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("settings.Plugins");
    auto lastDir = settings.value("Install.LastDirectory", QString()).toString();
    settings.endGroup();

    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setViewMode(QFileDialog::Detail);

    if (!lastDir.isEmpty())
    {
        dlg.setDirectory(lastDir);
    }

    dlg.setNameFilter("Plugin files (*.cpk *.zip)");

    if (dlg.exec() == QDialog::Accepted)
    {
        settings.beginGroup("settings.Plugins");
        settings.setValue("Install.LastDirectory", dlg.directory().absolutePath());
        settings.endGroup();

        QProgressDialog prgDlg(this);

        prgDlg.setAutoClose(true);
        prgDlg.setRange(0, static_cast<int>(dlg.selectedFiles().size()));
        prgDlg.setValue(0);
        prgDlg.setCancelButton(nullptr);

        QFutureWatcher<void> watcher;

        connect(&watcher, &QFutureWatcher<void>::finished, &prgDlg, &QProgressDialog::reset);
        connect(&watcher, &QFutureWatcher<void>::progressValueChanged, &prgDlg, &QProgressDialog::setValue);
        connect(&watcher, &QFutureWatcher<void>::progressTextChanged, &prgDlg, &QProgressDialog::setLabelText);

        QList<QPair<QString, PluginData>> toTableData;
        QStringList errorStrings;

        watcher.setFuture(
            QtConcurrent::run(QThreadPool::globalInstance(),
                [&]() {
                    const QString &resPath         = g_globals->paths.resPath;
                    const QString schemaJSONPlugin = resPath + "/Schema/plugins.schema.json";
                    // Open the plugin schema
                    namespace json = rapidjson;

                    QFile file(schemaJSONPlugin);
                    if (!file.open(QIODevice::ReadOnly))
                    {
                        errorStrings.push_back(tr("An internal file is missing. Reinstalling the application might solve the problem"));
                        return;
                    }

                    QTextStream textStream(&file);

                    json::Document schemaJSONDoc;
                    schemaJSONDoc.Parse(textStream.readAll().toUtf8().constData());
                    if (schemaJSONDoc.HasParseError())
                    {
                        errorStrings.push_back(tr("An internal file is corrupted. Reinstalling the application might solve the problem"));
                        return;
                    }

                    json::SchemaDocument schemaDoc(schemaJSONDoc);
                    json::SchemaValidator schemaValidator(schemaDoc);

                    int ipv = 0;
                    for (const auto &file : dlg.selectedFiles())
                    {
                        emit watcher.progressTextChanged(file);

                        int z_error      = ZIP_ER_OK;
                        auto zip_archive = zip_open(file.toLatin1().constData(), ZIP_RDONLY, &z_error);

                        if (z_error != ZIP_ER_OK)
                        {
                            errorStrings.push_back(tr("The file %1 is not a valid plugin package").arg(file));
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto json_index = zip_name_locate(zip_archive, "plugin.json", ZIP_FL_NOCASE | ZIP_FL_ENC_UTF_8);

                        if (json_index == -1)
                        {
                            errorStrings.push_back(tr("%1\ndoes not contain any plugin metadata information").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        zip_stat_t json_stats;

                        auto json_st   = zip_stat_index(zip_archive, static_cast<zip_uint64_t>(json_index), ZIP_STAT_SIZE, &json_stats);
                        auto json_file = zip_fopen_index(zip_archive, static_cast<zip_uint64_t>(json_index), 0);

                        if (json_st == -1 || json_stats.size <= 0)
                        {
                            errorStrings.push_back(tr("%1\nmetadata file is not available").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QScopedArrayPointer<char> _file_bytes(new char[json_stats.size + 1]);
                        auto bytes_read                    = zip_fread(json_file, _file_bytes.get(), json_stats.size);
                        _file_bytes.get()[json_stats.size] = 0;

                        if (bytes_read < 0)
                        {
                            errorStrings.push_back(tr("%1\nmetadata file was not read").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        json::Document jsonDoc;
                        jsonDoc.Parse(_file_bytes.get());
                        if (jsonDoc.HasParseError())
                        {
                            errorStrings.push_back(tr("%1\nmetadata file is not valid").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        if (!jsonDoc.Accept(schemaValidator))
                        {
                            errorStrings.push_back(tr("%1\nmetadata file contain an invalid format").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QString plVersion { jsonDoc["version"].GetString() };

                        // Currently there's only version 0.1.0
                        if (plVersion != "0.1.0")
                        {
                            errorStrings.push_back(tr("%1\nfile format version is not supported").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto exists = DataAccess::pluginExists(QString { jsonDoc["identification"]["uuid"].GetString() });
                        if (exists)
                        {
                            errorStrings.push_back(tr("%1\nhas an unready installed plugin").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto min_ui_version = QString { jsonDoc["identification"]["ui-version"]["min-uuid"].GetString() };

                        // Currently there is only version 0.1.0
                        if (min_ui_version != "a15c48b4-460b-4a79-a0a8-8ece90603f85")
                        {
                            errorStrings.push_back(tr("%1\nis not supported by this version of the UI").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        // Check for the Data integrity
                        auto hash = QString { jsonDoc["identification"]["checksum"].GetString() };

                        auto dynlib_index = zip_name_locate(zip_archive, jsonDoc["identification"]["dynamic"].GetString(), ZIP_FL_NOCASE | ZIP_FL_ENC_UTF_8);

                        if (dynlib_index == -1)
                        {
                            errorStrings.push_back(tr("%1\ndoes not contain any plugin").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        zip_stat_t dynlib_stats;
                        auto dynlib_st   = zip_stat_index(zip_archive, static_cast<zip_uint64_t>(dynlib_index), ZIP_STAT_SIZE, &dynlib_stats);
                        auto dynlib_file = zip_fopen_index(zip_archive, static_cast<zip_uint64_t>(dynlib_index), 0);

                        if (dynlib_st == -1 || dynlib_stats.size <= 0)
                        {
                            errorStrings.push_back(QString(tr("%1\nplugin file is not available")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QScopedArrayPointer<unsigned char> _plugin_bytes(new unsigned char[dynlib_stats.size + 1ull]);
                        bytes_read = zip_fread(dynlib_file, _plugin_bytes.get(), dynlib_stats.size);

                        if (bytes_read < 0)
                        {
                            errorStrings.push_back(tr("%1\nplugin file was not read").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto calcHash = QCryptographicHash::hash({ _plugin_bytes.get(), static_cast<qsizetype>(dynlib_stats.size) }, QCryptographicHash::Sha224).toHex();

                        if (calcHash != hash)
                        {
                            errorStrings.push_back(tr("%1\nplugin file is corrupted").arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        zip_close(zip_archive);

                        const QString pluginFile = [&]() -> QString {
                            QString data = g_globals->paths.pluginsPath;
                            return QString("%1/%2").arg(data, QString { jsonDoc["identification"]["dynamic"].GetString() });
                        }();

                        QFile toDiscPluginFile(pluginFile);
                        if (!toDiscPluginFile.open(QIODeviceBase::NewOnly | QIODeviceBase::WriteOnly))
                        {
                            errorStrings.push_back(tr("%1\nthe plugin file could not be opened to write").arg(file));
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QDataStream dataStream(&toDiscPluginFile);
                        auto nWritten = dataStream.writeRawData(reinterpret_cast<const char *>(_plugin_bytes.get()), static_cast<int>(dynlib_stats.size));
                        if (nWritten < static_cast<int>(dynlib_stats.size))
                        {
                            errorStrings.push_back(tr("%1\nthe plugin file could not be written").arg(file));
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        toDiscPluginFile.close();

                        QFileInfo nfo(pluginFile);
                        toTableData.push_back({
                            pluginFile, {jsonDoc["identification"]["name"].GetString(),
                                         jsonDoc["identification"]["version"].GetString(),
                                         jsonDoc["identification"]["manufacturer"].GetString(),
                                         jsonDoc["identification"]["uuid"].GetString(),
                                         jsonDoc["identification"]["ui-version"]["min-uuid"].GetString(),
                                         hash,
                                         nfo.fileName(),
                                         true,
                                         jsonDoc["identification"]["protected"].GetBool()}
                        });

                        emit watcher.progressValueChanged(++ipv);
                    }
                }));

        prgDlg.exec();

        watcher.waitForFinished();

        if (!errorStrings.empty())
        {
            int i = 0;
            QString errors;
            for (const auto &er : errorStrings)
                errors += QString("%1. %2\n\n").arg(++i).arg(er);

            QMessageBox::critical(this, tr("Error"),
                errors,
                QMessageBox::Ok);
        }

        for (const auto &[pluginFile, data] : toTableData)
        {
            auto insertionResult = DataAccess::insertPlugin(
                data.name,
                data.version,
                data.man,
                data.uuid,
                data.centaur_uuid,
                data.checksum,
                data.dynamic,
                true,
                data.protect);

            if (!insertionResult.has_value())
            {
                QFile::remove(pluginFile);
                QMessageBox::critical(this, tr("Error"),
                    tr("%1\n Could not be installed. Query Error").arg(pluginFile),
                    QMessageBox::Ok);

                continue;
            }

            if (!*insertionResult)
            {

                QMessageBox::critical(this, tr("Error"),
                    tr("%1\nPlugin was not installed").arg(pluginFile),
                    QMessageBox::Ok);

                QFile::remove(pluginFile);
                continue;
            }

            auto row = ui()->pluginsTableWidget->rowCount();

            ui()->pluginsTableWidget->insertRow(row);

            auto uiVersion      = DataAccess::fromUUIDtoVersion(data.centaur_uuid);
            auto *itemName      = new QTableWidgetItem(QIcon(":/ui/settings/plugins/plugin"), data.name);
            auto *itemVersion   = new QTableWidgetItem(data.version);
            auto *itemManu      = new QTableWidgetItem(data.man);
            auto *itemUIVer     = new QTableWidgetItem(uiVersion.has_value() ? *uiVersion : data.centaur_uuid);
            auto *itemUUID      = new QTableWidgetItem(data.uuid);
            auto *itemProtected = new QTableWidgetItem(data.protect ? "Yes" : "No");
            auto *itemLoaded    = new QTableWidgetItem(tr("Not Loaded"));

            itemName->setBackground(QColor(255, 255, 0, 25));
            itemVersion->setBackground(QColor(255, 255, 0, 25));
            itemManu->setBackground(QColor(255, 255, 0, 25));
            itemUIVer->setBackground(QColor(255, 255, 0, 25));
            itemUUID->setBackground(QColor(255, 255, 0, 25));
            itemProtected->setBackground(QColor(255, 255, 0, 25));
            itemLoaded->setBackground(QColor(255, 255, 0, 25));

            itemName->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            itemVersion->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemManu->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUIVer->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUUID->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemProtected->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemLoaded->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            itemName->setCheckState(data.enabled ? Qt::Checked : Qt::Unchecked);

            itemName->setData(Qt::UserRole + 1, _settings_plugin_enable_magic);
            itemName->setData(Qt::UserRole + 2, data.uuid);

            itemLoaded->setData(Qt::UserRole + 1, -1);

            itemName->setToolTip(tr("Check for enable the plugin\nUncheck for disabling the plugin"));

            ui()->pluginsTableWidget->setItem(row, 0, itemName);
            ui()->pluginsTableWidget->setItem(row, 1, itemVersion);
            ui()->pluginsTableWidget->setItem(row, 2, itemManu);
            ui()->pluginsTableWidget->setItem(row, 3, itemUIVer);
            ui()->pluginsTableWidget->setItem(row, 4, itemUUID);
            ui()->pluginsTableWidget->setItem(row, 5, itemProtected);
            ui()->pluginsTableWidget->setItem(row, 6, itemLoaded);

            _impl->forceShowPluginWarning = true;
            showWarningLabels();
        }
    }
}

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_POP()
#endif

void cen::SettingsDialog::uninstallPlugin() noexcept
{
    using namespace dal;

    auto res = QMessageBox::question(this,
        tr("Uninstall"),
        tr("Are you sure you want to uninstall this plugin?"),
        QMessageBox::Yes | QMessageBox::No);

    if (res == QMessageBox::No)
        return;

    const auto &selectedItem = ui()->pluginsTableWidget->selectedItems().front();

    // Get the UUID item
    auto uuidItem = selectedItem->column() == 4 ? selectedItem : ui()->pluginsTableWidget->item(selectedItem->row(), 4);

    const QString uuid = uuidItem->text();

    const auto dynFile = DataAccess::getDynamicFieldPlugin(uuid);
    if (!dynFile.has_value())
    {
        QMessageBox::critical(this,
            tr("Error"),
            tr("The plugin can not be uninstalled"),
            QMessageBox::Yes);
        return;
    }

    const auto remResult = DataAccess::removePlugin(uuid);

    if (!remResult.has_value() || !*remResult)
    {
        QMessageBox::critical(this,
            tr("Error"),
            tr("The plugin could not be removed from the database"),
            QMessageBox::Yes);
        return;
    }

    ui()->pluginsTableWidget->removeRow(uuidItem->row());
    showWarningLabels();
    _impl->forceShowPluginWarning = true;

    if (!QFile::remove(g_globals->paths.pluginsPath + "/" + *dynFile))
    {
        QMessageBox::critical(this,
            tr("Error"),
            tr("The plugin file could not be deleted from the filesystem.\nReinstalling this plugin can't be done unless the file is removed manually"),
            QMessageBox::Yes);
        return;
    }
}

void cen::SettingsDialog::pluginsTableItemChanged(QTableWidgetItem *item) noexcept
{
    using namespace dal;

    bool valid;
    auto isEnablingItem = item->data(Qt::UserRole + 1).toInt(&valid);

    if (!valid)
        return;

    if (isEnablingItem != _settings_plugin_enable_magic)
        return;

    const auto &uuid = item->data(Qt::UserRole + 2).toString();

    auto result = DataAccess::updateEnabledState(uuid, item->checkState() == Qt::Checked ? true : false);

    if (!result.has_value())
    {
        QMessageBox::critical(this, tr("Error"),
            QString(tr("The action failed to execute")),
            QMessageBox::Ok);

        return;
    }

    if (!*result)
    {
        QMessageBox::critical(this, tr("Error"),
            QString(tr("The plugin information could not be updated")),
            QMessageBox::Ok);

        return;
    }

    auto isLoadedItem = ui()->pluginsTableWidget->item(item->row(), 5);
    if (isLoadedItem == nullptr)
        return;

    if ((isLoadedItem->data(Qt::UserRole + 1).toInt() == 1 && item->checkState() == Qt::Unchecked)
        || (isLoadedItem->data(Qt::UserRole + 1).toInt() == 0 && item->checkState() == Qt::Checked)
        || isLoadedItem->data(Qt::UserRole + 1).toInt() == -1)
    {
        for (int i = 0; i <= 5; ++i)
        {
            auto selItem = ui()->pluginsTableWidget->item(item->row(), i);
            if (selItem != nullptr)
            {
                selItem->setBackground(QColor(255, 255, 0, 25));
            }
        }
    }

    if (isLoadedItem->data(Qt::UserRole + 1).toInt() == 1 && item->checkState() == Qt::Checked)
    {
        for (int i = 0; i <= 5; ++i)
        {
            auto selItem = ui()->pluginsTableWidget->item(item->row(), i);
            if (selItem != nullptr)
            {
                selItem->setBackground(QColor(0, 255, 0, 25));
            }
        }
    }

    if (isLoadedItem->data(Qt::UserRole + 1).toInt() == 0 && item->checkState() == Qt::Unchecked)
    {
        for (int i = 0; i <= 5; ++i)
        {
            auto selItem = ui()->pluginsTableWidget->item(item->row(), i);
            if (selItem != nullptr)
            {
                selItem->setBackground(QColor(255, 0, 0, 25));
            }
        }
    }

    for (int i = 0; i < ui()->pluginsTableWidget->rowCount(); ++i)
    {
        auto selItem = ui()->pluginsTableWidget->item(i, 0);
        if (selItem)
        {
            if (selItem->background().color() == QColor(255, 255, 0, 25))
            {
                showWarningLabels();
                return;
            }
        }
    }

    hideWarningLabels();

    if (_impl->forceShowPluginWarning)
        showWarningLabels();
}

END_CENTAUR_NAMESPACE
