/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ClientSocket.hpp"
#include "TraderApplication.hpp"
#include <ProtocolType.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <uuid.hpp>

#define SET_BASICS(msg)                \
    msg.uuid()       = m_connectionId; \
    msg.responseId() = cen::uuid::generate<std::mt19937_64>().to_string();

namespace
{
    /// User version: 1.0
    constexpr uint32_t g_userVersion = 10;
} // namespace

btrader::CentaurClient::CentaurClient(TraderApplication *app, std::string address, std::string endpoint, int port) :
    m_app { app },
    m_address { std::move(address) },
    m_endPoint { std::move(endpoint) },
    m_port { port }
{
    // Init protocol
    m_protocols[0] = { "wsProtocol", CentaurClient::eventManager, 0, 65535, 0, nullptr, 0 };
    m_protocols[1] = { nullptr, nullptr, 0, 0, 0, nullptr, 0 };
    // lws_set_log_level(LLL_THREAD | LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO, nullptr);
    lws_set_log_level(0, nullptr);
}

btrader::CentaurClient::~CentaurClient() = default;

auto btrader::CentaurClient::isRunning() -> bool
{
    return m_running.load();
}

auto btrader::CentaurClient::isTerminated() -> bool
{
    return m_terminate.load();
}

auto btrader::CentaurClient::run() -> void
{
    if (m_running)
        throw(std::runtime_error("WebSocket already running"));

    m_terminate = false;

    if (m_context != nullptr)
        lws_context_destroy(m_context);

    // Init Context
    lws_context_creation_info info {};                   /*C++ Initialization just to get rid of clang-tidy and clang warnings of uninitialized variables*/
    memset(&info, 0, sizeof(lws_context_creation_info)); // Proper zero initialization of a C struct

    info.options   = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port      = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = m_protocols;
    // info.fd_limit_per_thread = 1 + 1 + 1;

    m_context = lws_create_context(&info);

    lws_client_connect_info ccinfo {};
    memset(&ccinfo, 0, sizeof(lws_client_connect_info));
    ccinfo.context        = m_context;
    ccinfo.address        = m_address.c_str();
    ccinfo.port           = m_port;
    ccinfo.path           = m_endPoint.c_str();
    ccinfo.host           = lws_canonical_hostname(m_context);
    ccinfo.origin         = "origin";
    ccinfo.protocol       = m_protocols[0].name;
    ccinfo.userdata       = reinterpret_cast<void *>(this);
    ccinfo.ssl_connection = 0; /*LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK; */

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

auto btrader::CentaurClient::terminate() -> void
{
    m_terminate = true;
    if (m_context)
        lws_cancel_service(m_context);
}

auto btrader::CentaurClient::eventManager(struct lws *wsi, lws_callback_reasons reason, void *user, void *in, size_t len) -> int
{
    auto client = reinterpret_cast<btrader::CentaurClient *>(user);

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
                client->m_running = true;
                client->connected();
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            {
                if (client)
                {
                    if (lws_frame_is_binary(wsi) && !client->m_ignoreAllMessages)
                    {
                        CENTAUR_PROTOCOL_NAMESPACE::ProtocolHeader header {};

                        auto data           = CENTAUR_PROTOCOL_NAMESPACE::Generator::getData(&header, reinterpret_cast<uint8_t *>(in), len, 6000);

                        bool messageCorrect = true;
                        if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongSize)
                        {
                            messageCorrect = false;
                            logError("Data size received is not consistent with a Centaur Communication Protocol");
                        }

                        if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongMagic)
                        {
                            messageCorrect = false;
                            logError("The message received is not consistent with a Centaur Communication Protocol");
                        }

                        if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongHash)
                        {
                            messageCorrect = false;
                            logError("Centaur Communication Protocol. Data is corrupted");
                        }

                        if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongMessageSize)
                        {
                            messageCorrect = false;
                            logError("Centaur Communication Protocol. Data received is incomplete");
                        }

                        if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongTimestamp)
                        {
                            messageCorrect = false;
                            logError("Centaur Communication Protocol. Data received is out of time");
                        }

                        if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongVersion)
                        {
                            messageCorrect = false;
                            logError("Centaur Communication Protocol. Library Version and Message Version are incompatible");
                        }

                        if (messageCorrect)
                        {
                            client->handleProtocolMessage(header.type, header.userVersion, data);
                        }
                    }
                    else
                    {
                        logWarning("LWS received text data and was ignored.\n");
                    }
                }
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            break;

        case LWS_CALLBACK_CLOSED:
            C_FALLTHROUGH;
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
            C_FALLTHROUGH;
        default:
            break;
    }

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

    return 0;
}

auto btrader::CentaurClient::connected() -> void
{
    logInfo("A connection to the Centaur server was made.");
    m_app->sendMessage<CentaurConnectedMessage>();
}

auto btrader::CentaurClient::close() -> void
{
    logWarning("The server connection was lost");
    // Reconnection Message
    m_app->sendMessage<ClosedConnection>();
}

auto btrader::CentaurClient::connectionError() -> void
{
    logError("Failed to connect to the Centaur Server");
    // Reconnection Message
    m_app->sendMessage<FailedToConnect>();
}

