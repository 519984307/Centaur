/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "TraderApplication.hpp"

#include <filesystem>

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

cen::tools::ToolLogger<20> *g_futuresLogger = nullptr;

bspot::TraderApplication::TraderApplication()
{
    /// Init logger basics
    g_futuresLogger = new cen::tools::ToolLogger<20>("/Volumes/RicardoESSD/Projects/Centaur/tools/local/log");

    g_futuresLogger->addEventName(static_cast<uint32_t>(bspot::logger::LoggingEvent::warning), { "warning", fmt::color::yellow });
    g_futuresLogger->addEventName(static_cast<uint32_t>(bspot::logger::LoggingEvent::info), { "info", fmt::color::dodger_blue });
    g_futuresLogger->addEventName(static_cast<uint32_t>(bspot::logger::LoggingEvent::error), { "error", fmt::color::red });
    g_futuresLogger->addEventName(static_cast<uint32_t>(bspot::logger::LoggingEvent::success), { "success", fmt::color::green_yellow });
    g_futuresLogger->addEventName(static_cast<uint32_t>(bspot::logger::LoggingEvent::failure), { "failed", fmt::color::orange_red });
    g_futuresLogger->addEventName(static_cast<uint32_t>(bspot::logger::LoggingEvent::timeout), { "timeout", fmt::color::medium_purple });

    m_loggerThread = std::thread([] { g_futuresLogger->run(); });

    std::string applicationSupportFile;
    logInfo("Retrieving keys file... ");

#if defined(P_APPLE)
    // Apple Specific
    applicationSupportFile = getApplicationSupportDirectory();
#elif defined(P_LINUX)
    // LINUX Specific
#elifdefined(P_WINDOWS)
    // Windows Specific
#endif
    applicationSupportFile += "/Centaur/bspot.config.json";

    if (!std::filesystem::exists(applicationSupportFile))
    {
        logError("File with keys was not found");
        throw std::runtime_error("File with keys was not found");
    }

    if (!acquireKeys(applicationSupportFile))
    {
        logFailure("Keys acquisition failed");
        throw std::runtime_error("Missing configuration data. Can not access account information.");
    }

    logSuccess("Keys acquire successfully");

    /*
    fmt::print("Acquiring first instance balances... ");
    if (onAcquireFirstBalances())
        fmt::print(fmt::fg(fmt::color::green_yellow), "Ok\n");*/

    localSend<CentaurConnectionMessage>();
}

bspot::TraderApplication::~TraderApplication()
{
    // Release memory
    delete g_futuresLogger;
}

auto bspot::TraderApplication::acquire(const messages &msg) noexcept -> void
{
    if (getMessage<CentaurConnectionMessage>(msg) != std::nullopt)
    {
        onConnectCentaur();
    }
    else if (getMessage<CentaurConnectedMessage>(msg) != std::nullopt)
    {
        if (m_reconnection)
        {
            m_reconnection->terminate();
            logInfo("The reconnection timer is stopped");
        }
    }
    else if (getMessage<FailedToConnect>(msg) != std::nullopt)
    {
        logInfo("The client will attempt to connect every minute");
        onReconnectCentaur();
    }
    else if (getMessage<ClosedConnection>(msg) != std::nullopt)
    {
        logInfo("The client will attempt to connect every minute");
        onReconnectCentaur();
    }

    else if (getMessage<TimeoutMessage>(msg) != std::nullopt)
    {
        onTimeout();
    }
}

auto bspot::TraderApplication::onConnectCentaur() noexcept -> void
{
    //////////////////////////////////////////////////////////////////////////////////
    // the timeout thread
    if (m_timeout)
    {
        m_timeout->terminate();
        if (m_timeoutThread.joinable())
            m_timeoutThread.join();
        m_timeout.reset();
    }

    // The User Websocket has higher priority over the Market threads
    m_timeout = std::make_unique<ResponseTimeout>(this);
    if (m_timeout)
    {
        m_timeoutThread = std::thread([&] {
            m_timeout->run();
        });
    }

    //////////////////////////////////////////////////////////////////////////////////
    if (m_centaurClient)
    {
        m_centaurClient->terminate();
        if (m_centaurThread.joinable())
            m_centaurThread.join();
        m_centaurClient.reset();
    }

    // The User Websocket has higher priority over the Market threads
    m_centaurClient = std::make_unique<CentaurClient>(this, "localhost", "", 80);
    if (m_centaurClient)
    {
        m_centaurThread = std::thread([&] {
            m_centaurClient->run();
        });
    }
}

auto bspot::TraderApplication::onReconnectCentaur() noexcept -> void
{
    if (m_timeout)
    {
        m_timeout->terminate();
        if (m_timeoutThread.joinable())
            m_timeoutThread.join();
        m_timeout.reset();
    }

    if (m_reconnection)
    {
        m_reconnection->terminate();
        if (m_centaurReconnectThread.joinable())
            m_centaurReconnectThread.join();
        m_reconnection.reset();
    }

    m_reconnection = std::make_unique<Reconnection>(this);
    if (m_reconnection)
    {
        std::future<void> started = m_reconnection->getStartedPromise().get_future();
        m_centaurReconnectThread  = std::thread([&] {
            m_reconnection->run();
        });
        started.wait();
        logSuccess("The reconnection timer is set");
    }
}

auto bspot::TraderApplication::onTerminate() noexcept -> void
{
    if (m_reconnection)
    {
        m_reconnection->terminate();
        if (m_centaurReconnectThread.joinable())
            m_centaurReconnectThread.join();
        m_reconnection.reset();
    }

    // Terminate Centaur WebSocket Client
    m_centaurClient->terminate();
    if (m_centaurThread.joinable())
        m_centaurThread.join();
    m_centaurClient.reset();

    // Terminate logger
    g_futuresLogger->terminate();
    if (m_loggerThread.joinable())
        m_loggerThread.join();
}

auto bspot::TraderApplication::onAcquireFirstBalances() noexcept -> bool
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

auto bspot::TraderApplication::addResponse(bspot::TimeoutType type) noexcept -> void
{
    m_responseQueue.push(type);
}

auto bspot::TraderApplication::onTimeout() noexcept -> void
{
    if (m_responseQueue.empty())
        return;

    logInfo("A timeout message was received");

    auto data = m_responseQueue.get();

    switch (data)
    {
        case TimeoutType::acceptConnection:
            {
                if (!m_centaurConnected)
                {
                    logTimeout("Clien was not connected to the Centaur server. The connection will be stopped and reattempted");
                    m_centaurClient->terminate();
                }
            }
            break;
    }
}
