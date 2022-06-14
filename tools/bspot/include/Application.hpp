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
#include "Tools.hpp"
#include "ToolsThread.hpp"

namespace bspot
{
    template <typename T>
    struct empty_message
    {
        using message_type = T;
    };
    using ReconnectionMessage = empty_message<void>;

    /// \brief The main application Object
    /// This is a message driven application
    class Application final : public cen::tools::Thread<ReconnectionMessage>
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
            inline explicit Reconnection(Application *app) :
                m_app { app } { }
            inline ~Reconnection() override = default;
            inline auto timerEvent() noexcept -> void override
            {
                // Send a reconnection message
                m_app->sendMessage<ReconnectionMessage>();
            }

        private:
            Application *m_app;
        };
        // clang-format off
#if defined(__clang__) || defined(__GNUC__) \
CENTAUR_TOOL_WARN_POP
#endif /*__clang__*/
        // clang-format on
    public:
        Application();
        ~Application() override;

    protected:
        auto acquire(const messages &s) noexcept -> void override;

    protected:
        auto onAcquireFirstBalances() noexcept -> bool;

    protected:
        auto acquireKeys(const std::string &file) noexcept -> bool;

    protected:
        Reconnection *m_reconnection { nullptr };

    private:
        std::unique_ptr<BINAPI_NAMESPACE::BinanceAPISpot> m_spot;
        BINAPI_NAMESPACE::BinanceKeys m_binanceKeys;
        BINAPI_NAMESPACE::BinanceLimits m_binanceLimits;
    };
} // namespace bspot

#endif // CENTAUR_APPLICATION_HPP
