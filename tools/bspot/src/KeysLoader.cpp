/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "Application.hpp"
#include <fmt/color.h>
#include <fmt/core.h>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

auto bspot::Application::acquireKeys(const std::string &file) noexcept -> bool
{
    std::ifstream stream(file);

    rapidjson::IStreamWrapper jsonStream(stream);
    rapidjson::Document doc;
    doc.ParseStream(jsonStream);

    if (doc.HasParseError())
    {
        fmt::print(fmt::fg(fmt::color::red), "ERROR: ");
        fmt::print("Configuration key file is ill-formed\n");
        return false;
    }

    if (doc.FindMember("api") != doc.MemberEnd())
        m_binanceKeys.apiKey = std::string { doc["api"].GetString(), doc["api"].GetStringLength() };
    else
    {
        fmt::print(fmt::fg(fmt::color::red), "ERROR: ");
        fmt::print("API Key is missing\n");
        return false;
    }

    if (doc.FindMember("secret") != doc.MemberEnd())
        m_binanceKeys.secretKey = std::string { doc["secret"].GetString(), doc["secret"].GetStringLength() };
    else
    {
        fmt::print(fmt::fg(fmt::color::red), "ERROR: ");
        fmt::print("Secret Key is missing\n");
        return false;
    }

    return true;
}
