////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "WSThread.hpp"
#include <random>

#include <fmt/color.h>
#include <fmt/core.h>

BINAPI_NAMESPACE::ws::WSThread::WSThread(std::string url, std::string endPoint) :
    m_url { std::move(url) },
    m_endPoint { std::move(endPoint) }
{
    m_jsonData.reserve(1024 * 1024);

    // Init protocol
    m_protocols[0] = { "wsProtocol", WSThread::eventManager, 0, 65536, 0, nullptr, 0 };
    m_protocols[1] = { nullptr, nullptr, 0, 0, 0, nullptr, 0 };
    // lws_set_log_level(LLL_THREAD | LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO, nullptr);
    lws_set_log_level(0, nullptr);
}
BINAPI_NAMESPACE::ws::WSThread::~WSThread() = default;

auto BINAPI_NAMESPACE::ws::WSThread::sendData(const std::string &msg) -> void
{
    char *str = new char[msg.size() + LWS_PRE + 1];
    memset(str, 0, msg.size() + LWS_PRE);
    strcpy(str + LWS_PRE, msg.c_str());
    lws_write(m_lws, reinterpret_cast<unsigned char *>(str) + LWS_PRE, msg.size(), LWS_WRITE_TEXT);
    // All previous calls are C functions and do not throw, so we can be assured that delete is going to be called
    delete[] str;
}

auto BINAPI_NAMESPACE::ws::WSThread::randNumber() -> int
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1'147'483'648);
    return distrib(gen);
}

auto BINAPI_NAMESPACE::ws::WSThread::isRunning() -> bool
{
    return m_running.load();
}

auto BINAPI_NAMESPACE::ws::WSThread::isTerminated() -> bool
{
    return m_terminate.load();
}

auto BINAPI_NAMESPACE::ws::WSThread::setPingTimer(const uint64_t &timer) -> void
{
    m_pingTimer = timer;
}

void BINAPI_NAMESPACE::ws::WSThread::terminate()
{
    m_terminate = true;
    lws_cancel_service(m_context);
}

auto BINAPI_NAMESPACE::ws::WSThread::getJsonData() const -> const std::string &
{
    return m_jsonData;
}

auto BINAPI_NAMESPACE::ws::WSThread::run() -> void
{
    if (m_running)
        throw(std::runtime_error("WebSocket already running"));

    m_terminate = false;

    if (m_context != nullptr)
        lws_context_destroy(m_context);

    // Init Context
    lws_context_creation_info info {} /*C++ Initialization just to get rid of clang-tidy and clang warnings of uninitialized variables*/;
    memset(&info, 0, sizeof(info)); // Proper zero initialization of a C struct

    info.port      = CONTEXT_PORT_NO_LISTEN;
    info.protocols = m_protocols;
    info.gid       = static_cast<gid_t>(0);
    info.uid       = static_cast<uid_t>(0);
    info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    m_context = lws_create_context(&info);

    lws_client_connect_info ccinfo {};
    memset(&ccinfo, 0, sizeof(lws_client_connect_info));
    ccinfo.context        = m_context;
    ccinfo.address        = m_url.c_str();
    ccinfo.port           = 443;
    ccinfo.path           = m_endPoint.c_str();
    ccinfo.host           = lws_canonical_hostname(m_context);
    ccinfo.origin         = "origin";
    ccinfo.protocol       = m_protocols[0].name;
    ccinfo.userdata       = reinterpret_cast<void *>(this);
    ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

    m_lws                 = lws_client_connect_via_info(&ccinfo);

    if (m_lws == nullptr)
        throw std::runtime_error("failed to connect");

    while (!m_terminate)
    {
        lws_service(m_context, 0 /*As documentation denotes it, this value is deprecated and must be zero*/);
    }
    lws_context_destroy(m_context);
    m_context = nullptr;
    m_running = false;
}

auto BINAPI_NAMESPACE::ws::WSThread::eventManager(struct lws *wsi, lws_callback_reasons reason, void *user, void *in, size_t len) -> int
{
    auto client = reinterpret_cast<BINAPI_NAMESPACE::ws::WSThread *>(user);

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif /*__clang__*/
    switch (reason)
    {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lws_callback_on_writable(wsi);
            if (client)
            {
                if (client->m_pingTimer > 0)
                    lws_set_timer_usecs(wsi, static_cast<lws_usec_t>(client->m_pingTimer) * LWS_USEC_PER_SEC);

                client->m_running = true;
                client->connected();
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            {
                std::string msg = std::string(reinterpret_cast<char *>(in), len);

                if (client)
                {
                    if (lws_frame_is_binary(wsi))
                    {
                        fmt::print("LWS received binary data and was ignored.\n");
                    }
                    else
                    {
                        client->m_jsonData = std::string { reinterpret_cast<char *>(in), len };
                        client->receivedData();
                    }
                }
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            break;

        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_CLIENT_CLOSED:
            if (client)
            {
                // Set proper flags to allow reconnection
                client->m_running   = false;
                client->m_terminate = true;

                client->close();
            }
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            if (client)
            {
                client->m_running   = false;
                client->m_terminate = true;
                client->connectionError();
            }
            break;

        case LWS_CALLBACK_TIMER:
            if (client && client->m_pingTimer)
            {
                uint8_t ping[LWS_PRE + 125];
                int m = lws_write(wsi, ping + LWS_PRE, 0, LWS_WRITE_PING);
                if (m < 0)
                    client->pingSent(false);
                else
                    client->pingSent(true);

                lws_set_timer_usecs(wsi, static_cast<lws_usec_t>(client->m_pingTimer * LWS_USEC_PER_SEC));
            }
            break;

        default:
            break;
    }

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

    return 0;
}
