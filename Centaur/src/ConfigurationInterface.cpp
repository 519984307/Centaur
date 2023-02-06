/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 30/05/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ConfigurationInterface.hpp"
#include "Globals.hpp"

cen::PluginConfiguration::PluginConfiguration(const QString &uuidString)
{
    m_settingsFile = QString("%1/Settings/%2.json").arg(g_globals->paths.pluginsPath, uuidString);
    m_path         = QString("%1/Private/%2.pem").arg(g_globals->paths.pluginsPath, uuidString);
}

cen::PluginConfiguration::~PluginConfiguration() = default;

std::string cen::PluginConfiguration::getPluginPublicKeyPath()
{
    return m_path.toStdString();
}

auto cen::PluginConfiguration::getConfigurationFileName() noexcept -> std::string
{
    return m_settingsFile.toStdString();
}

auto cen::PluginConfiguration::getAssetImage(int size, CENTAUR_INTERFACE_NAMESPACE::AssetImageSource source, const QString &asset, QWidget *parent) -> QPixmap
{
    return findAssetImage(size, asset, source, parent);
}
