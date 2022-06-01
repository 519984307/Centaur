/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 30/05/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ConfigurationInterface.hpp"
#include "Globals.hpp"

cen::PluginConfiguration::PluginConfiguration(const QString &uuidString)
{
    m_path = QString("%1/Private/%2.pem").arg(g_globals->paths.pluginsPath, uuidString);
}

cen::PluginConfiguration::~PluginConfiguration() = default;

std::string cen::PluginConfiguration::getPluginPublicKeyPath()
{
    return m_path.toStdString();
}

auto cen::PluginConfiguration::addValue(const std::string &key, const std::string &value) -> void
{
    m_data[key] = value;
}

std::string cen::PluginConfiguration::getValue(const std::string &key, bool *error)
{
    auto iter = m_data.find(key);
    if (iter == m_data.end())
    {
        *error = false;
        return "";
    }

    *error = true;
    return iter->second;
}

uint32_t cen::PluginConfiguration::getUint32(const std::string &key, bool *error)
{
    const auto str = getValue(key, error);
    if (!(*error))
        return 0;
    return static_cast<uint32_t>(std::stoul(str));
}

uint64_t cen::PluginConfiguration::getUint64(const std::string &key, bool *error)
{
    const auto str = getValue(key, error);
    if (!(*error))
        return 0;
    return std::stoull(str);
}

int32_t cen::PluginConfiguration::getInt32(const std::string &key, bool *error)
{
    const auto str = getValue(key, error);
    if (!(*error))
        return 0;
    return static_cast<int32_t>(std::stol(str));
}

int64_t cen::PluginConfiguration::getInt64(const std::string &key, bool *error)
{
    const auto str = getValue(key, error);
    if (!(*error))
        return 0;
    return std::stoll(str);
}

bool cen::PluginConfiguration::getBool(const std::string &key, bool *error)
{
    const auto str = getValue(key, error);
    if (!(*error))
        return 0;
    if (str == "false" || str == "FALSE" || str == "False")
        return false;
    else if (str == "true" || str == "TRUE" || str == "True")
        return true;
    else
    {
        *error = false;
        return false;
    }
}
