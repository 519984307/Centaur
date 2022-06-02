/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 26/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PluginsDialog.h" resolved

#include "PluginsDialog.hpp"
#include "../ui/ui_PluginsDialog.h"
#include "CentaurApp.hpp"
#include "CentaurPlugin.hpp"
#include "Globals.hpp"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQueryModel>
#include <fmt/core.h>

namespace CENTAUR_NAMESPACE
{
#if defined(__clang__) || defined(__GNUC__)
    // clang-format off
CENTAUR_WARN_PUSH()
CENTAUR_WARN_OFF(weak-vtables)
    // clang-format on
#endif /*__clang__*/
    class PluginsSQLModel final : public QSqlQueryModel
    {
    public:
        inline explicit PluginsSQLModel(QObject *parent) :
            QSqlQueryModel(parent) { }

        C_NODISCARD inline QVariant data(const QModelIndex &index, int role) const override
        {
            QVariant value = QSqlQueryModel::data(index, role);

            if (value.isValid())
            {
                if (role == Qt::DisplayRole)
                {
                    if (index.column() == 4 && value.toString() == CENTAUR_PLUGIN_NAMESPACE::centaurUUID)
                    {
                        value.setValue(QVariant { CentaurVersionString });
                    }
                    else if (index.column() == 5)
                    {
                        bool loaded = false;
                        auto plid   = this->index(index.row(), 3);

                        for (auto &loadedPlugins : g_app->m_pluginsData)
                        {
                            QString strPlid { loadedPlugins->getPluginUUID().to_string().c_str() };
                            if (strPlid == plid.data())
                            {
                                value.setValue(LS("ui-dialog-loaded"));
                                loaded = true;
                                break;
                            }
                        }
                        if (!loaded)
                            value.setValue(LS("ui-dialog-not-loaded"));
                    }
                }
            }
            else
            {
                switch (role)
                {
                    case Qt::DecorationRole:
                        if (index.column() == 0)
                        {
                            value.setValue(g_globals->icons.pluginIcon);
                        }
                        break;
                    case Qt::BackgroundRole:
                        {
                            auto plid = this->index(index.row(), 5);
                            QString ld { plid.data(Qt::DisplayRole).toString() };
                            if (ld == LS("ui-dialog-loaded"))
                                value.setValue(QColor(0, 255, 0, 35));
                            else
                                value.setValue(QColor(255, 0, 0, 35));
                        }
                        break;
                    default:;
                }
            }

            return value;
        }
    };
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_POP()
#endif /*__clang__*/
} // namespace CENTAUR_NAMESPACE

CENTAUR_NAMESPACE::PluginsDialog::PluginsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PluginsDialog)
{
    m_ui->setupUi(this);

    loadInterfaceState();

    auto model = new PluginsSQLModel(this);
    model->setQuery("SELECT name,version,manufacturer,uuid,centaur_uuid,id FROM plugins;");
    model->setHeaderData(0, Qt::Horizontal, LS("ui-dialog-plginfo-name"));
    model->setHeaderData(1, Qt::Horizontal, LS("ui-dialog-plginfo-version"));
    model->setHeaderData(2, Qt::Horizontal, LS("ui-dialog-plginfo-manufacturer"));
    model->setHeaderData(3, Qt::Horizontal, LS("ui-dialog-plginfo-id"));
    model->setHeaderData(4, Qt::Horizontal, LS("ui-dialog-plginfo-centaur"));
    model->setHeaderData(5, Qt::Horizontal, LS("ui-dialog-plginfo-loaded"));

    if (model->lastError().isValid())
    {
        logError("app", QString(LS("error-plugin-db-select")).arg(model->lastError().text()));
    }
    else
    {
        m_ui->pluginsTable->setModel(model);

        m_ui->pluginsTable->setColumnWidth(0, m_state.datacols.name);
        m_ui->pluginsTable->setColumnWidth(1, m_state.datacols.version);
        m_ui->pluginsTable->setColumnWidth(2, m_state.datacols.manu);
        m_ui->pluginsTable->setColumnWidth(3, m_state.datacols.plid);
        m_ui->pluginsTable->setColumnWidth(4, m_state.datacols.cver);
        m_ui->pluginsTable->setColumnWidth(5, m_state.datacols.lded);
    }

    m_ui->pluginsTable->setIconSize({ 16, 16 });
}

CENTAUR_NAMESPACE::PluginsDialog::~PluginsDialog() = default;

void CENTAUR_NAMESPACE::PluginsDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("pluginsDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("listState");
    settings.setValue("c0", m_ui->pluginsTable->columnWidth(0));
    settings.setValue("c1", m_ui->pluginsTable->columnWidth(1));
    settings.setValue("c2", m_ui->pluginsTable->columnWidth(2));
    settings.setValue("c3", m_ui->pluginsTable->columnWidth(3));
    settings.setValue("c4", m_ui->pluginsTable->columnWidth(4));
    settings.setValue("c5", m_ui->pluginsTable->columnWidth(5));
    settings.endGroup();
}

void CENTAUR_NAMESPACE::PluginsDialog::loadInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("pluginsDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();

    settings.beginGroup("listState");
    m_state.datacols.name    = settings.value("c0", m_state.datacols.name).toInt();
    m_state.datacols.version = settings.value("c1", m_state.datacols.version).toInt();
    m_state.datacols.manu    = settings.value("c2", m_state.datacols.manu).toInt();
    m_state.datacols.plid    = settings.value("c3", m_state.datacols.plid).toInt();
    m_state.datacols.cver    = settings.value("c4", m_state.datacols.cver).toInt();
    m_state.datacols.lded    = settings.value("c5", m_state.datacols.lded).toInt();
    settings.endGroup();
}

void CENTAUR_NAMESPACE::PluginsDialog::accept()
{
    saveInterfaceState();
    QDialog::accept();
}
