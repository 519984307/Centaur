/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_GLOBALS_HPP
#define CENTAUR_GLOBALS_HPP

#include <ToolsLogger.hpp>

namespace bspot::logger
{
    enum class LoggingEvent : uint32_t
    {
        info,
        warning,
        error,
        success,
        failure,
        timeout
    };

    enum class CoinEvent : uint32_t
    {

    };
} // namespace bspot::logger

extern cen::tools::ToolLogger<20> *g_futuresLogger;

template <typename msg, typename... Args>
static inline auto logCoin(const bspot::logger::CoinEvent &event, const uint32_t &coin, const msg &format, Args &&...args)
{
    g_futuresLogger->template sendMessage(cen::tools::LoggerEvent {
        .dateTime        = static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
        .eventId         = static_cast<uint32_t>(event),
        .coinId          = coin,
        .message         = fmt::vformat(format, fmt::make_args_checked<Args...>(format, args...)),
        .preventCoinFile = false });
}

template <typename msg, typename... Args>
static inline auto logEvent(const bspot::logger::LoggingEvent &event, const msg &format, Args &&...args)
{
    g_futuresLogger->template sendMessage(cen::tools::LoggerEvent {
        .dateTime        = static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
        .eventId         = static_cast<uint32_t>(event),
        .coinId          = 0,
        .message         = fmt::vformat(format, fmt::make_args_checked<Args...>(format, args...)),
        .preventCoinFile = true });
}

template <typename msg, typename... Args>
static inline auto logEventCoin(const bspot::logger::LoggingEvent &event, const uint32_t &coin, const msg &format, Args &&...args)
{
    g_futuresLogger->template sendMessage(cen::tools::LoggerEvent {
        .dateTime        = static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
        .eventId         = static_cast<uint32_t>(event),
        .coinId          = coin,
        .message         = fmt::vformat(format, fmt::make_args_checked<Args...>(format, args...)),
        .preventCoinFile = true });
}

template <typename msg, typename... Args>
static inline auto logInfo(const msg &format, Args &&...args)
{
    logEvent(bspot::logger::LoggingEvent::info, format, args...);
}

template <typename msg, typename... Args>
static inline auto logWarning(const msg &format, Args &&...args)
{
    logEvent(bspot::logger::LoggingEvent::warning, format, args...);
}

template <typename msg, typename... Args>
static inline auto logError(const msg &format, Args &&...args)
{
    logEvent(bspot::logger::LoggingEvent::error, format, args...);
}

template <typename msg, typename... Args>
static inline auto logSuccess(const msg &format, Args &&...args)
{
    logEvent(bspot::logger::LoggingEvent::success, format, args...);
}

template <typename msg, typename... Args>
static inline auto logFailure(const msg &format, Args &&...args)
{
    logEvent(bspot::logger::LoggingEvent::failure, format, args...);
}

template <typename msg, typename... Args>
static inline auto logTimeout(const msg &format, Args &&...args)
{
    logEvent(bspot::logger::LoggingEvent::timeout, format, args...);
}

template <typename msg, typename... Args>
static inline auto logInfo(const uint32_t &coin, const msg &format, Args &&...args)
{
    logEventCoin(bspot::logger::LoggingEvent::info, coin, format, args...);
}

template <typename msg, typename... Args>
static inline auto logWarning(const uint32_t &coin, const msg &format, Args &&...args)
{
    logEventCoin(bspot::logger::LoggingEvent::warning, coin, format, args...);
}

template <typename msg, typename... Args>
static inline auto logError(const uint32_t &coin, const msg &format, Args &&...args)
{
    logEventCoin(bspot::logger::LoggingEvent::error, coin, format, args...);
}

#endif // CENTAUR_GLOBALS_HPP
