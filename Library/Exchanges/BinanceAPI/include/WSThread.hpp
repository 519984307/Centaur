////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#pragma once

#ifndef FUTURES_WSTHREAD_HPP
#define FUTURES_WSTHREAD_HPP


#include "BinanceAPIGlobal.hpp"

namespace BINAPI_NAMESPACE::ws
{
    class WSThread
    {
    public:
        WSThread(std::string url, std::string endPoint);
        virtual ~WSThread();

        auto run() -> void;

        /// \brief Stop receiving. The function is thread safe
        void terminate();

    public:
        /// \brief Called when the WebSocket is connected
        virtual auto connected() -> void = 0;
        /// \brief Called when the WebSocket gets closed
        virtual auto close() -> void = 0;
        /// \brief Called when the WebSocket failed to connect
        virtual auto connectionError() -> void = 0;
        /// \brief Called when the ping frame was sent successfully or unsuccessfully
        virtual void pingSent(const bool &success) noexcept = 0;

    protected:
        auto getJsonData() const -> const std::string &;

    public:
        auto isRunning() -> bool;
        auto isTerminated() -> bool;
        auto setPingTimer(const uint64_t &timer) -> void;

    protected:
        virtual auto receivedData() -> void = 0;
        auto sendData(const std::string &msg) -> void;
        auto randNumber() -> int;

    private:
        lws_context *m_context { nullptr };
        lws_protocols m_protocols[2] {};
        lws *m_lws { nullptr };

        std::string m_url;
        std::string m_endPoint;
        std::string m_jsonData;

        uint64_t m_pingTimer{0};

        std::atomic_bool m_running;
        std::atomic_bool m_terminate;

        static auto eventManager(struct lws *wsi, lws_callback_reasons reason, void *user, void *in, size_t len) -> int;
    };
} // namespace trader::api::ws

#endif // FUTURES_WSTHREAD_HPP
