/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_APPLICATION_HPP
#define CENTAUR_APPLICATION_HPP

#include "BinanceAPI.hpp"
#include "ClientSocket.hpp"
#include "Globals.hpp"
#include "Tools.hpp"
#include "ToolsThread.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

namespace bspot
{
    enum class TimeoutType
    {
        acceptConnection = 0,
    };

    template <typename T>
    struct CentaurConnectionMessageT
    {
        using message_type = T;
    };
    template <typename T>
    struct FailedToConnectT
    {
        using message_type = T;
    };
    template <typename T>
    struct ClosedConnectionT
    {
        using message_type = T;
    };
    template <typename T>
    struct TimeoutMessageT
    {
        using message_type = T;
    };

    using CentaurConnectionMessage = CentaurConnectionMessageT<void>;
    using CentaurConnectedMessage  = CentaurConnectionMessageT<bool>;
    using FailedToConnect          = FailedToConnectT<void>;
    using ClosedConnection         = ClosedConnectionT<void>;
    using TimeoutMessage           = TimeoutMessageT<void>;

    /// \brief The main application Object
    /// This is a message driven application
    class TraderApplication final : public cen::tools::Thread<CentaurConnectionMessage, CentaurConnectedMessage, FailedToConnect, ClosedConnection, TimeoutMessage>
    {
    protected:
        // clang-format off
#if defined(__clang__) || defined(__GNUC__)
CENTAUR_TOOL_WARN_PUSH
CENTAUR_TOOL_WARN_OFF(weak-vtables)
#endif /*__clang__*/
        // clang-format on
        /// \brief Attempt reconnection every minute if the connection fails or is disconnected
        struct Reconnection : public cen::tools::TimerEventMinutes<1>
        {
            inline explicit Reconnection(TraderApplication *app) :
                m_app { app } { }
            inline ~Reconnection() override = default;
            inline auto timerEvent() noexcept -> void override
            {
                // Send a reconnection message
                logInfo("The reconnection timer event was triggered");
                m_app->sendMessage<CentaurConnectionMessage>();
            }

        private:
            TraderApplication *m_app;
        };

        // Response timeout timer
        struct ResponseTimeout : public CENTAUR_NAMESPACE::tools::TimerEventSecs<30>
        {
            explicit inline ResponseTimeout(TraderApplication *app) :
                m_app { app } { }

            inline auto timerEvent() noexcept -> void override
            {
                m_app->sendMessage<TimeoutMessage>();
            }

        protected:
            TimeoutType m_type;
            TraderApplication *m_app;
        };

        // Response queue
        struct ResponseQueue
        {
            inline auto empty() -> bool
            {
                std::lock_guard<std::mutex> lock { m_dataProtect };
                return m_data.empty();
            }

            inline auto get() -> TimeoutType
            {
                std::lock_guard<std::mutex> lock { m_dataProtect };
                auto data = m_data.front();
                m_data.pop();
                return data;
            }

            inline auto push(TimeoutType t) -> void
            {
                std::lock_guard<std::mutex> lock { m_dataProtect };
                m_data.push(t);
            }

        protected:
            std::mutex m_dataProtect;
            std::queue<TimeoutType> m_data;
        };

        // clang-format off
#if defined(__clang__) || defined(__GNUC__) \
CENTAUR_TOOL_WARN_POP
#endif /*__clang__*/
        // clang-format on
    public:
        TraderApplication();
        ~TraderApplication() override;

    protected:
        auto acquire(const messages &s) noexcept -> void override;
        auto onTerminate() noexcept -> void override;

    public:
        auto addResponse(TimeoutType type) noexcept -> void;

    protected:
        auto onConnectCentaur() noexcept -> void;
        auto onReconnectCentaur() noexcept -> void;
        auto onTimeout() noexcept -> void;

    protected:
        auto onAcquireFirstBalances() noexcept -> bool;

    protected:
        auto acquireKeys(const std::string &file) noexcept -> bool;

    private:
        std::unique_ptr<BINAPI_NAMESPACE::BinanceAPISpot> m_spot;
        BINAPI_NAMESPACE::BinanceKeys m_binanceKeys;
        BINAPI_NAMESPACE::BinanceLimits m_binanceLimits;

    public:
        // True when a Protocol_AcceptedConnection is received
        std::atomic_bool m_centaurConnected { false };

    private:
        std::thread m_centaurThread;
        std::thread m_centaurReconnectThread;
        std::thread m_timeoutThread;
        std::thread m_loggerThread;
        std::unique_ptr<Reconnection> m_reconnection;
        std::unique_ptr<CentaurClient> m_centaurClient;
        std::unique_ptr<ResponseTimeout> m_timeout;

    private:
        ResponseQueue m_responseQueue;
    };
} // namespace bspot

#endif // CENTAUR_APPLICATION_HPP
