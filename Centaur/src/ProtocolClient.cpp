/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ProtocolClient.hpp"
#include "CentaurApp.hpp"
#include "Protocol.hpp"
#include "ProtocolServer.hpp"
#include "ProtocolType.hpp"

#define LOG_NAME m_clientName.isEmpty() ? "client" : m_clientName

cen::ProtocolClient::ProtocolClient(QWebSocket *socket, QObject *parent) :
    QObject(parent),
    m_socket { socket }
{
    assert(m_socket != nullptr);
}

cen::ProtocolClient::~ProtocolClient() = default;

void cen::ProtocolClient::sendData(cen::protocol::ProtocolBase *data) noexcept
{
    protocol::Protocol pro;
    protocol::Generator::generate(&pro, CENTAUR_PROTOCOL_VERSION, data);
    m_socket->sendBinaryMessage(QByteArray::fromRawData(reinterpret_cast<char *>(pro.get()), static_cast<qsizetype>(pro.getSize())));
}

void cen::ProtocolClient::onTextMessageReceived(C_UNUSED const QString &message) noexcept
{
    // ANY ATTEMPT OF SENDING TEXT MESSAGES WILL BE DISCARED
    logWarn("client", LS("warning-wrong-protocol-message-type"));
    if (++m_textAttempts > g_maxTextAttempts)
    {
        logWarn("client", LS("warning-wrong-protocol-message-type-disconnect"));
        m_socket->close(QWebSocketProtocol::CloseCodeBadOperation, LS("info-wrong-message-types"));
    }
}

void cen::ProtocolClient::onBinaryMessageReceived(const QByteArray &message) noexcept
{
    CENTAUR_PROTOCOL_NAMESPACE::ProtocolHeader header {};
    auto data = CENTAUR_PROTOCOL_NAMESPACE::Generator::getData(
        &header,
        reinterpret_cast<const uint8_t *>(message.data()),
        static_cast<size_t>(message.size()),
        10000);

    bool messageCorrect = true;
    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongSize)
    {
        messageCorrect = false;
        logError("client", LS("error-protocol-size"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongMagic)
    {
        messageCorrect = false;
        logError("client", LS("error-protocol-unknown-data"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongHash)
    {
        messageCorrect = false;
        logError("client", LS("error-protocol-corrupted"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongMessageSize)
    {
        messageCorrect = false;
        logError("client", LS("error-protocol-payload-size"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongTimestamp)
    {
        messageCorrect = false;
        logError("client", LS("error-protocol-out-of-sync"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongVersion)
    {
        messageCorrect = false;
        logError("client", LS("error-protocol-wrong-version"));
    }

    if (messageCorrect)
    {
        onHandleProtocolMessage(header.type, data);
    }
    else
    {
        if (++m_wrongFormat > g_maxWrongFormatted)
        {
            logWarn("client", LS("warning-wrong-protocol-message-format-disconnect"));
            m_socket->close(QWebSocketProtocol::CloseCodeBadOperation, LS("info-wrong-message-format"));
        }
    }
}

void cen::ProtocolClient::onDisconnected() noexcept
{
    logInfo("client", QString(LS("info-client-disconnected")).arg(m_clientName.isEmpty() ? "unknown" : m_clientName));
}

void cen::ProtocolClient::onHandleProtocolMessage(uint32_t type, const std::string &data)
{
    if (type != CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_AcceptConnection && !m_accepted)
    {
        // Automatic disconnection
        logWarn("client", LS("warning-automatic-disconnection"));
        m_socket->close(QWebSocketProtocol::CloseCodeBadOperation, LS("info-automatic_disconnection"));
    }

    switch (type)
    {
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_AcceptConnection:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_AcceptConnection acc;
                acc.fromJson(data.c_str());
                onHandleAcceptConnection(&acc);
            }
            break;
    }
}

void cen::ProtocolClient::onHandleAcceptConnection(cen::protocol::message::Protocol_AcceptConnection *accept)
{
    auto server = qobject_cast<ProtocolServer *>(this->parent());
    assert(server != nullptr);

    // Server. Check the UUID
    if (!server->isClientSocket(QString::fromStdString(accept->uuid()), m_socket))
    {
        logWarn(LOG_NAME, LS("warning-client-uuid"));
        return;
    }

    // Set the id
    m_id = accept->uuid();
    // Set the name of the connection
    m_clientName = QString::fromStdString(accept->name());
    logInfo(LOG_NAME, QString(LS("info-protocol-name")).arg(m_clientName));

    // Send the accepted connection
    protocol::message::Protocol_AcceptedConnection accepted;
    accepted.uuid() = m_id;
    sendData(&accepted);
}

void cen::ProtocolClient::sendIncomingConnection(const QString &id) noexcept
{
    // Send the acceptance connection
    protocol::message::Protocol_IncomingConnection in;
    in.uuid() = id.toStdString();
    sendData(&in);
}
