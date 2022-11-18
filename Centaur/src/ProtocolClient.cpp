/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ProtocolClient.hpp"
#include "CentaurApp.hpp"
#include "Globals.hpp"
#include "Protocol.hpp"
#include "ProtocolServer.hpp"
#include "ProtocolType.hpp"

#define LOG_NAME m_clientName.isEmpty() ? "client" : m_clientName

#define VERIFY_CLIENT_UUID(msg)                                                 \
    auto server = qobject_cast<ProtocolServer *>(this->parent());               \
    assert(server != nullptr);                                                  \
    if (!server->isClientSocket(QString::fromStdString(msg->uuid()), m_socket)) \
    {                                                                           \
        logWarn(LOG_NAME, tr("A client sent the wrong identification string")); \
        return;                                                                 \
    }

#define MAKE_BALANCE_RESPONSE(msg, sts)                                                                       \
    protocol::message::Protocol_BalancesResponse response;                                                    \
    response.responseId() = msg->responseId();                                                                \
    response.status()     = static_cast<int>(cen::protocol::message::Protocol_BalancesResponse::Status::sts); \
    sendData(&response);

namespace
{
    constexpr char g_totalRed[] {
        R"(QLabel{
color: rgb(172, 6, 0);
border-radius: 3px;
min-width: 85px;
qproperty-alignment: AlignCenter;
})"
    };

    constexpr char g_totalGreen[] {
        R"(QLabel{
color: rgb(0, 104, 18);
border-radius: 3px;
min-width: 85px;
qproperty-alignment: AlignCenter;
})"
    };
} // namespace

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
    // ANY ATTEMPT OF SENDING TEXT MESSAGES WILL BE DISCARDED
    logWarn("client", tr("A client send a wrong message type. More attempts will result in a disconnection"));
    if (++m_textAttempts > g_maxTextAttempts)
    {
        logWarn("client", tr("A client sent too many wrong message types. The client will be disconnected"));
        m_socket->close(QWebSocketProtocol::CloseCodeBadOperation, tr("Too many wrong messages type"));
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
        logError("client", tr("Data size received is not consistent with a Centaur Communication Protocol"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongMagic)
    {
        messageCorrect = false;
        logError("client", tr("The message received is not consistent with a Centaur Communication Protocol"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongHash)
    {
        messageCorrect = false;
        logError("client", tr("Centaur Communication Protocol. Data is corrupted"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongMessageSize)
    {
        messageCorrect = false;
        logError("client", tr("Centaur Communication Protocol. Data received is incomplete"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongTimestamp)
    {
        messageCorrect = false;
        logError("client", tr("Centaur Communication Protocol. Data received is out of time"));
    }

    if (header.flags & CENTAUR_PROTOCOL_NAMESPACE::PFWrongVersion)
    {
        messageCorrect = false;
        logError("client", tr("Centaur Communication Protocol. Library Version and Message Version are incompatible"));
    }

    if (messageCorrect)
    {
        onHandleProtocolMessage(header.type, data);
    }
    else
    {
        if (++m_wrongFormat > g_maxWrongFormatted)
        {
            logWarn("client", tr("A client sent too many messages with a wrong format. The client will be disconnected"));
            m_socket->close(QWebSocketProtocol::CloseCodeBadOperation, tr("Too many messages with the wrong format"));
        }
    }
}

void cen::ProtocolClient::onDisconnected() noexcept
{
    logInfo("client", QString(tr("The client with name %1 was disconnected")).arg(m_clientName.isEmpty() ? "unknown" : m_clientName));

    for (auto &item : m_balancesItems)
    {
        if (item.second)
            delete item.first;
    }
}

void cen::ProtocolClient::onHandleProtocolMessage(uint32_t type, const std::string &data)
{
    if (type != CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_AcceptConnection && !m_accepted)
    {
        // Automatic disconnection
        logWarn("client", tr("A protocol-message was sent without a prior connection-acceptance from the server"));
        m_socket->close(QWebSocketProtocol::CloseCodeBadOperation, tr("The client sent a protocol message without a prior connection-acceptance from the server"));
    }

    switch (type)
    {
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_AcceptConnection:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_AcceptConnection protocol;
                protocol.fromJson(data.c_str());
                onHandleAcceptConnection(&protocol);
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_Icon:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon protocol;
                protocol.fromJson(data.c_str());
                onReceiveIcon(&protocol);
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalancesAsset:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalancesAsset protocol;
                protocol.fromJson(data.c_str());
                onBalanceAsset(&protocol);
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalancesAssetItem:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalancesAssetItem protocol;
                protocol.fromJson(data.c_str());
                onBalanceAssetItem(&protocol);
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalanceAssetUpdate:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalanceAssetUpdate protocol;
                protocol.fromJson(data.c_str());
                onBalanceAssetUpdate(&protocol);
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalanceAssetItemUpdate:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalanceAssetItemUpdate protocol;
                protocol.fromJson(data.c_str());
                onBalanceAssetItemUpdate(&protocol);
            }
            break;
        case CENTAUR_PROTOCOL_NAMESPACE::type::Protocol_BalanceTotalUpdate:
            {
                CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_BalanceTotalUpdate protocol;
                protocol.fromJson(data.c_str());
                onBalanceTotalUpdate(&protocol);
            }
            break;

        default:
            logWarn(LOG_NAME, tr("Unknown protocol message"));
            break;
    }
}

void cen::ProtocolClient::onHandleAcceptConnection(cen::protocol::message::Protocol_AcceptConnection *accept)
{
    VERIFY_CLIENT_UUID(accept)

    m_accepted = true;
    // Set the id
    m_id = accept->uuid();
    // Set the name of the connection
    m_clientName = QString::fromStdString(accept->name());
    logInfo(LOG_NAME, QString(tr("A client connection with name -%1- was accepted.")).arg(m_clientName));

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

auto cen::ProtocolClient::onBalanceAsset(cen::protocol::message::Protocol_BalancesAsset *asset) noexcept -> void
{
    VERIFY_CLIENT_UUID(asset);
    /*
        // Verify the list of asset id's
        if (m_balancesItems.contains(QString::fromStdString(asset->assetId())))
        {
            MAKE_BALANCE_RESPONSE(asset, repeatedHandle);
        }
        else
        {
            auto tree = g_app->getBalancesTree();

            auto item = new QTreeWidgetItem(tree, { QString::fromStdString(asset->asset()), QString::fromStdString(asset->total()) });

            item->setForeground(1, QBrush(QColor(158, 231, 255)));
            if (!asset->assetIcon().empty())
            {
                QPixmap pm;
                g_globals->symIcons.find(32, QString::fromStdString(asset->assetIcon()), &pm, 0);
                if (!pm.isNull())
                    item->setIcon(0, pm);
            }

            m_balancesItems.insert(QString::fromStdString(asset->assetId()), { item, true });
            logInfo(LOG_NAME, QString(LS("info-asset-received")).arg(QString::fromStdString(asset->asset())));
            MAKE_BALANCE_RESPONSE(asset, allOk);
        }*/
}

auto cen::ProtocolClient::onBalanceAssetItem(cen::protocol::message::Protocol_BalancesAssetItem *assetItem) noexcept -> void
{
    VERIFY_CLIENT_UUID(assetItem);

    // Verify the parent
    auto iter = m_balancesItems.find(QString::fromStdString(assetItem->handle()));
    if (iter == m_balancesItems.end())
    {
        MAKE_BALANCE_RESPONSE(assetItem, handle);
    }
    else
    {
        if (m_balancesItems.contains(QString::fromStdString(assetItem->subHandle())))
        {
            MAKE_BALANCE_RESPONSE(assetItem, repeatedSubHandle);
        }
        else
        {
            auto itemParent = iter.value().first;
            auto item       = new QTreeWidgetItem(itemParent, { QString::fromStdString(assetItem->name()), QString::fromStdString(assetItem->value()) });

            // QIcon ico{m_icons[QString::fromStdString(assetItem->icon())]};

            item->setIcon(0,
                QIcon(QPixmap(
                    m_icons[QString::fromStdString(assetItem->icon())])
                          .scaled(
                              QSize(20, 20),
                              Qt::KeepAspectRatio, Qt::SmoothTransformation)));

            m_balancesItems.insert(QString::fromStdString(assetItem->subHandle()), { item, false });

            MAKE_BALANCE_RESPONSE(assetItem, allOk);
            logInfo(LOG_NAME, QString(tr("A client sent a balance asset item -%1")).arg(QString::fromStdString(assetItem->name())));
        }
    }
}

auto cen::ProtocolClient::onBalanceAssetUpdate(cen::protocol::message::Protocol_BalanceAssetUpdate *update) noexcept -> void
{
    VERIFY_CLIENT_UUID(update);
}

auto cen::ProtocolClient::onBalanceAssetItemUpdate(cen::protocol::message::Protocol_BalanceAssetItemUpdate *update) noexcept -> void
{
    VERIFY_CLIENT_UUID(update);
}

auto cen::ProtocolClient::onReceiveIcon(cen::protocol::message::Protocol_Icon *icon) noexcept -> void
{
    VERIFY_CLIENT_UUID(icon);

    QString id { QString::fromStdString(icon->iconId()) };

    if (m_icons.contains(id))
    {
        logError(LOG_NAME, tr("A client sent an image with an already existing ID"));
        return;
    }

    QByteArray data { QByteArray::fromBase64(QByteArray::fromStdString(icon->data())) };
    const char *format;

    switch (icon->format())
    {
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_BMP: format = "bmp"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_GIF: format = "gif"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_JPG: format = "jpg"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_PNG: format = "png"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_PBM: format = "pbm"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_PGM: format = "pgm"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_PPM: format = "ppm"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_XBM: format = "xbm"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_XPM: format = "xpm"; break;
        case CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_SVG: format = "svg"; break;
    }

    QPixmap pm;
    pm.loadFromData(data, format);
    m_icons.insert(id, pm);

    logInfo(LOG_NAME, QString(tr("A client sent an icon with format -%1- and ID -%2-")).arg(format, id));
}

auto cen::ProtocolClient::onBalanceTotalUpdate(cen::protocol::message::Protocol_BalanceTotalUpdate *update) noexcept -> void
{
    VERIFY_CLIENT_UUID(update);

    /*
    QString id { QString::fromStdString(update->assetId()) };
    QString display { QString::fromStdString(update->display()) };

    auto iter = m_balancesItems.find(id);
    if (iter != m_balancesItems.end())
    {
        iter.value().first->setText(1, display);

        // Dynamic adding
        m_totalBalances[id] = update->total();

        double total        = 0.0;
        for (auto &totals : m_totalBalances)
            total += totals;

        if (total >= m_previousTotal)
            g_app->getTotalLabel()->setStyleSheet(g_totalGreen);
        else
            g_app->getTotalLabel()->setStyleSheet(g_totalRed);

        g_app->getTotalLabel()->setText(QString("$ %1").arg(QLocale(QLocale::English).toString(total, 'f', 5)));
    }*/
}
