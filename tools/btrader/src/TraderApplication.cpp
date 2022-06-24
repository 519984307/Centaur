/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include <filesystem>

#include "Protocol.hpp"
#include "TraderApplication.hpp"
#include "ui/Icons.hpp"

#ifdef P_APPLE
#include "apple/AppleDirectory.h"
#endif /*P_APPLE*/

#define CATCH_API_EXCEPTION()                                                                                                      \
    switch (ex.type())                                                                                                             \
    {                                                                                                                              \
        case BINAPI_NAMESPACE::APIException::Type::request:                                                                        \
            {                                                                                                                      \
                auto request = ex.request();                                                                                       \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("REQUEST.\nCode: {}. URL: {}\n{}\n", std::get<0>(request), std::get<1>(request), std::get<2>(request)); \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::limits:                                                                         \
            fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                       \
            fmt::print("LIMITS.\n{}\n", std::get<0>(ex.limits()));                                                                 \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::http1:                                                                          \
            {                                                                                                                      \
                auto http1 = ex.http1();                                                                                           \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("HTTPa.\nCode: {}\nURL: {}\n{}\n", std::get<0>(http1), std::get<1>(http1), std::get<2>(http1));         \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::http2:                                                                          \
            {                                                                                                                      \
                auto http2 = ex.http2();                                                                                           \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("HTTPb.\nStatus: {}. Code: {}\nURL: {}.\n{}\n",                                                         \
                    std::get<0>(http2),                                                                                            \
                    std::get<1>(http2),                                                                                            \
                    std::get<2>(http2),                                                                                            \
                    std::get<4>(http2)                                                                                             \
                        .c_str());                                                                                                 \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::json:                                                                           \
            {                                                                                                                      \
                auto json = ex.json();                                                                                             \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("JSON.\nURL: {}\n{}\n", std::get<0>(json), std::get<1>(json));                                          \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::api:                                                                            \
            {                                                                                                                      \
                auto api1 = ex.api();                                                                                              \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("API.\nCode: {}\nURL: {}\n{}", std::get<0>(api1), std::get<1>(api1), std::get<2>(api1));                \
            }                                                                                                                      \
            break;                                                                                                                 \
        case BINAPI_NAMESPACE::APIException::Type::schema:                                                                         \
            {                                                                                                                      \
                auto schema = ex.schema();                                                                                         \
                fmt::print(fmt::fg(fmt::color::red), "ERROR: ");                                                                   \
                fmt::print("SCHEMA.\nURL: {}\n{}\n", std::get<0>(schema), std::get<1>(schema));                                    \
            }                                                                                                                      \
            break;                                                                                                                 \
    }

cen::tools::ToolLogger<20> *g_futuresLogger = nullptr;

btrader::TraderApplication::TraderApplication()
{
    /// Init logger basics
    g_futuresLogger = new cen::tools::ToolLogger<20>("/Volumes/RicardoESSD/Projects/Centaur/tools/local/log");

    g_futuresLogger->addEventName(static_cast<uint32_t>(btrader::logger::LoggingEvent::warning), { "warning", fmt::color::yellow });
    g_futuresLogger->addEventName(static_cast<uint32_t>(btrader::logger::LoggingEvent::info), { "info", fmt::color::dodger_blue });
    g_futuresLogger->addEventName(static_cast<uint32_t>(btrader::logger::LoggingEvent::error), { "error", fmt::color::red });
    g_futuresLogger->addEventName(static_cast<uint32_t>(btrader::logger::LoggingEvent::success), { "success", fmt::color::green_yellow });
    g_futuresLogger->addEventName(static_cast<uint32_t>(btrader::logger::LoggingEvent::failure), { "failed", fmt::color::orange_red });
    g_futuresLogger->addEventName(static_cast<uint32_t>(btrader::logger::LoggingEvent::timeout), { "timeout", fmt::color::medium_purple });

    m_loggerThread = std::thread([] { g_futuresLogger->run(); });

    std::string applicationSupportFile;
    logInfo("Retrieving keys file... ");

#if defined(P_APPLE)
    // Apple Specific
    applicationSupportFile = getApplicationSupportDirectory();
#elif defined(P_LINUX)
    // LINUX Specific
#elifdefined(P_WINDOWS)
    // Windows Specific
#endif
    applicationSupportFile += "/Centaur/bspot.config.json";

    if (!std::filesystem::exists(applicationSupportFile))
    {
        logError("File with keys was not found");
        throw std::runtime_error("File with keys was not found");
    }

    if (!acquireKeys(applicationSupportFile))
    {
        logFailure("Keys acquisition failed");
        throw std::runtime_error("Missing configuration data. Can not access account information.");
    }
    m_spot = std::make_unique<BINAPI_NAMESPACE::BinanceAPISpot>(&m_binanceKeys, &m_binanceLimits);

    logSuccess("Keys acquire successfully");
    localSend<CentaurConnectionMessage>();
}

btrader::TraderApplication::~TraderApplication()
{
    // Release memory
    delete g_futuresLogger;
}

auto btrader::TraderApplication::acquire(const messages &msg) noexcept -> void
{
    if (getMessage<CentaurConnectionMessage>(msg) != std::nullopt)
    {
        onConnectCentaur();
    }
    else if (getMessage<CentaurConnectedMessage>(msg) != std::nullopt)
    {
        if (m_centaurServer.m_reconnection.isValid())
        {
            m_centaurServer.m_reconnection.thisObject()->terminate();
            logInfo("The reconnection timer is stopped");
        }
    }
    else if (getMessage<FailedToConnect>(msg) != std::nullopt)
    {
        logInfo("The client will attempt to connect every minute");
        onReconnectCentaur();
    }
    else if (getMessage<ClosedConnection>(msg) != std::nullopt)
    {
        logInfo("The client will attempt to connect every minute");
        onReconnectCentaur();
    }

    else if (getMessage<TimeoutMessage>(msg) != std::nullopt)
    {
        onTimeout();
    }
    else if (getMessage<TickerUpdate>(msg) != std::nullopt)
    {
        updateTotalAsset(std::get<TickerUpdate>(msg).asset, std::get<TickerUpdate>(msg).value);
    }
}

auto btrader::TraderApplication::onConnectCentaur() noexcept -> void
{
    m_centaurServer.m_timeout.startAndRelease(this);
    m_centaurServer.m_centaurClient.startAndRelease(this, "localhost", "", 80);
}

auto btrader::TraderApplication::onSetCentaurConnection() noexcept -> void
{
    m_centaurServer.m_centaurConnected = true;
}

auto btrader::TraderApplication::onReconnectCentaur() noexcept -> void
{
    m_tickerStreams.release();
    m_centaurServer.m_timeout.release();
    m_centaurServer.m_reconnection.startAndReleaseWithPromise(this);
    logSuccess("The reconnection timer is set");
}

auto btrader::TraderApplication::onTerminate() noexcept -> void
{
    m_tickerStreams.release();
    m_centaurServer.m_reconnection.release();
    m_centaurServer.m_centaurClient.release();

    // Terminate logger
    g_futuresLogger->terminate();
    if (m_loggerThread.joinable())
        m_loggerThread.join();
}

auto btrader::TraderApplication::onAcquireFirstBalances() noexcept -> bool
{
    try
    {
        auto data = m_spot->getAllCoinsInformation();

        // Send the icons
        m_centaurServer.m_centaurClient.thisObject()->sendIcon(m_centaurServer.m_centaurIcons.ipoIconId, icons::ipoIcon, CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_SVG);
        m_centaurServer.m_centaurClient.thisObject()->sendIcon(m_centaurServer.m_centaurIcons.moneyId, icons::money, CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_SVG);
        m_centaurServer.m_centaurClient.thisObject()->sendIcon(m_centaurServer.m_centaurIcons.moneyOffId, icons::moneyOff, CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_SVG);
        m_centaurServer.m_centaurClient.thisObject()->sendIcon(m_centaurServer.m_centaurIcons.storageId, icons::storage, CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_SVG);
        m_centaurServer.m_centaurClient.thisObject()->sendIcon(m_centaurServer.m_centaurIcons.withdrawId, icons::withdraw, CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_SVG);
        m_centaurServer.m_centaurClient.thisObject()->sendIcon(m_centaurServer.m_centaurIcons.binanceId, icons::binance, CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_Icon::IF_PNG);

        for (auto &[sym, info] : data)
        {
            if (info.free > 0 || info.freeze > 0 || info.locked > 0 || info.ipoable > 0 || info.ipoing > 0 || info.storage > 0 || info.withdrawing > 0)
            {
                std::string name        = fmt::format("{} ({})", info.name, sym);

                std::string free        = fmt::format("$ {:.8f}", info.free);
                std::string freeze      = fmt::format("$ {:.8f}", info.freeze);
                std::string locked      = fmt::format("$ {:.8f}", info.locked);
                std::string ipoable     = fmt::format("$ {:.8f}", info.ipoable);
                std::string ipoing      = fmt::format("$ {:.8f}", info.ipoing);
                std::string storage     = fmt::format("$ {:.8f}", info.storage);
                std::string withdrawing = fmt::format("$ {:.8f}", info.withdrawing);

                std::string symUpper { sym };
                std::transform(symUpper.begin(), symUpper.end(), symUpper.begin(), ::toupper);

                auto id = m_centaurServer.m_centaurClient.thisObject()->sendBalanceAsset(name, ".", symUpper);

                if (!id.empty())
                {
                    auto subId = m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(id, "Binance SPOT", "", m_centaurServer.m_centaurIcons.binanceId);
                    if (!subId.empty())
                    {
                        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(subId, "Free", free, m_centaurServer.m_centaurIcons.moneyId);
                        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(subId, "Locked", locked, m_centaurServer.m_centaurIcons.moneyOffId);
                        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(subId, "Freeze", freeze, m_centaurServer.m_centaurIcons.moneyOffId);
                        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(subId, "IPOable", ipoable, m_centaurServer.m_centaurIcons.ipoIconId);
                        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(subId, "IPOing", ipoing, m_centaurServer.m_centaurIcons.ipoIconId);
                        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(subId, "Storage", storage, m_centaurServer.m_centaurIcons.storageId);
                        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetItem(subId, "Withdrawing", withdrawing, m_centaurServer.m_centaurIcons.withdrawId);

                        m_assetBalances[symUpper]            = info.free + info.freeze + info.locked + info.ipoable + info.ipoing + info.storage + info.withdrawing;
                        m_centaurServer.m_assetIds[symUpper] = id;

                        updateBalanceTicker(symUpper);
                    }
                }
            }
        }

    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        CATCH_API_EXCEPTION()
        terminate();
        return false;
    }
    return true;
}

auto btrader::TraderApplication::addResponse(btrader::TimeoutType type) noexcept -> void
{
    m_centaurServer.m_responseQueue.push(type);
}

auto btrader::TraderApplication::onTimeout() noexcept -> void
{
    if (m_centaurServer.m_responseQueue.empty())
        return;

    logInfo("A timeout message was received");

    auto data = m_centaurServer.m_responseQueue.get();

    switch (data)
    {
        case TimeoutType::acceptConnection:
            {
                if (!m_centaurServer.m_centaurConnected)
                {
                    logTimeout("Clien was not connected to the Centaur server. The connection will be stopped and reattempted");
                    m_centaurServer.m_centaurClient.thisObject()->terminate();
                }
            }
            break;
    }
}

auto btrader::TraderApplication::updateBalanceTicker(const std::string &asset) noexcept -> void
{
    if (asset == "USDT")
    {
        // Send directly
        m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetTotalUpdate(m_centaurServer.m_assetIds[asset], m_assetBalances[asset]);
    }
    else
    {
        if (!m_tickerStreams.isValid())
        {
            // Must start the thread. Create the asset as ...usdt; for example, for btc would be, btcusdt
            m_tickerStreams.startAndReleaseWithPromise(this, asset, "usdt");
            logInfo("SPOT Streams thread was created");
        }
        else
        {
            // Just push a new stream
            m_tickerStreams.thisObject()->subscribeIndividualMiniTicker(asset + "usdt");
        }
    }
}

auto btrader::TraderApplication::updateTotalAsset(const std::string &asset, double value) noexcept -> void
{
    // Get the id
    auto &id   = m_centaurServer.m_assetIds[asset];
    auto total = m_assetBalances[asset] * value;

    m_centaurServer.m_centaurClient.thisObject()->sendBalanceAssetTotalUpdate(id, total);
}
