/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "BinanceStatus.hpp"
#include "Protocol.hpp"
#include "StatusDialog.hpp"
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <fmt/core.h>

/// UUID v5 Hashed String: CentaurProject-BinanceSPOTStatus-0.1.0

namespace
{
    constexpr char g_ExchangeRateName[]          = "BinanceSPOTStatus";
    constexpr char g_ExchangeRateVersionString[] = "0.1.0";
    constexpr char g_uuidString[]                = "{c9c99ff8-c074-5697-b5ad-80bb6fe7337a}";

    constexpr char g_statusRed[] {
        R"(QLabel{
color: rgb(255, 255, 255);
background-color: rgb(172, 6, 0);
border-radius: 3px;
min-width: 85px;
qproperty-alignment: AlignCenter;
})"
    };

    constexpr char g_statusGreen[] {
        R"(QLabel{
color: rgb(255, 255, 255);
background-color: rgb(0, 104, 18);
border-radius: 3px;
min-width: 85px;
qproperty-alignment: AlignCenter;
})"
    };

    constexpr char g_statusBlue[] {
        R"(QLabel{
color: rgb(25, 25, 25);
background-color: rgb(158, 231, 255);
border-radius: 3px;
min-width: 85px;
qproperty-alignment: AlignCenter;
})"
    };
} // namespace

