/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "DAL.hpp"

#include "Globals.hpp"
#include "Logger.hpp"

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>

CENTAUR_NAMESPACE::dal::OpenDatabaseCode CENTAUR_NAMESPACE::dal::DataAccess::openDatabase(QWidget *parent) noexcept
{
    const auto path   = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/local";
    const auto dbFile = path + "/centaur.db";

    if (!QFileInfo::exists(dbFile))
    {
        auto response = QMessageBox::critical(parent, QObject::tr("Error"),
            QObject::tr("The Centaur database could not be opened. Would you like to repair it? (all previous information will be lost)"),
            QMessageBox::Yes, QMessageBox::No);

        if (response == QMessageBox::No)
        {
            return OpenDatabaseCode::Fatal;
        }
        else
        {
            return OpenDatabaseCode::Recreate;
        }
    }
    else
    {
        auto db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbFile);

        if (!db.open())
        {
            logFatal("app", QObject::tr("The Centaur database could not be opened"));
            return OpenDatabaseCode::Fatal;
        }
    }

    return OpenDatabaseCode::Ok;
}

CENTAUR_NAMESPACE::dal::OpenDatabaseCode CENTAUR_NAMESPACE::dal::DataAccess::createDatabase(QWidget *parent) noexcept
{
    QDir dir;

    const auto path   = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/local";
    const auto dbFile = path + "/centaur.db";

    if (!dir.exists(path))
    {
        if (dir.mkpath(path))
            logInfo("database", "Database directory was created");
        else
        {
            logFatal("app", "The Centaur database path could not be created");

            QMessageBox::critical(parent, QObject::tr("Error"),
                QObject::tr("Data path could not be created"));

            return OpenDatabaseCode::Fatal;
        }
    }

    // Restore
    for (const auto &c : QSqlDatabase::connectionNames())
        QSqlDatabase::removeDatabase(c);

    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFile);
    if (!db.open())
    {
        logFatal("app", "The Centaur database could not be opened");
        QMessageBox::critical(parent, QObject::tr("Error"),
            QObject::tr("The Centaur database could not be opened. Reinstalling the application could solve the problem"));

        return OpenDatabaseCode::Fatal;
    }

    const QString repairFile = g_globals->paths.resPath + "/Repair/cdb.sql";

    QFile repairSQLFile(repairFile);

    if (repairSQLFile.open(QFile::ReadOnly))
    {
        QTextStream stream(&repairSQLFile);
        QString sqlQuery = stream.readAll();
        repairSQLFile.close();

        // Qt SQL does not support multiple statements,
        // so the repair file contain '--!!!!' as a mark to locate all statements
        QStringList statements = sqlQuery.split("--!!!!", Qt::SkipEmptyParts);

        for (const auto &statement : statements)
        {
            QSqlQuery query;
            if (!query.exec(statement))
            {

                qDebug() << query.lastError().text();

                logFatal("app", "The Centaur database creation query failed");
                QMessageBox::critical(parent, QObject::tr("Error"),
                    QObject::tr("The Centaur database could not be created. Reinstalling the application could solve the problem"));
                return OpenDatabaseCode::Fatal;
            }
        }
    }
    else
    {
        logFatal("app", "The Centaur database repair file could not be opened");
        QMessageBox::critical(parent, QObject::tr("Error"),
            QObject::tr("The Centaur database repair file could not be opened. Reinstalling the application could solve the problem"));
        return OpenDatabaseCode::Fatal;
    }

    return OpenDatabaseCode::Ok;
}

std::optional<bool> CENTAUR_NAMESPACE::dal::DataAccess::pluginExists(const QString &uuid) noexcept
{
    QSqlQuery q;

    q.prepare("SELECT COUNT(*) FROM plugins WHERE uuid = :uuid;");
    q.bindValue(":uuid", uuid);

    if (!q.exec())
        return std::nullopt;

    return q.size() > 0;
}

std::optional<CENTAUR_NAMESPACE::dal::PluginData> CENTAUR_NAMESPACE::dal::DataAccess::pluginInformation(const QString &uuid) noexcept
{
    QSqlQuery q;

    q.prepare("SELECT name,version,manufacturer,centaur_uuid,enabled,checksum,dynamic,protected FROM plugins WHERE uuid = :uuid;");
    q.bindValue(":uuid", uuid);

    if (!q.exec())
    {
        QSqlError e = q.lastError();
        QString er  = QString("%1").arg(e.text());
        qDebug() << er;
        logError("dbInsertPlugin", er);
        return std::nullopt;
    }

    if(!q.size())
        return {};

    q.next();
    return PluginData {
        q.value(q.record().indexOf("name")).toString(),
        q.value(q.record().indexOf("version")).toString(),
        q.value(q.record().indexOf("manufacturer")).toString(),
        uuid,
        q.value(q.record().indexOf("centaur_uuid")).toString(),
        q.value(q.record().indexOf("checksum")).toString(),
        q.value(q.record().indexOf("dynamic")).toString(),
        q.value(q.record().indexOf("enabled")).toBool(),
        q.value(q.record().indexOf("protected")).toBool(),
    };
}

