/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 14/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ProtocolServer.hpp"
#include "CentaurApp.hpp"
#include "Protocol.hpp"
#include "ProtocolType.hpp"
#include <QHostAddress>

cen::ProtocolServer::ProtocolServer(QObject *parent) :
    QWebSocketServer("CentaurCommunticationProtocol", QWebSocketServer::SslMode::NonSecureMode, parent)
{
    const qint16 port = 80;
    if (listen(QHostAddress("localhost"), port))
    {
        logInfo("server", QString(LS("info-server-listening")).arg(port));
        connect(this, &QWebSocketServer::acceptError, this, &ProtocolServer::onAcceptError);
        connect(this, &QWebSocketServer::closed, this, &ProtocolServer::onClosed);
        connect(this, &QWebSocketServer::newConnection, this, &ProtocolServer::onNewConnection);
        connect(this, &QWebSocketServer::serverError, this, &ProtocolServer::onServerError);
    }
}

cen::ProtocolServer::~ProtocolServer() = default;

void cen::ProtocolServer::onAcceptError(QAbstractSocket::SocketError socketError) noexcept
{
    qDebug() << "ACCEPT ERROR";
}

void cen::ProtocolServer::onClosed() noexcept
{
    for (auto &socket : m_clients)
    {
        delete socket;
    }
}

void cen::ProtocolServer::onNewConnection() noexcept
{
    auto socket = this->nextPendingConnection();
    if (socket == nullptr)
    {
        logError("server", LS("error-server-empty_connection"));
        return;
    }
    auto client     = new ProtocolClient(socket, this);

    auto generateId = []() -> QString {
        return QString::fromStdString(uuid::generate<std::mt19937>().to_string());
    };

    QString id { generateId() };
    while (m_clients.constFind(id) != m_clients.cend())
        id = generateId();

    // Insert the new Id
    m_clients.insert(id, client);

    // Make the connections
    connect(socket, &QWebSocket::binaryMessageReceived, client, &ProtocolClient::onBinaryMessageReceived);
    connect(socket, &QWebSocket::textMessageReceived, client, &ProtocolClient::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected, client, &ProtocolClient::onDisconnected);

    client->sendIncomingConnection(id);
}

void cen::ProtocolServer::onServerError(QWebSocketProtocol::CloseCode closeCode) noexcept
{
    qDebug() << "SERVER ERROR";
}

bool cen::ProtocolServer::isClientSocket(const QString &id, QWebSocket *webSocket) const noexcept
{
    auto iter = m_clients.constFind(id);
    if (iter == m_clients.constEnd())
        return false;
    return iter.value()->getSocket() == webSocket;
}
