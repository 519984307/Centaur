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
        auto addValue(const std::string &key, const std::string &value) -> void;

    public:
        auto getPluginPublicKeyPath() -> std::string override;
        auto getValue(const std::string &key, bool *error) -> std::string override;
        auto getUint32(const std::string &key, bool *error) -> uint32_t override;
        auto getUint64(const std::string &key, bool *error) -> uint64_t override;
        auto getInt32(const std::string &key, bool *error) -> int32_t override;
        auto getInt64(const std::string &key, bool *error) -> int64_t override;
        auto getBool(const std::string &key, bool *error) -> bool override;

    public:
        auto getSymbolImage(int size, int format, const QString &asset, QPixmap *pm) -> bool override;

    private:
        QString m_path;
        std::unordered_map<std::string, std::string> m_data;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CONFIGURATIONINTERFACE_HPP