std::optional<QList<CENTAUR_NAMESPACE::dal::PluginData>> CENTAUR_NAMESPACE::dal::DataAccess::pluginInformation() noexcept
{
    QSqlQuery q;

    const QString query { R"(SELECT name, version, manufacturer, p.uuid, v.version_string AS vs, enabled, protected, checksum, dynamic
FROM plugins p
JOIN ui_versions v ON p.centaur_uuid = v.uuid;)" };

    if (!q.exec(query))
    {
        logError("DAL", "plugin information not acquired");
        return std::nullopt;
    }

    QList<PluginData> data;

    while (q.next())
    {
        data.emplace_back(
            q.value(q.record().indexOf("name")).toString(),
            q.value(q.record().indexOf("version")).toString(),
            q.value(q.record().indexOf("manufacturer")).toString(),
            q.value(q.record().indexOf("uuid")).toString(),
            q.value(q.record().indexOf("vs")).toString(),
            q.value(q.record().indexOf("checksum")).toString(),
            q.value(q.record().indexOf("dynamic")).toString(),
            q.value(q.record().indexOf("enabled")).toBool(),
            q.value(q.record().indexOf("protected")).toBool());
    }

    logInfo("DAL", "plugin information acquired");
    return data;
}

std::optional<bool> CENTAUR_NAMESPACE::dal::DataAccess::updateEnabledState(const QString &uuid, bool newState) noexcept
{
    QSqlQuery q;
    q.prepare("UPDATE plugins SET enabled = :stat WHERE uuid = :uuid;");

    q.bindValue(":stat", newState ? 1 : 0);
    q.bindValue(":uuid", uuid);

    if (!q.exec())
        return std::nullopt;

    return q.numRowsAffected() > 0;
}

std::optional<bool> CENTAUR_NAMESPACE::dal::DataAccess::insertPlugin(const QString &name,
    const QString &version,
    const QString &man,
    const QString &uuid,
    const QString &centaur_uuid,
    const QString &checksum,
    const QString &dynamic,
    bool enabled,
    bool protect) noexcept
{
    QSqlQuery q;

    q.prepare(R"(INSERT INTO plugins(uuid, name, version, manufacturer, centaur_uuid, enabled, protected, checksum, dynamic) VALUES (:uuid, :name, :version, :man, :ui, :enb, :prt, :check, :dyn);)");

    q.bindValue(":uuid", uuid);
    q.bindValue(":name", name);
    q.bindValue(":version", version);
    q.bindValue(":man", man);
    q.bindValue(":ui", centaur_uuid);
    q.bindValue(":enb", enabled ? 1 : 0);
    q.bindValue(":prt", protect ? 1 : 0);
    q.bindValue(":check", checksum);
    q.bindValue(":dyn", dynamic);

    if (!q.exec())
    {
        QSqlError e = q.lastError();
        QString er  = QString("%1").arg(e.text());
        qDebug() << er;
        logError("dbInsertPlugin", er);
        return std::nullopt;
    }

    return q.numRowsAffected() > 0;
}

std::optional<QString> CENTAUR_NAMESPACE::dal::DataAccess::getDynamicFieldPlugin(const QString &uuid) noexcept
{
    QSqlQuery q;

    q.prepare("SELECT dynamic AS dyn FROM plugins WHERE uuid = :uuid;");

    q.bindValue(":uuid", uuid);
    if (!q.exec())
    {
        QSqlError e = q.lastError();
        QString er  = QString("%1").arg(e.text());
        qDebug() << er;
        logError("dbSelectDynamicPlugin", er);
        return std::nullopt;
    }

    q.next();
    return q.value(q.record().indexOf("dyn")).toString();
}

std::optional<bool> CENTAUR_NAMESPACE::dal::DataAccess::removePlugin(const QString &uuid) noexcept
{
    QSqlQuery q;

    q.prepare("DELETE FROM plugins WHERE uuid = :uuid;");

    q.bindValue(":uuid", uuid);

    if (!q.exec())
    {
        QSqlError e = q.lastError();
        QString er  = QString("%1").arg(e.text());
        qDebug() << er;
        logError("dbRemovePlugin", er);
        return std::nullopt;
    }

    return q.numRowsAffected() > 0;
}

std::optional<QString> CENTAUR_NAMESPACE::dal::DataAccess::fromUUIDtoVersion(const QString &uuid) noexcept
{
    QSqlQuery q;

    q.prepare(R"(SELECT version_string FROM ui_versions WHERE uuid=:uuid;)");
    q.bindValue(":uuid", uuid);

    if (!q.exec())
    {
        QSqlError e = q.lastError();
        QString er  = QString("%1").arg(e.text());
        qDebug() << er;
        logError("dbInsertPlugin", er);
        return std::nullopt;
    }

    q.next();
    return q.value(q.record().indexOf("version_string")).toString();
}
