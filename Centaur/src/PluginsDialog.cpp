/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 26/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PluginsDialog.h" resolved

#include "PluginsDialog.hpp"
#include "../ui/ui_PluginsDialog.h"
#include "CentaurApp.hpp"
#include "Globals.hpp"
#include <QMessageBox>
#include <fstream>

#include <fmt/core.h>

CENTAUR_NAMESPACE::PluginsDialog::PluginsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PluginsDialog)
{
    m_ui->setupUi(this);

    loadInterfaceState();

    m_ui->m_pluginsTable->setHorizontalHeaderLabels({ tr("Name"), tr("Version"), tr("Manufacturer"), tr("Centaur Version"), tr("Subsystem version"), tr("PluginUUID") });
    m_ui->m_pluginsTable->horizontalHeaderItem(0)->setFont(QFont("Arial", 10));
    m_ui->m_pluginsTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    m_ui->m_pluginsTable->horizontalHeaderItem(1)->setFont(QFont("Arial", 10));
    m_ui->m_pluginsTable->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    m_ui->m_pluginsTable->horizontalHeaderItem(2)->setFont(QFont("Arial", 10));
    m_ui->m_pluginsTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);
    m_ui->m_pluginsTable->horizontalHeaderItem(3)->setFont(QFont("Arial", 10));
    m_ui->m_pluginsTable->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignLeft);
    m_ui->m_pluginsTable->horizontalHeaderItem(4)->setFont(QFont("Arial", 10));
    m_ui->m_pluginsTable->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignLeft);
    m_ui->m_pluginsTable->horizontalHeaderItem(5)->setFont(QFont("Arial", 10));
    m_ui->m_pluginsTable->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignLeft);

    m_ui->m_pluginsTable->setColumnWidth(0, m_state.datacols.name);
    m_ui->m_pluginsTable->setColumnWidth(1, m_state.datacols.version);
    m_ui->m_pluginsTable->setColumnWidth(2, m_state.datacols.manu);
    m_ui->m_pluginsTable->setColumnWidth(3, m_state.datacols.cver);
    m_ui->m_pluginsTable->setColumnWidth(4, m_state.datacols.sver);
    m_ui->m_pluginsTable->setColumnWidth(5, m_state.datacols.uuid);

    m_ui->m_pluginsTable->setIconSize({ 12, 12 });

    loadLocalData();
    populate();
}

CENTAUR_NAMESPACE::PluginsDialog::~PluginsDialog()
{
    delete m_ui;
}

void CENTAUR_NAMESPACE::PluginsDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("pluginsDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("listState");
    settings.setValue("c0", m_ui->m_pluginsTable->columnWidth(0));
    settings.setValue("c1", m_ui->m_pluginsTable->columnWidth(1));
    settings.setValue("c2", m_ui->m_pluginsTable->columnWidth(2));
    settings.setValue("c3", m_ui->m_pluginsTable->columnWidth(3));
    settings.setValue("c4", m_ui->m_pluginsTable->columnWidth(4));
    settings.setValue("c5", m_ui->m_pluginsTable->columnWidth(5));
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
    m_state.datacols.cver    = settings.value("c3", m_state.datacols.cver).toInt();
    m_state.datacols.sver    = settings.value("c4", m_state.datacols.sver).toInt();
    m_state.datacols.uuid    = settings.value("c5", m_state.datacols.uuid).toInt();
    settings.endGroup();
}

void CENTAUR_NAMESPACE::PluginsDialog::accept()
{
    saveInterfaceState();
    QDialog::accept();
}

void CENTAUR_NAMESPACE::PluginsDialog::populate()
{
    if (g_app == nullptr)
    {
        logError("PluginsDialog", "Failed to acquire the global application object");
        return;
    }

    // Since versions are controled view preprocessor, older versions will be invisible

    // clang-format off
    CENTAUR_WARN_PUSH
    CENTAUR_WARN_OFF(exit-time-destructors)
    // clang-format on
    static std::map<uint8_t, std::string_view> locals = {
        // Since Id's will never collide will just manage them this way
        { 0x0f, "0.3.0" },
        { 0xe3, "0.2.0" }
    };
    CENTAUR_WARN_POP

    auto insertItem = [&ui = m_ui](const QString &text, const int &row, const int &col) {
        auto item = new QTableWidgetItem(text);
        item->setFont(QFont("Arial", 10));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if (col == 0)
        {
            item->setIcon(QIcon(":/img/res/img/plugin.svg"));
        }
        ui->m_pluginsTable->setItem(row, col, item);
    };

    int rows = 0;
    m_ui->m_pluginsTable->setRowCount(static_cast<int>(g_app->m_pluginsData.size()));
    for (auto &pl : g_app->m_pluginsData)
    {
        insertItem(pl->getPluginName(), rows, 0);
        insertItem(pl->getPluginVersionString(), rows, 1);

        auto uuid            = pl->getPluginUUID();
        auto uuidString      = fmt::format("{:08x}-{:04x}-{:04x}-{:04x}-{:04x}{:08x}",
                 uuid.dev0.dev0_u,
                 uuid.dev1.dev1_s,
                 uuid.sp0.sp0_s,
                 uuid.sp1.sp1_s,
                 uuid.plg.plg0.plg0_s,
                 uuid.plg.plg1.plg1_u);

        QString manufacturer = m_localData[{ uuid.dev0.dev0_u, uuid.dev1.dev1_s }];
        if (manufacturer.isEmpty())
            manufacturer = "<unknown>";
        insertItem(manufacturer, rows, 2);

        insertItem(uuidString.c_str(), rows, 5);

        insertItem(locals[uuid.sp0.sp0_c[1]].data(), rows, 3);
        insertItem(locals[uuid.sp1.sp1_c[0]].data(), rows, 4);

        ++rows;
    }
}

