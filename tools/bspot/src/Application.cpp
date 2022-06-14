/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "Application.hpp"
#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <protocol.hpp>

#ifdef P_APPLE
#include "AppleDirectory.h"
#endif /*P_APPLE*/

#define CATCH_API_EXCEPTION()                                                                                                      \
    switch (ex.type())                                                                                                             \
    {                                                                                                                              \
        case BINAPI_NAMESPACE::APIException::Type::request:                                                                        \
            {                                                                                                                      \
                auto request = ex.request();                                                                                       \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("REQUEST.\nCode: {}. URL: {}\n{}\n", std::get<0>(request), std::get<1>(request), std::get<2>(request)); \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::limits:                                                                         \
            fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                       \
            fmt::print("LIMITS.\n{}\n", std::get<0>(ex.limits()));                                                                 \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::http1:                                                                          \
            {                                                                                                                      \
                auto http1 = ex.http1();                                                                                           \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("HTTPa.\nCode: {}\nURL: {}\n{}\n", std::get<0>(http1), std::get<1>(http1), std::get<2>(http1));         \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::http2:                                                                          \
            {                                                                                                                      \
                auto http2 = ex.http2();                                                                                           \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("HTTPb.\nStatus: {}. Code: {}\nURL: {}.\n{}\n",                                                         \
                    std::get<0>(http2),                                                                                            \
                    std::get<1>(http2),                                                                                            \
                    std::get<2>(http2),                                                                                            \
                    std::get<4>(http2)                                                                                             \
                        .c_str());                                                                                                 \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::json:                                                                           \
            {                                                                                                                      \
                auto json = ex.json();                                                                                             \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("JSON.\nURL: {}\n{}\n", std::get<0>(json), std::get<1>(json));                                          \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::api:                                                                            \
            {                                                                                                                      \
                auto api1 = ex.api();                                                                                              \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("API.\nCode: {}\nURL: {}\n{}", std::get<0>(api1), std::get<1>(api1), std::get<2>(api1));                \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::schema:                                                                         \
            {                                                                                                                      \
                auto schema = ex.schema();                                                                                         \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("SCHEMA.\nURL: {}\n{}\n", std::get<0>(schema), std::get<1>(schema));                                    \
            }                                                                                                                      \
            break;                                                                                                                 \
    }

bspot::Application::Application()
{
    std::string applicationSupporFile;
    fmt::print("Retrieving keys file... ");

#if defined(P_APPLE)
    // Apple Specific
    applicationSupporFile = getApplicationSupportDirectory();
#elif defined(P_LINUX)
    // LINUX Specific
#elifdefined(P_WINDOWS)
    // Windows Specific
#endif
    applicationSupporFile += "/Centaur/bspot.config.json";

    if (!std::filesystem::exists(applicationSupporFile))
    {
        throw std::runtime_error("File with keys was not found");
    }

    if (!acquireKeys(applicationSupporFile))
    {
        throw std::runtime_error("Missing configuration data. Can not access account information.");
    }

    fmt::print(fmt::fg(fmt::color::green_yellow), "Ok\n");

    fmt::print("Acquiring first instance balances... ");
    if (onAcquireFirstBalances())
        fmt::print(fmt::fg(fmt::color::green_yellow), "Ok\n");
}

bspot::Application::~Application() = default;

auto bspot::Application::acquire(const messages &s) noexcept -> void
{
}

auto bspot::Application::onAcquireFirstBalances() noexcept -> bool
{
    try
    {
        m_spot    = std::make_unique<BINAPI_NAMESPACE::BinanceAPISpot>(&m_binanceKeys, &m_binanceLimits);
        auto data = m_spot->getAllCoinsInformation();

    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        CATCH_API_EXCEPTION()
        terminate();
        return false;
    }
    return true;
}
