/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CLIENTSOCKET_HPP
#define CENTAUR_CLIENTSOCKET_HPP

#include "Globals.hpp"
#include "Protocol.hpp"
#include "Tools.hpp"
#include "ToolsThread.hpp"
#include <libwebsockets.h>

namespace bspot
{
    class TraderApplication;
    class CentaurClient final
    {
    public:
        CentaurClient(TraderApplication *app, std::string address, std::string endpoint, int port);
        ~CentaurClient();

    protected:
        auto sendData(CENTAUR_PROTOCOL_NAMESPACE::Protocol *protocol) noexcept -> bool;

    protected:
        auto handleProtocolMessage(uint32_t type, uint32_t version, const std::string &message) noexcept -> void;

    public:
        auto run() -> void;
        auto terminate() -> void;
        auto isRunning() -> bool;
        auto isTerminated() -> bool;

    public:
        /// \brief Called when the WebSocket is connected
        auto connected() -> void;
        /// \brief Called when the WebSocket gets closed
        auto close() -> void;
        /// \brief Called when the WebSocket failed to connect
        auto connectionError() -> void;

    private:
        TraderApplication *m_app { nullptr };

    private:
        lws_context *m_context { nullptr };
        lws_protocols m_protocols[2] {};
        lws *m_lws { nullptr };
        static auto eventManager(struct lws *wsi, lws_callback_reasons reason, void *user, void *in, size_t len) -> int;

    private:
        std::string m_address;
        std::string m_endPoint;
        int m_port;

    private:
        std::atomic_bool m_running;
        std::atomic_bool m_terminate;

    protected:
        bool m_ignoreAllMessages { false };
        std::string m_connectionId;

    private:
        struct InternalData
        {
            inline ~InternalData()
            {
                release();
            }

            inline auto release() -> void
            {
                if (data != nullptr)
                {
                    free(data);
                    data = nullptr;
                }
                size = 0;
                data = nullptr;
            }

            inline auto alloc(std::size_t ns) -> void
            {
                auto newPtr = static_cast<uint8_t *>(realloc(data, ns));
                if (newPtr != data)
                {
                    logInfo("Protocol internal data address was moved.");
                    data = newPtr;
                }
                size = ns;
            }

            inline auto setData(const uint8_t *newData, std::size_t offset, std::size_t bytes) -> void
            {
                memcpy(data + offset, newData, bytes);
            }

            C_NODISCARD inline auto getSize() const -> std::size_t
            {
                return size;
            }

            C_NODISCARD inline auto get() const -> uint8_t *
            {
                return data;
            }

        protected:
            uint8_t *data;
            std::size_t size;

        } m_internalData;
    };
} // namespace bspot

#endif // CENTAUR_CLIENTSOCKET_HPP
