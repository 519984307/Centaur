/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 14/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_COMSSERVER_HPP
#define CENTAUR_COMSSERVER_HPP

#include "CentaurGlobal.hpp"
#include "ProtocolClient.hpp"
#include <QWebSocketServer>

namespace CENTAUR_NAMESPACE
{
    class ProtocolServer : public QWebSocketServer
    {
        Q_OBJECT
    public:
        explicit ProtocolServer(QObject *parent = nullptr);
        ~ProtocolServer() override;

    public slots:
        void onAcceptError(QAbstractSocket::SocketError socketError) noexcept;
        void onClosed() noexcept;
        void onNewConnection() noexcept;
        void onServerError(QWebSocketProtocol::CloseCode closeCode) noexcept;

    public:
        /// \brief Checks whether a client id exists and the socket associated is actually associated with the id
        /// \remarks This means: is prohibited that a client send a message with an id that its not associated with its connection.
        bool isClientSocket(const QString &id, QWebSocket *webSocket) const noexcept;

    protected:

    private:
        QMap<QString, ProtocolClient *> m_clients;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_COMSSERVER_HPP
       /*void	acceptError(QAbstractSocket::SocketError socketError)
       void	alertReceived(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description)
       void	alertSent(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description)
       void	closed()
       void	handshakeInterruptedOnError(const QSslError &error)
       void	newConnection()
       void	originAuthenticationRequired(QWebSocketCorsAuthenticator *authenticator)
       void	peerVerifyError(const QSslError &error)
       void	preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *authenticator)
       void	serverError(QWebSocketProtocol::CloseCode closeCode)
       void	sslErrors(const QList<QSslError> &errors)*/