auto btrader::CentaurClient::sendData(cen::protocol::Protocol *protocol, bool log) noexcept -> bool
{
    if (!protocol->isValid())
    {
        logError("Protocol is not valid or is incomplete.");
        return false;
    }

    const auto size = protocol->getSize() + LWS_PRE /*Include the padding*/;
    // Resize if the buffer it's too small or if the buffer is twice as big as the data to be sent
    if (m_internalData.getSize() < size || m_internalData.getSize() > size * 2)
    {
        logInfo("Protocol internal buffer will be resized");
        m_internalData.alloc(size);
    }

    m_internalData.setData(protocol->get(), LWS_PRE, protocol->getSize());
    int i = lws_write(m_lws, m_internalData.get() + LWS_PRE, protocol->getSize(), LWS_WRITE_BINARY);

    if (i < 0)
    {
        logFailure("Protocol data was not sent");
        return false;
    }

    if (log)
        logSuccess("Protocol data was sent");
    return true;
}

auto btrader::CentaurClient::sendProtocolData(cen::protocol::message::Protocol_MessageBase *message, bool compress, bool log) noexcept -> bool
{
    using namespace CENTAUR_PROTOCOL_NAMESPACE::message;
    CENTAUR_PROTOCOL_NAMESPACE::Protocol pro;
    CENTAUR_PROTOCOL_NAMESPACE::Generator::generate(&pro, g_userVersion, message, compress);
    if (!sendData(&pro, log))
    {
        logWarning("Protocol message was not send");
        return false;
    }
    return true;
}

auto btrader::CentaurClient::handleProtocolMessage(uint32_t type, uint32_t version, const std::string &message) noexcept -> void
{
    if (version < g_userVersion)
    {
        logError("Protocol Message version received is not supported");
        return;
    }

    switch (type)
    {
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_AcceptConnection: C_FALLTHROUGH;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_AccessPluginMetadata: C_FALLTHROUGH;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalanceAssetItemUpdate: C_FALLTHROUGH;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalanceAssetUpdate: C_FALLTHROUGH;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalancesAsset: C_FALLTHROUGH;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalancesAssetItem:
            logWarning("Unexpected protocol message response");
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_IncomingConnection:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_IncomingConnection inc;
                inc.fromJson(message.c_str());
                m_connectionId = inc.uuid();

                logInfo("Protocol_IncomingConnection message received");

                using namespace CENTAUR_PROTOCOL_NAMESPACE::message;
                CENTAUR_PROTOCOL_NAMESPACE::Protocol pro;
                Protocol_AcceptConnection protocolAcceptConnection;
                protocolAcceptConnection.uuid() = m_connectionId;
                protocolAcceptConnection.name() = "BSPOT";
                CENTAUR_PROTOCOL_NAMESPACE::Generator::generate(&pro, g_userVersion, &protocolAcceptConnection, false);

                if (!sendData(&pro))
                {
                    logWarning("Connection will be stopped");
                    terminate();
                }

                // Wait for the response
                m_app->addResponse(TimeoutType::acceptConnection);
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_AcceptedConnection:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_AcceptedConnection pac;
                pac.fromJson(message.c_str());

                if (pac.uuid() != m_connectionId)
                {
                    logError("Connection established but returned an unexpected UUID");
                    terminate();
                    return;
                }

                m_app->onSetCentaurConnection();
                logSuccess("Protocol Message. The server acknowledge the connection");

                m_app->onAcquireFirstBalances();
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalancesResponse:
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_PluginMetadataResponse:
            break;
        default:
            logWarning("Unknown protocol message");
    }
}
auto btrader::CentaurClient::sendBalanceAsset(const std::string &name, const std::string &source, const std::string &icon) -> std::string
{
    logInfo("Send Balance Asset: {}", name);
    using namespace CENTAUR_PROTOCOL_NAMESPACE::message;
    Protocol_BalancesAsset asset;

    SET_BASICS(asset)

    asset.asset()     = name;
    asset.total()     = source;
    asset.assetIcon() = icon;
    asset.assetId()   = cen::uuid::generate<std::mt19937_64>().to_string();

    sendProtocolData(&asset, true);

    return asset.assetId();
}

auto btrader::CentaurClient::sendBalanceAssetItem(const std::string &id, const std::string &name, const std::string &value, const std::string &icon) -> std::string
{
    logInfo("Send Balance Asset Item: {} ({})", name, value);
    using namespace CENTAUR_PROTOCOL_NAMESPACE::message;
    Protocol_BalancesAssetItem asset;

    SET_BASICS(asset)

    asset.handle()    = id;
    asset.name()      = name;
    asset.value()     = value;
    asset.subHandle() = cen::uuid::generate<std::mt19937_64>().to_string();
    asset.icon()      = icon;

    sendProtocolData(&asset, true);

    return asset.subHandle();
}

auto btrader::CentaurClient::sendIcon(const std::string &id, const char *data, int format) noexcept -> void
{
    logInfo("Send icon data: {}.", id);
    using namespace CENTAUR_PROTOCOL_NAMESPACE::message;
    Protocol_Icon icon;

    SET_BASICS(icon)

    icon.iconId() = id;
    icon.data()   = data;
    icon.format() = format;

    sendProtocolData(&icon, true);
}

auto btrader::CentaurClient::sendBalanceAssetTotalUpdate(const std::string &id, double total) noexcept -> void
{
    using namespace CENTAUR_PROTOCOL_NAMESPACE::message;
    Protocol_BalanceTotalUpdate update;

    SET_BASICS(update)

    update.assetId() = id;
    update.display() = fmt::format("$ {:.5f}", total);
    update.total()   = total;

    sendProtocolData(&update, false /*no for need the extra overhead of the compression*/, false);
}
