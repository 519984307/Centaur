/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include <QObject>

#include "AssetDetailDialog.hpp"
#include "BinanceSPOT.hpp"
#include "CoinInfoDialog.hpp"
#include "FeesDialog.hpp"
#include "NetworkAddressDialog.hpp"
#include "Protocol.hpp"
#include <QApplication>
#include <QMessageBox>
#include <fmt/core.h>

namespace
{
    /// UUID V5 string "CentaurProject-BinanceExchangeSPOT-0.1.0"
    constexpr char g_BinanceSpotName[]          = "BinanceSPOT";
    constexpr char g_BinanceSpotVersionString[] = "0.1.0";
    constexpr char g_uuidString[]               = "{85261bc6-8f92-57ca-802b-f08b819031db}";
} // namespace

#define CATCH_API_EXCEPTION()                                                                                                                                              \
    switch (ex.type())                                                                                                                                                     \
    {                                                                                                                                                                      \
        case BINAPI_NAMESPACE::APIException::Type::request:                                                                                                                \
            {                                                                                                                                                              \
                auto request = ex.request();                                                                                                                               \
                logError("BinanceSPOTPlugin", fmt::format("REQUEST.\nCode: {}. URL: {}\n{}\n", std::get<0>(request), std::get<1>(request), std::get<2>(request)).c_str()); \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::limits:                                                                                                                 \
            logError("BinanceSPOTPlugin", fmt::format("LIMITS.\n{}\n", std::get<0>(ex.limits())).c_str());                                                                 \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::http1:                                                                                                                  \
            {                                                                                                                                                              \
                auto http1 = ex.http1();                                                                                                                                   \
                logError("BinanceSPOTPlugin", fmt::format("HTTPa.\nCode: {}\nURL: {}\n{}\n", std::get<0>(http1), std::get<1>(http1), std::get<2>(http1)).c_str());         \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::http2:                                                                                                                  \
            {                                                                                                                                                              \
                auto http2 = ex.http2();                                                                                                                                   \
                logError("BinanceSPOTPlugin", fmt::format("HTTPb.\nStatus: {}. Code: {}\nURL: {}.\n{}\n",                                                                  \
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
                logError("BinanceSPOTPlugin", fmt::format("JSON.\nURL: {}\n{}\n", std::get<0>(json), std::get<1>(json)).c_str());                                          \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::api:                                                                                                                    \
            {                                                                                                                                                              \
                auto api1 = ex.api();                                                                                                                                      \
                logError("BinanceSPOTPlugin", fmt::format("API.\nCode: {}\nURL: {}\n{}", std::get<0>(api1), std::get<1>(api1), std::get<2>(api1)).c_str());                \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
        case BINAPI_NAMESPACE::APIException::Type::schema:                                                                                                                 \
            {                                                                                                                                                              \
                auto schema = ex.schema();                                                                                                                                 \
                logError("BinanceSPOTPlugin", fmt::format("SCHEMA.\nURL: {}\n{}\n", std::get<0>(schema), std::get<1>(schema)).c_str());                                    \
            }                                                                                                                                                              \
            break;                                                                                                                                                         \
    }

CENTAUR_NAMESPACE::BinanceSpotPlugin::BinanceSpotPlugin(QObject *parent) :
    QObject(parent),
    m_spotWS { nullptr },
    m_globalPluginUuid { g_uuidString }
{
    m_sevenDayLastUpdate = QDate::currentDate();
}

CENTAUR_NAMESPACE::BinanceSpotPlugin::~BinanceSpotPlugin()
{
    if (m_spotWSThread != nullptr && m_spotWSThread->joinable())
    {
        m_spotWS->terminate();
        m_spotWSThread->join();
    }
    m_spotWSThread.reset();
    m_spotWS.reset();
}

QObject *CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginObject() noexcept
{
    return qobject_cast<QObject *>(this);
}

QString CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginVersionString() noexcept
{
    return g_BinanceSpotVersionString;
}

QString CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginName() noexcept
{
    return g_BinanceSpotName;
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config) noexcept
{
    m_logger = logger;
    m_config = config;
}

CENTAUR_NAMESPACE::uuid CENTAUR_NAMESPACE::BinanceSpotPlugin::getPluginUUID() noexcept
{
    return m_globalPluginUuid;
}
/*
bool CENTAUR_NAMESPACE::BinanceSpotPlugin::addMenuAction(QAction *action, const uuid &menuId) noexcept
{
    if (menuId == uuid { "{5445eaec-d22c-4204-b720-ab07a570ab2e}" })
    {
        connect(action, &QAction::triggered, this, &BinanceSpotPlugin::onSpotStatus);
        return true;
    }
    else if (menuId == uuid { "{394f3857-c797-49c0-9343-37e1e66e028a}" })
    {
        connect(action, &QAction::triggered, this, &BinanceSpotPlugin::onCoinInformation);
        return true;
    }
    else if (menuId == uuid { "{3cd3662b-a52f-4c3d-ab15-bde1c1962cb9}" })
    {
        connect(action, &QAction::triggered, this, [&]() { onShowFees(); });
        return true;
    }

    return false;
}*/

bool CENTAUR_NAMESPACE::BinanceSpotPlugin::initialization() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::initialization");

    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;
    auto publicKeyFile = m_config->getPluginPublicKeyPath();

    try
    {
        ec.loadPublicKey(publicKeyFile);
    } catch (const std::exception &ex)
    {
        logError("BinanceSpotPlugin", QString("Could not load the plugin public key. %1").arg(ex.what()));
        //  return false;
    }
    /*
        bool apiError, secError;
        const auto apiKeyCip = m_config->getValue("apiKey", &apiError);
        const auto secKeyCip = m_config->getValue("secretKey", &secError);*/
    /*
        if (!apiError || !secError)
        {
            logError("BinanceSpotPlugin", "Either the API Key or the secret key are empty on the plugins configuration file.");
            return false;
        }

        const auto apiKey = ec.decryptPublic(apiKeyCip, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64);
        const auto secKey = ec.decryptPublic(secKeyCip, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64);

        if (apiKey.empty() || secKey.empty())
        {
            logError("BinanceSpotPlugin", "Could not decrypt the binance keys");
            return false;
        }

        // set the keys
        m_keys.apiKey    = apiKey;
        m_keys.secretKey = secKey;
    */
    m_api = std::make_unique<BINAPI_NAMESPACE::BinanceAPISpot>(&m_keys, &m_limits);

    try
    {

        m_api->ping();
        logInfo("BinanceSpotPlugin", "Binance Server SPOT ping correctly");
        if (!m_api->getExchangeStatus())
        {
            logError("BinanceSpotPlugin", "Binance Server is under maintenance");
            return false;
        }

        logInfo("BinanceSpotPlugin", "Acquiring Binance SPOT Exchange Information");
        m_exchInfo = m_api->getExchangeInformation();
        logInfo("BinanceSpotPlugin", QString("Binance SPOT Exchange Information Acquired in ##00FFFF#%1# secs").arg(m_api->getLastCallTime()));

        m_limits.setAPIRequestsLimits(m_exchInfo.limitWeight.limit, m_exchInfo.limitWeight.seconds);
        m_limits.setAPIOrderLimitLow(m_exchInfo.limitOrderSecond.limit, m_exchInfo.limitOrderSecond.seconds);
        m_limits.setAPIOrderLimitHigh(m_exchInfo.limitOrderDay.limit, m_exchInfo.limitOrderDay.seconds);

        logInfo("BinanceSpotPlugin", "Binance SPOT limits changed.");

        for (const auto &[sym, info] : m_exchInfo.symbols)
        {
            if (info.permissions.test(BINAPI_NAMESPACE::SPOT::AccountPermissions::spot))
                m_symbols.push_back({ QString { info.symbol.c_str() }, &m_cryptoIcon });
        }
        logInfo("BinanceSpotPlugin", QString("Symbols handled: ##00FFFF#%1#").arg(m_symbols.size()));
    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        CATCH_API_EXCEPTION()
        return false;
    }
    return true;
}

CENTAUR_PLUGIN_NAMESPACE::StringIcon CENTAUR_NAMESPACE::BinanceSpotPlugin::getSymbolListName() const noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::getSymbolListName");

    return { QString { g_BinanceSpotName }, &m_exchIcon };
}

CENTAUR_PLUGIN_NAMESPACE::StringIconVector CENTAUR_NAMESPACE::BinanceSpotPlugin::getSymbolList() const noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::getSymbolList");

    return m_symbols;
}

QString CENTAUR_NAMESPACE::BinanceSpotPlugin::getQuoteFromSymbol(const QString &symbol) noexcept
{
    return m_exchInfo.symbols[symbol.toStdString()].quoteAsset.c_str();
}

QString CENTAUR_NAMESPACE::BinanceSpotPlugin::getBaseFromSymbol(const QString &symbol) noexcept
{
    return m_exchInfo.symbols[symbol.toStdString()].baseAsset.c_str();
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::runMarketWS(const QString &symbol) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::runMarketWS");

    if (m_spotWSThread && m_spotWSThread->joinable())
    {
        m_spotWS->terminate();
        m_spotWSThread->join();
    }
    m_spotWSThread.reset();
    m_spotWS.reset();

    std::promise<void> connected;
    std::future<void> future = connected.get_future();

    m_spotWS = std::make_unique<SpotMarketWS>(std::move(connected));
    m_spotWS->initialize(this, m_logger);

    auto ticker          = m_spotWS->subscribeIndividualMiniTicker(symbol.toStdString());
    const std::string ep = m_spotWS->constructEndPoint(std::get<std::string>(ticker));

    m_spotWS->setEndPoint(ep);

    m_spotWSThread = std::make_unique<std::thread>([&] {
        logInfo("BinanceSpotPlugin", "Spot WebSocket thread created.");
        m_spotWS->run();
    });

    future.wait();

    logInfo("BinanceSpotPlugin", "The main thread is unblocked");
}

bool CENTAUR_NAMESPACE::BinanceSpotPlugin::addSymbolToWatchlist(const QString &name) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::addSymbolToWatchlist()");

    auto itemId = m_symbolsWatch.find(name);
    if (itemId != m_symbolsWatch.end())
    {
        logError("BinanceSpotPlugin", QString("The %1 symbol is already handled").arg(name));
        return false;
    }

    logInfo("BinanceSpotPlugin", QString("Attempting to add %1 to the watchlist").arg(name));

    if (!m_spotWSThread || !m_spotWS) // The WS is not running
    {
        logInfo("BinanceSpotPlugin", "Adding the symbol will attempt to run a websocket.");
        // Start the thread and specify the new data
        runMarketWS(name);
    }
    else
    {
        // Subscribe the method
        logInfo("BinanceSpotPlugin", QString("Attempting to add %1 to the watchlist").arg(name));
        auto subsVar = m_spotWS->subscribeIndividualMiniTicker(name.toStdString());
        if (std::holds_alternative<int>(subsVar))
            m_wsIds[std::get<int>(subsVar)] = name;
    }

    m_symbolsWatch.insert(name);

    // set the seven 7 data to cache
    get7dayData(name);

    // According to the API, there is no way to see if the stream was successfully retrieved in this part of the code
    // since the stream receiving is asynchronous
    // So we'll handle the symbol return true even if there is a slight chance that this will not happen
    return true;
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::removeSymbolFromWatchlist(const QString &name) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::removeSymbolFromWatchlist()");

    logInfo("BinanceSpotPlugin", QString("Attempting to remove %1 from the watchlist").arg(name));

    // Remove from the id's list
    m_symbolsWatch.erase(name);
    // remove the seven-day cache
    m_sevenDayCache.remove(name);

    if (m_symbolsWatch.empty())
    {
        /// if the list is empty prefer to stop the thread
        logWarn("BinanceSpotPlugin", "The Spot WebSocket thread will be stopped because the watchlist is empty.");

        // Stop the thread instead
        if (m_spotWSThread != nullptr && m_spotWSThread->joinable())
        {
            m_spotWS->terminate();
            m_spotWSThread->join();
        }
        m_spotWSThread.reset();
        m_spotWS.reset();
        logWarn("BinanceSpotPlugin", "The Spot WebSocket thread stopped.");
    }
    else
    {
        // Stop receiving data
        int id      = m_spotWS->unsubscribeIndividualMiniTicker(name.toStdString());
        m_wsIds[id] = name;
    }
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onTickerUpdate(const QString &symbol, quint64 receivedTime, double price) noexcept
{
    // Don't log since this message will be sent every 500 ms

    // Redirect the signal
    emit snTickerUpdate(symbol, g_uuidString, receivedTime, price);
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSubscription(bool subscribe, bool status, int id) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSubscription()");

    if (auto idIter = m_wsIds.find(id); idIter == m_wsIds.end())
    {
        logError("BinanceSpotPlugin", "A unknown (un)subscribe message was received.");
        return;
    }
    else
    {

        auto &name = idIter->second;

        if (!status)
            logWarn("BinanceSpotPlugin", QString("%1 for %2 with id: %3 failed").arg(subscribe ? "Subscription" : "Unsubscription", name).arg(id));
        else
            logInfo("BinanceSpotPlugin", QString("%1 for %2 with id: %3 succeed").arg(subscribe ? "Subscription" : "Unsubscription", name).arg(id));

        m_wsIds.erase(id);
    }
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::updateOrderbook(const QString &symbol) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotSymbolListPlugin::updateOrderbook()");

    auto itemId = m_symbolsWatch.find(symbol);
    if (itemId == m_symbolsWatch.end())
    {
        logError("BinanceSpotPlugin", QString("The %1 symbol is not handled and the orderbook can not be send").arg(symbol));
        return;
    }

    logInfo("BinanceSpotPlugin", QString("Attempting to subscribe Orderbook event for %1").arg(symbol));

    auto subsVar = m_spotWS->subscribeDiffBookDepth(symbol.toStdString(), 1000);

    if (std::holds_alternative<int>(subsVar))
    {
        int &id     = std::get<int>(subsVar);
        m_wsIds[id] = symbol;
        logInfo("BinanceSpotPlugin", QString("Orderbook subscription event id for %1 is %2").arg(symbol).arg(id));
    }

    m_symbolOrderbookSnapshot[symbol] = { false, 0 };
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::stopOrderbook(const QString &symbol) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotSymbolListPlugin::stopOrderbook()");
    auto itemId = m_symbolsWatch.find(symbol);
    if (itemId == m_symbolsWatch.end())
    {
        logError("BinanceSpotPlugin", QString("The %1 symbol is not handled and the orderbook can not be stopped").arg(symbol));
        return;
    }

    logInfo("BinanceSpotPlugin", QString("Attempting to unsubscribe Orderbook event for %1").arg(symbol));

    int id = m_spotWS->unsubscribeDiffBookDepth(symbol.toStdString(), 1000);
    logInfo("BinanceSpotPlugin", QString("Orderbook unsubscribe event id for %1 is %2").arg(symbol).arg(id));
    m_wsIds[id] = symbol;

    m_symbolOrderbookSnapshot.erase(symbol);
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onDepthUpdate(const QString &symbol, quint64 eventTime, const BINAPI_NAMESPACE::StreamDepthUpdate &sdp) noexcept
{
    /*
    Open a stream to wss://stream.binance.com:9443/ws/bnbbtc@@depth.
    Buffer the events you receive from the stream.
    Get a depth snapshot from https://api.binance.com/api/v3/depth?symbol=BNBBTC&limit=1000 .
    Drop any event where u is <= lastUpdateId in the snapshot.
    The first processed event should have U <= lastUpdateId+1 AND u >= lastUpdateId+1.
    While listening to the stream, each new event's U should be equal to the previous event's u+1.
    The data in each event is the absolute quantity for a price level.
    If the quantity is 0, remove the price level.
    Receiving an event that removes a price level that is not in your local order book can happen and is normal.
    */

    const auto &[snap, lastUpdate] = m_symbolOrderbookSnapshot[symbol];
    uint64_t lastUpdateId          = lastUpdate;
    if (!snap)
    {
        // Take the snapshot

        BINAPI_NAMESPACE::BinanceAPISpot api(nullptr, nullptr);

        try
        {
            auto orderbook                    = m_api->getOrderBook(symbol.toStdString(), 1000);
            m_symbolOrderbookSnapshot[symbol] = { true, lastUpdateId };
            lastUpdateId                      = orderbook.lastUpdateId;
            logInfo("BinanceSpotPlugin", QString("Orderbook snapshot successfully taken for %1").arg(symbol));
        } catch (const BINAPI_NAMESPACE::APIException &ex)
        {
            CATCH_API_EXCEPTION()
            return;
        }
    }

    if (sdp.finalUpdateId <= lastUpdateId)
    {
        logWarn("BinanceSpotPlugin", QString("Orderbook event ignored for %1").arg(symbol));
        return;
    }

    QMap<qreal, QPair<qreal, qreal>> bids, asks;

    for (const auto &[price, quantity] : sdp.bids)
    {
        if (quantity > 0.0)
        {
            const double total = price * quantity;

            bids[price] = { quantity, total };
        }
    }

    for (const auto &[price, quantity] : sdp.asks)
    {
        if (quantity > 0.0)
        {
            const double total = price * quantity;

            asks[price] = { quantity, total };
        }
    }

    emit snOrderbookUpdate(g_uuidString, symbol, eventTime, bids, asks);

    m_symbolOrderbookSnapshot[symbol] = { true, sdp.finalUpdateId };
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSpotStatus() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSpotStatus");
    QString message;
    try
    {
        if (m_api->getExchangeStatus())
            message = "System status: Normal";
        else
            message = "System status: System maintenance";
    } catch (C_UNUSED const std::exception &ex)
    {
        message = "System status: API Error";
    }

    QMessageBox::information(nullptr, "System status", message, QMessageBox::StandardButton::Ok);
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onCoinInformation() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onCoinInformation");
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    if (m_coinInformation.empty())
    {
        try
        {
            m_coinInformation = m_api->getAllCoinsInformation();
        } catch (const BINAPI_NAMESPACE::APIException &ex)
        {
            CATCH_API_EXCEPTION()
        }
    }
    QApplication::restoreOverrideCursor();

    if (!m_coinInformation.empty())
    {
        CoinInfoDialog dlg(&m_coinInformation, m_config);
        dlg.setModal(true);
        dlg.exec();
    }
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSpotDepositHistory() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSpotDepositHistory");
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onSpotWithdrawHistory() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onSpotWithdrawHistory");
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onGetAllOrders() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onGetAllOrders");
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onAccountTradeList() noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onAccountTradeList");
}

QList<QAction *> CENTAUR_NAMESPACE::BinanceSpotPlugin::dynamicWatchListMenuItems() noexcept
{
    auto *baseAsset         = new QAction("Get Base Asset Coin information", this);
    auto *quoteAsset        = new QAction("Get Quote Asset Coin information", this);
    auto *separator0        = new QAction;
    auto *baseAssetDeposit  = new QAction("Get Base Asset deposit address", this);
    auto *quoteAssetDeposit = new QAction("Get Quote Asset deposit address", this);
    auto *separator1        = new QAction;
    auto *baseAssetDetails  = new QAction("Get Base Asset Details", this);
    auto *quoteAssetDetails = new QAction("Get Quote Asset Details", this);
    auto *separator3        = new QAction;
    auto *symbolFees        = new QAction("Get Symbol Fees", this);

    connect(baseAsset, &QAction::triggered, this, [&, baseAsset](C_UNUSED bool checked = false) {
        onDisplayBaseAssetInfo(baseAsset->data().toString());
    });

    connect(quoteAsset, &QAction::triggered, this, [&, quoteAsset](C_UNUSED bool checked = false) {
        onDisplayBaseAssetInfo(quoteAsset->data().toString());
    });

    connect(baseAssetDeposit, &QAction::triggered, this, [&, baseAssetDeposit](C_UNUSED bool checked = false) {
        onDisplayCoinAssetDepositAddress(baseAssetDeposit->data().toString());
    });

    connect(quoteAssetDeposit, &QAction::triggered, this, [&, quoteAssetDeposit](C_UNUSED bool checked = false) {
        onDisplayCoinAssetDepositAddress(quoteAssetDeposit->data().toString());
    });

    connect(baseAssetDetails, &QAction::triggered, this, [&, baseAssetDetails](C_UNUSED bool checked = false) {
        onDisplayAssetDetail(baseAssetDetails->data().toString());
    });

    connect(quoteAssetDetails, &QAction::triggered, this, [&, quoteAssetDetails](C_UNUSED bool checked = false) {
        onDisplayAssetDetail(quoteAssetDetails->data().toString());
    });

    connect(symbolFees, &QAction::triggered, this, [&, symbolFees](C_UNUSED bool checked = false) {
        onShowFees(symbolFees->data().toString());
    });

    return {
        baseAsset,
        quoteAsset,
        separator0,
        baseAssetDeposit,
        quoteAssetDeposit,
        separator1,
        baseAssetDetails,
        quoteAssetDetails,
        separator3,
        symbolFees,
    };
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onDisplayBaseAssetInfo(const QString &asset) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onDisplayBaseAssetInfo");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (m_coinInformation.empty())
    {
        try
        {
            m_coinInformation = m_api->getAllCoinsInformation();
        } catch (const BINAPI_NAMESPACE::APIException &ex)
        {
            CATCH_API_EXCEPTION()
        }
    }
    QApplication::restoreOverrideCursor();

    if (!m_coinInformation.empty())
    {
        CoinInfoDialog dlg(&m_coinInformation, m_config, asset.toUpper());
        dlg.setModal(true);
        dlg.exec();
    }
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onDisplayCoinAssetDepositAddress(const QString &asset) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onDisplayCoinAssetDepositAddress");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (m_coinInformation.empty())
    {
        try
        {
            m_coinInformation = m_api->getAllCoinsInformation();
        } catch (const BINAPI_NAMESPACE::APIException &ex)
        {
            CATCH_API_EXCEPTION()
        }
    }
    QApplication::restoreOverrideCursor();

    NetworkAddressDialog dlg(m_api.get(), &m_coinInformation, m_config, asset);
    dlg.setModal(true);
    dlg.exec();
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onDisplayAssetDetail(const QString &asset) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onDisplayAssetDetail");

    try
    {
        const auto assetDetail = m_api->assetDetail(asset.toStdString());
        AssetDetailDialog dlg(assetDetail, asset, m_config, nullptr);
        dlg.setModal(true);
        dlg.exec();
    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        CATCH_API_EXCEPTION()
    }
}

void CENTAUR_NAMESPACE::BinanceSpotPlugin::onShowFees(const QString &symbol) noexcept
{
    logTrace("BinanceSpotPlugin", "BinanceSpotPlugin::onShowFees");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (m_fees.empty())
    {
        try
        {
            m_fees = m_api->tradeFee("" /*Retrieve all*/);
        } catch (const BINAPI_NAMESPACE::APIException &ex)
        {
            CATCH_API_EXCEPTION()
        }
    }

    QApplication::restoreOverrideCursor();

    TradeFeesDialog dlg(m_fees, symbol, nullptr);
    dlg.setModal(true);
    dlg.exec();
}

QList<std::tuple<qreal, qreal, QString>> CENTAUR_NAMESPACE::BinanceSpotPlugin::getWatchlist24hrPriceChange() noexcept
try
{
    QList<std::tuple<qreal, qreal, QString>> ret;
    std::vector<std::string> list;
    list.reserve(m_symbolsWatch.size());
    for (const auto &id : m_symbolsWatch)
        list.emplace_back(id.toStdString());

    auto data = m_api->tickerPriceChangeStatistics24hr(list);

    for (const auto &[sym, data] : data)
        ret.emplace_back(data.lastPrice, data.priceChangePercent, QString::fromStdString(sym));

    return ret;
} catch (const BINAPI_NAMESPACE::APIException &ex)
{
    CATCH_API_EXCEPTION()
    return {};
}

QList<std::pair<quint64, qreal>> cen::BinanceSpotPlugin::get7dayData(const QString &symbol) noexcept
{
    QDate thisDate = QDate::currentDate();

    if (thisDate.day() != m_sevenDayLastUpdate.day())
    {
        // Invalidate caches
        m_sevenDayCache.clear();
        m_sevenDayLastUpdate = thisDate;
    }

    auto cache = m_sevenDayCache.find(symbol);

    if (cache != m_sevenDayCache.end())
        return cache.value();

    const auto dayMS   = binapi::BinanceAPI::fromIntervalToMilliseconds(binapi::BinanceTimeIntervals::i1d);
    const auto todayMS = binapi::BinanceAPI::getTime();
    const auto today   = (todayMS - (todayMS % dayMS));

    auto data = m_api->candlestickData(symbol.toStdString(),
        binapi::BinanceTimeIntervals::i1d,
        today - dayMS * 7,
        today,
        7);

    QList<std::pair<quint64, qreal>> ret;
    for (const auto &pt : data)
    {
        ret.emplace_back(pt.openTime, pt.close);
    }

    m_sevenDayCache[symbol] = ret;

    return ret;
}
