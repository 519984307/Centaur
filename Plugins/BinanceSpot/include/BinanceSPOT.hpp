/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_BINANCESPOT_HPP
#define CENTAUR_BINANCESPOT_HPP

#include "BinanceAPI.hpp"
#include "WSSpotBinanceAPI.hpp"
#include <CentaurInterface.hpp>
#include <CentaurPlugin.hpp>
#include <QIcon>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QPair>
#include <memory>
#include <thread>
#include <unordered_map>

class SpotMarketWS;
class BinanceSpotPlugin : public QObject,
                          public cent::plugin::IExchange
{

    Q_OBJECT

    Q_PLUGIN_METADATA(IID "com.centaur-project.plugin.BinanceSpotPlugin/1.0")
    Q_INTERFACES(cent::plugin::IBase cent::plugin::IExchange)

public:
    explicit BinanceSpotPlugin(QObject *parent = nullptr);
    ~BinanceSpotPlugin() override;

protected:
    void runMarketWS(const QString &symbol) noexcept;

    // IBase
public:
    QObject *getPluginObject() noexcept override;
    void setPluginInterfaces(cent::interface::ILogger *logger, cent::interface::IConfiguration *config) noexcept override;
    QString getPluginName() noexcept override;
    QString getPluginVersionString() noexcept override;
    cent::plugin::PluginUUID getPluginUUID() noexcept override;
    cent::plugin::FuncPointer connectMenu(const QString &identifier) noexcept override;

    // IExchange
public:
    bool initialization() noexcept override;
    cent::plugin::StringIcon getSymbolListName() const noexcept override;
    cent::plugin::StringIconVector getSymbolList() const noexcept override;
    bool addSymbol(const QString &name, const int &item) noexcept override;
    void removeSymbol(const QString &name) noexcept override;
    void updateOrderbook(const QString &symbol) noexcept override;
    void stopOrderbook(const QString &symbol) noexcept override;

public slots:
    void onTickerUpdate(const QString &symbol, const quint64 &receivedTime, const double &price) noexcept;
    void onSubscription(const bool &subscribe, const bool &status, const int &id) noexcept;
    void onDepthUpdate(const QString &symbol, const quint64 &eventTime, const trader::api::StreamDepthUpdate &sdp) noexcept;
    void onSpotStatus() noexcept;
    void onCoinInformation() noexcept;
    void onSpotDepositHistory() noexcept;
    void onSpotWithdrawHistory() noexcept;
    void onGetAllOrders() noexcept;
    void onAccountTradeList() noexcept;

signals:
    void sgTickerUpdate(const QString &symbol, const int &symbolId, const quint64 &receivedTime, const double &price);
    void sgOrderbookUpdate(const QString &source, const QString &symbol, const quint64 &receivedTime, const QMap<QString, QPair<QString, QString>> &bids, const QMap<QString, QPair<QString, QString>> &asks);

    // Resources
protected:
    mutable QIcon m_exchIcon { ":/img/BinancePlugin.png" };
    QIcon m_cryptoIcon { ":/img/crypto.svg" };

private:
    cent::interface::ILogger *m_logger { nullptr };
    cent::interface::IConfiguration *m_config { nullptr };
    std::unique_ptr<SpotMarketWS> m_spotWS { nullptr };
    std::unique_ptr<std::thread> m_spotWSThread { nullptr };

protected:
    cent::plugin::StringIconVector m_symbols;

private:
    std::unique_ptr<trader::api::BinanceAPI> m_api { nullptr };
    trader::api::ExchangeInformation m_exchInfo;
    trader::api::BinanceLimits m_limits;
    trader::api::BinanceKeys m_keys;

protected:
    cent::plugin::GlobalPluginUUID m_globalPluginUuid;
    std::map<QString, int> m_symbolId;
    std::map<int, QString> m_wsIds;
    std::unordered_map<QString, std::pair<bool, uint64_t>> m_symbolOrderbookSnapshot;
};

class SpotMarketWS : public trader::api::ws::WSSpotBinanceAPI
{
public:
    SpotMarketWS();
    ~SpotMarketWS() override;

public:
    void initialize(BinanceSpotPlugin *obj, cent::interface::ILogger *logger);
    void connected() override;
    void close() override;
    void connectionError() override;

protected:
    void subscribe(const bool &status, const int &id) override;
    void unsubscribe(const bool &status, const int &id) override;
    void individualSymbolMiniTicker(const std::string &symbol, const uint64_t &eventTime, const trader::api::StreamIndividualSymbolMiniTicker &ticker) override;
    void depthUpdate(const std::string &symbol, const uint64_t &eventTime, const trader::api::StreamDepthUpdate &sdp) override;

private:
    cent::interface::ILogger *m_logger { nullptr };
    BinanceSpotPlugin *m_obj { nullptr };
};

Q_DECLARE_METATYPE(trader::api::StreamDepthUpdate)

// Helper macros
#define logInfo(x, y) \
    m_logger->info(x, y)

#define logWarn(x, y) \
    m_logger->warning(x, y)

#if !defined(NDEBUG)
#define logTrace(x, y) \
    m_logger->trace(x, y)
#define logDebug(x, y) \
    m_logger->debug(x, y)
#else
#define logTrace(x, y) \
    ((void *)0)
#define logDebug(x, y) \
    ((void *)0)
#endif /**/

#define logError(x, y) \
    m_logger->error(x, y)

#define logFatal(x, y) \
    m_logger->fatal(x, y)

#endif // CENTAUR_BINANCESPOT_HPP
