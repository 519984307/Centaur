/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "TraderApplication.hpp"
#include <fmt/color.h>
#include <fmt/core.h>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

auto btrader::TraderApplication::acquireKeys(const std::string &file) noexcept -> bool
{
    std::ifstream stream(file);

    rapidjson::IStreamWrapper jsonStream(stream);
    rapidjson::Document doc;
    doc.ParseStream(jsonStream);

    if (doc.HasParseError())
    {
        logError("Configuration key file is ill-formed");
        return false;
    }

    if (doc.FindMember("api") != doc.MemberEnd())
        m_binanceKeys.apiKey = std::string { doc["api"].GetString(), doc["api"].GetStringLength() };
    else
    {
        logError("API Key is missing");
        return false;
    }

    if (doc.FindMember("secret") != doc.MemberEnd())
        m_binanceKeys.secretKey = std::string { doc["secret"].GetString(), doc["secret"].GetStringLength() };
    else
    {
        logError("Secret Key is missing");
        return false;
    }

    return true;
}
