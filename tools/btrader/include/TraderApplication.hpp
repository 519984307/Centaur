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

#include "Tools.hpp"
#include "ToolsThread.hpp"
#include "TraderGlobals.hpp"
#include "uuid.hpp"

#include "SpotStreams.hpp"

#include "ApplicationMessages.hpp"
#include "BinanceAPI.hpp"
#include "ClientSocket.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

namespace btrader
{
    /// \brief The main application Object
    /// This is a message driven application
    class TraderApplication final : public cen::tools::Thread<APPLICATION_MESSAGES>
    {
    private:
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

        // This generates the icons id
        struct UIIconIdGenerator
        {
            inline UIIconIdGenerator() :
                ipoIconId { CENTAUR_NAMESPACE::uuid::generate().to_string() },
                storageId { CENTAUR_NAMESPACE::uuid::generate().to_string() },
                withdrawId { CENTAUR_NAMESPACE::uuid::generate().to_string() },
                moneyId { CENTAUR_NAMESPACE::uuid::generate().to_string() },
                moneyOffId { CENTAUR_NAMESPACE::uuid::generate().to_string() },
                binanceId { CENTAUR_NAMESPACE::uuid::generate().to_string() }
            {
            }
            std::string ipoIconId;
            std::string storageId;
            std::string withdrawId;
            std::string moneyId;
            std::string moneyOffId;
            std::string binanceId;
        };

        template <typename T>
        struct ApplicationThread
        {
            using Tptr          = T *;
            ApplicationThread() = default;
            inline ~ApplicationThread() noexcept
            {
                release();
            }
            inline auto release()
            {
                if (object)
                {
                    object->terminate();
                    if (thisThread.joinable())
                        thisThread.join();
                    object.reset();
                }
            }

            // Start the thread releasing the memory and closing the thread if there was a prior thread running
            template <typename... Args>
            inline auto startAndRelease(Args &&...args)
            {
                release();
                object = std::make_unique<T>(std::forward<Args>(args)...);
                if (object)
                {
                    thisThread = std::thread([&] {
                        object->run();
                    });
                }
            }

            template <typename... Args>
            inline auto startAndReleaseWithPromise(Args &&...args)
            {
                release();
                object = std::make_unique<T>(std::forward<Args>(args)...);
                if (object)
                {
                    std::future<void> started = object->getStartedPromise().get_future();
                    thisThread                = std::thread([&] {
                        object->run();
                    });
                    started.wait();
                }
            }

            inline auto isValid() const noexcept -> bool
            {
                return object.operator bool();
            }

            inline auto thisObject() noexcept -> Tptr
            {
                return object.get();
            }

        private:
            std::thread thisThread;
            std::unique_ptr<T> object;
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

    public:
        auto onSetCentaurConnection() noexcept -> void;
        auto onAcquireFirstBalances() noexcept -> bool;

    protected:
        auto acquireKeys(const std::string &file) noexcept -> bool;
        /// \brief This functions starts and pushes into a Binance WebSocket stream the acquisition of the 24hr miniticker
        /// in order, to have balances referenced against USDT.
        /// If the asset passed is USDT and a connection to centaur server was made, this sends a Protocol_UpdateAssetTotal directly to update the total
        /// \param asset Asset name
        auto updateBalanceTicker(const std::string &asset) noexcept -> void;
        auto updateTotalAsset(const std::string &asset, double value) noexcept -> void;

    private:
        std::unique_ptr<BINAPI_NAMESPACE::BinanceAPISpot> m_spot;
        BINAPI_NAMESPACE::BinanceKeys m_binanceKeys;
        BINAPI_NAMESPACE::BinanceLimits m_binanceLimits;

    public:
        // True when a Protocol_AcceptedConnection is received

    private:
        local::AssetBalances m_assetBalances;
        std::thread m_loggerThread;

        ApplicationThread<btrader::stream::SpotStreams> m_tickerStreams;
        struct CentaurServer
        {
            ApplicationThread<ResponseTimeout> m_timeout;
            ApplicationThread<Reconnection> m_reconnection;
            ApplicationThread<CentaurClient> m_centaurClient;
            UIIconIdGenerator m_centaurIcons;
            ResponseQueue m_responseQueue;
            std::unordered_map<std::string, std::string> m_assetIds;
            std::atomic_bool m_centaurConnected { false };
        } m_centaurServer;
    };
} // namespace btrader

#endif // CENTAUR_APPLICATION_HPP
