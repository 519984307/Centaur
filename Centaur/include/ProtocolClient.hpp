/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PROTOCOLCLIENT_HPP
#define CENTAUR_PROTOCOLCLIENT_HPP

#include "Centaur.hpp"
#include "CentaurGlobal.hpp"
#include "Protocol.hpp"
#include <QTreeWidgetItem>
#include <QWebSocket>

namespace CENTAUR_NAMESPACE
{
    /// \brief Hard-coded max text attempts
    constexpr int g_maxTextAttempts = 10;
    /// \brief Hard-coded max wrong format messages
    constexpr int g_maxWrongFormatted = 100;

    class ProtocolClient : public QObject
    {
        Q_OBJECT
    public:
        explicit ProtocolClient(QWebSocket *socket, QObject *parent = nullptr);
        ~ProtocolClient() override;

    public slots:
        void onTextMessageReceived(const QString &message) noexcept;
        void onBinaryMessageReceived(const QByteArray &message) noexcept;
        void onDisconnected() noexcept;

    public:
        void sendData(CENTAUR_PROTOCOL_NAMESPACE::ProtocolBase *data) noexcept;
        auto onBalanceAsset(CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalancesAsset *asset) noexcept -> void;
        auto onBalanceAssetItem(CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalancesAssetItem *assetItem) noexcept -> void;
        auto onBalanceAssetUpdate(CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalanceAssetUpdate *update) noexcept -> void;
        auto onBalanceAssetItemUpdate(CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalanceAssetItemUpdate *update) noexcept -> void;
        auto onReceiveIcon(CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon *icon) noexcept -> void;
        auto onBalanceTotalUpdate(CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalanceTotalUpdate *update) noexcept -> void;

    public:
        void sendIncomingConnection(const QString &id) noexcept;

    public:
        inline QWebSocket *getSocket() noexcept { return m_socket; }

    protected:
        /// \brief Handle all protocol messages
        /// \param type Type of the message
        /// \param data JSON Data
        /// \remarks USER VERSION is totally ignore since we do not need that information
        /// This parameter is sent from the specialized message interface or through a plugin and that is up to the client to check that value
        void onHandleProtocolMessage(uint32_t type, const std::string &data);

    protected:
        void onHandleAcceptConnection(CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_AcceptConnection *accept);

    protected:
        /// \brief If the client send to much wrong formatted messages the client will be disconnect
        int m_wrongFormat { 0 };
        /// \brief When a client send > 10 text messages the client will be disconnect
        int m_textAttempts { 0 };
        bool m_accepted { false };
        QString m_clientName {};
        std::string m_id {};

    private:
        QMap<QString, QPair<QTreeWidgetItem *, bool>> m_balancesItems;
        QMap<QString, double> m_totalBalances;
        QMap<QString, QPixmap> m_icons;
        QWebSocket *m_socket;
        double m_previousTotal { 0.0 };
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_PROTOCOLCLIENT_HPP