#define CATCH_API_EXCEPTION()                                                                                                                                              \
    switch (ex.type())                                                                                                                                                     \
    {                                                                                                                                                                      \
        case BINAPI_NAMESPACE::APIException::Type::request:                                                                                                                \
            {                                                                                                                                                              \
                auto request = ex.request();                                                                                                                               \
                logError("BinanceSPOTStatus", fmt::format("REQUEST.\nCode: {}. URL: {}\n{}\n", std::get<0>(request), std::get<1>(request), std::get<2>(request)).c_str()); \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::limits:                                                                                                                 \
            logError("BinanceSPOTStatus", fmt::format("LIMITS.\n{}\n", std::get<0>(ex.limits())).c_str());                                                                 \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::http1:                                                                                                                  \
            {                                                                                                                                                              \
                auto http1 = ex.http1();                                                                                                                                   \
                logError("BinanceSPOTStatus", fmt::format("HTTPa.\nCode: {}\nURL: {}\n{}\n", std::get<0>(http1), std::get<1>(http1), std::get<2>(http1)).c_str());         \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::http2:                                                                                                                  \
            {                                                                                                                                                              \
                auto http2 = ex.http2();                                                                                                                                   \
                logError("BinanceSPOTStatus", fmt::format("HTTPb.\nStatus: {}. Code: {}\nURL: {}.\n{}\n",                                                                  \
                                                  std::get<0>(http2),                                                                                                      \
                                                  std::get<1>(http2),                                                                                                      \
                                                  std::get<2>(http2),                                                                                                      \
                                                  std::get<4>(http2))                                                                                                      \
                                                  .c_str());                                                                                                               \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::json:                                                                                                                   \
            {                                                                                                                                                              \
                auto json = ex.json();                                                                                                                                     \
                logError("BinanceSPOTStatus", fmt::format("JSON.\nURL: {}\n{}\n", std::get<0>(json), std::get<1>(json)).c_str());                                          \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::api:                                                                                                                    \
            {                                                                                                                                                              \
                auto api1 = ex.api();                                                                                                                                      \
                logError("BinanceSPOTStatus", fmt::format("API.\nCode: {}\nURL: {}\n{}", std::get<0>(api1), std::get<1>(api1), std::get<2>(api1)).c_str());                \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::schema:                                                                                                                 \
            {                                                                                                                                                              \
                auto schema = ex.schema();                                                                                                                                 \
                logError("BinanceSPOTStatus", fmt::format("SCHEMA.\nURL: {}\n{}\n", std::get<0>(schema), std::get<1>(schema)).c_str());                                    \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
    }

cen::plugin::ClickableLabel::ClickableLabel(QWidget *parent) :
    QLabel { parent }
{
}
cen::plugin::ClickableLabel::~ClickableLabel() = default;
void cen::plugin::ClickableLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    emit snClick();
    QLabel::mouseReleaseEvent(ev);
}

cen::plugin::BinanceSPOTStatus::BinanceSPOTStatus(QObject *parent) :
    QObject(parent),
    m_thisUUID { g_uuidString }
{
}

QObject *cen::plugin::BinanceSPOTStatus::getPluginObject() noexcept
{
    return qobject_cast<QObject *>(this);
}

QString cen::plugin::BinanceSPOTStatus::getPluginName() noexcept
{
    return g_ExchangeRateName;
}

QString cen::plugin::BinanceSPOTStatus::getPluginVersionString() noexcept
{
    return g_ExchangeRateVersionString;
}

void cen::plugin::BinanceSPOTStatus::setPluginInterfaces(cen::interface::ILogger *logger, cen::interface::IConfiguration *config, C_UNUSED cen::interface::ILongOperation *lOper) noexcept
{
    m_logger = logger;
    m_config = config;
}

cen::uuid cen::plugin::BinanceSPOTStatus::getPluginUUID() noexcept
{
    return m_thisUUID;
}

bool cen::plugin::BinanceSPOTStatus::addMenuAction(C_UNUSED QAction *action, C_UNUSED const uuid &menuId) noexcept
{
    return false;
}

void cen::plugin::BinanceSPOTStatus::initialization(QStatusBar *bar) noexcept
{
    logTrace("BinanceSPOTStatus", "BinanceSPOTStatus::initialization");

    m_status = new ClickableLabel(bar);
    m_status->setText("BinanceSPOT Account Status: Unknown");
    m_status->setStyleSheet(g_statusBlue);
    bar->addPermanentWidget(m_status, 0);

    connect(m_status, &ClickableLabel::snClick, this, &BinanceSPOTStatus::onViewData);

    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;
    auto publicKeyFile = m_config->getPluginPublicKeyPath();

    try
    {
        ec.loadPublicKey(publicKeyFile);
    } catch (const std::exception &ex)
    {
        logError("BinanceSPOTStatus", QString("Could not load the plugin public key. %1").arg(ex.what()));
        return;
    }

    bool apiError, secError, timerError;
    const auto apiKeyCip = m_config->getValue("apiKey", &apiError);
    const auto secKeyCip = m_config->getValue("secretKey", &secError);
    auto timerTime       = m_config->getInt32("update-time", &timerError);

    if (!apiError || !secError || !timerError)
    {
        logError("BinanceSPOTStatus", "Either the API Key or the secret key are empty on the plugins configuration file.");
        return;
    }

    const auto apiKey = ec.decryptPublic(apiKeyCip, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64);
    const auto secKey = ec.decryptPublic(secKeyCip, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64);

    if (apiKey.empty() || secKey.empty())
    {
        logError("BinanceSPOTStatus", "Could not decrypt the binance keys");
        return;
    }

    // set the keys
    m_keys.apiKey    = apiKey;
    m_keys.secretKey = secKey;

    if (timerTime < 300'000)
    {
        logWarn("BinanceSPOTStatus", "Account Status time check is very low. Defaulting to 300,000 ms (5 min)");
        timerTime = 300'000;
    }

    m_api      = std::make_unique<BINAPI_NAMESPACE::BinanceAPISpot>(&m_keys, &m_limits);

    auto timer = new QTimer { this };

    // We don't' need precise timers
    timer->setTimerType(Qt::TimerType::CoarseTimer);
    timer->setSingleShot(false);

    connect(timer, &QTimer::timeout, this, &BinanceSPOTStatus::onCheckAccountStatus);
    timer->start(timerTime);

    onCheckAccountStatus();
}

void cen::plugin::BinanceSPOTStatus::onCheckAccountStatus() noexcept
{
    logTrace("BinanceSPOTStatus", "BinanceSPOTStatus::onCheckAccountStatus");

    m_status->setText("BinanceSPOT Account Status: Unknown");
    m_status->setStyleSheet(g_statusBlue);

    try
    {
        m_accountStatus    = QString::fromStdString(m_api->accountStatus());
        m_apiPermissions   = m_api->getAPIKeyPermission();
        m_apiTradingStatus = m_api->accountAPITradingStatus();

        m_status->setText(QString("BinanceSPOT Account Status: %1").arg(m_accountStatus));
        if (m_accountStatus == "Normal")
            m_status->setStyleSheet(g_statusGreen);
        else
            m_status->setStyleSheet(g_statusRed);

    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        CATCH_API_EXCEPTION()
    }
}

void cen::plugin::BinanceSPOTStatus::onViewData() noexcept
{
    logTrace("BinanceSPOTStatus", "BinanceSPOTStatus::onViewData");
    StatusDialog dlg(&m_apiPermissions, &m_apiTradingStatus, m_status->text(), nullptr);
    dlg.setModal(true);
    dlg.exec();
}