void CENTAUR_NAMESPACE::PluginsDialog::loadLocalData()
{
    std::string fileName = fmt::format("{}/local/plid.json", g_globals->pluginsPath.toStdString());

    std::ifstream stream(fileName);
    if (!stream.is_open())
    {
        logError("PluginsDialog", "Could not locate plid.json");
        QMessageBox::critical(this,
            tr("Fatal error"),
            tr("Plugins local information file could not be located"));
        return;
    }

    rapidjson::IStreamWrapper isw(stream);
    rapidjson::Document jsonDoc;
    if (jsonDoc.ParseStream(isw).HasParseError())
    {
        QString str = QString(tr("%1 at %2")).arg(rapidjson::GetParseError_En(jsonDoc.GetParseError())).arg(jsonDoc.GetErrorOffset());
        logError("PluginsDialog", QString("File plid.json is ill-formed. %1").arg(str));
        QMessageBox::critical(this,
            tr("Fatal error"),
            str);
        return;
    }

    std::string schemaFileName = fmt::format("{}/local/plid.schema.json", g_globals->pluginsPath.toStdString());
    std::ifstream schemaStream(schemaFileName);
    if (!schemaStream.is_open())
    {
        logError("PluginsDialog", "Could not locate plid.schema.json");
        QMessageBox::critical(this,
            tr("Fatal error"),
            tr("Plugins local information structure file could not be located"));
        return;
    }

    rapidjson::IStreamWrapper iswSchema(schemaStream);
    rapidjson::Document jsonSchema;
    if (jsonSchema.ParseStream(iswSchema).HasParseError())
    {
        QString str = QString(tr("%1 at %2")).arg(rapidjson::GetParseError_En(jsonDoc.GetParseError())).arg(jsonDoc.GetErrorOffset());
        logError("PluginsDialog", QString("File plid.schema.json is ill-formed. %1").arg(str));
        QMessageBox::critical(this,
            tr("Fatal error"),
            str);
        return;
    }
    rapidjson::SchemaDocument sd(jsonSchema);

    rapidjson::SchemaValidator sv { sd };

    if (!jsonDoc.Accept(sv))
    {
        rapidjson::StringBuffer sb;
        sv.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        QString str = QString("Invalid schema %1.\nInvalid keyword: %2\n").arg(sb.GetString(), sv.GetInvalidSchemaKeyword());
        sb.Clear();
        sv.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        str += QString("Invalid document: %1\n").arg(sb.GetString());
        logError("PluginsDialog", QString("File plid.json does not comply the schema. %1").arg(str));
        QMessageBox::critical(this,
            tr("Fatal error"),
            str);
    }

    for (const auto &dc : jsonDoc.GetArray())
    {
        CENTAUR_NAMESPACE::plugin::PluginUUID uuid;
        uuid.dev0.dev0_c[0]                                 = static_cast<uint8_t>(dc["bytes"]["0"].GetUint());
        uuid.dev0.dev0_c[1]                                 = static_cast<uint8_t>(dc["bytes"]["1"].GetUint());
        uuid.dev0.dev0_c[2]                                 = static_cast<uint8_t>(dc["bytes"]["2"].GetUint());
        uuid.dev0.dev0_c[3]                                 = static_cast<uint8_t>(dc["bytes"]["3"].GetUint());
        uuid.dev1.dev1_c[0]                                 = static_cast<uint8_t>(dc["bytes"]["4"].GetUint());
        uuid.dev1.dev1_c[1]                                 = static_cast<uint8_t>(dc["bytes"]["5"].GetUint());
        m_localData[{ uuid.dev0.dev0_u, uuid.dev1.dev1_s }] = dc["name"].GetString();
    }
}
