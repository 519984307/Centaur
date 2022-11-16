/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 30/05/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CONFIGURATIONINTERFACE_HPP
#define CENTAUR_CONFIGURATIONINTERFACE_HPP

#include "Centaur.hpp"
#include "CentaurPlugin.hpp"

#include <unordered_map>

namespace CENTAUR_NAMESPACE
{
    class PluginConfiguration : public CENTAUR_NAMESPACE::interface::IConfiguration
    {
    public:
        explicit PluginConfiguration(const QString &uuidString);
        ~PluginConfiguration() override;


    public:
        auto getPluginPublicKeyPath() -> std::string override;
        auto getConfigurationFileName() noexcept  -> std::string override;

    public:
        auto getSymbolImage(int size, int format, const QString &asset, QPixmap *pm) -> bool override;

    private:
        QString m_path;
        QString m_settingsFile;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CONFIGURATIONINTERFACE_HPP
