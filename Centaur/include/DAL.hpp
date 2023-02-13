/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_DAL_HPP
#define CENTAUR_DAL_HPP

#include <Centaur.hpp>
#include <QException>
#include <QWidget>

namespace CENTAUR_NAMESPACE::dal
{
    enum class OpenDatabaseCode
    {
        Ok,      // Database open
        Fatal,   // Failed to create the database
        Recreate // Recreate the database
    };

    struct PluginData
    {
        inline PluginData(QString nm, QString vr, QString mn, QString id, QString uiv, QString chk, QString dyn, bool en, bool pr) :
            name { std::move(nm) },
            version { std::move(vr) },
            man { std::move(mn) },
            uuid { std::move(id) },
            centaur_uuid { std::move(uiv) },
            checksum { std::move(chk) },
            dynamic { std::move(dyn) },
            enabled { en },
            protect { pr } { }

        QString name;
        QString version;
        QString man;
        QString uuid;
        QString centaur_uuid;
        QString checksum;
        QString dynamic;
        bool enabled;
        bool protect;
    };

    struct DataAccess final
    {
        /// \brief Open the database
        /// \param parent Parent Widget for QMessageBox internal calls
        /// \return Actions to take after the call
        static OpenDatabaseCode openDatabase(QWidget *parent = nullptr) noexcept;

        /// \brief Creates the database an all its queries
        static OpenDatabaseCode createDatabase(QWidget *parent = nullptr) noexcept;

        /// \brief Determinates if a plugin exists
        static std::optional<bool> pluginExists(const QString &uuid) noexcept;

        /// \brief Retrieve all plugin information
        static std::optional<QList<PluginData>> pluginInformation() noexcept;

        /// \brief Update enabling state
        static std::optional<bool> updateEnabledState(const QString &uuid, bool newState) noexcept;

        /// \brief Inserts a new plugin into the database. The function does not do any previous checks
        static std::optional<bool> insertPlugin(const QString &name,
            const QString &version,
            const QString &man,
            const QString &uuid,
            const QString &centaur_uuid,
            const QString &checksum,
            const QString &dynamic,
            bool enabled,
            bool protect) noexcept;

        static std::optional<QString> fromUUIDtoVersion(const QString &uuid) noexcept;
    };

} // namespace CENTAUR_NAMESPACE::dal

#endif // CENTAUR_DAL_HPP
