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
#include <future>
#include <memory>
#include <thread>
#include <unordered_map>

namespace CENTAUR_NAMESPACE
{
    class SpotMarketWS;
    class BinanceSpotPlugin : public QObject,
                              public CENTAUR_PLUGIN_NAMESPACE::IExchange
    {

        Q_OBJECT

        Q_PLUGIN_METADATA(IID "com.centaur-project.plugin.BinanceSpotPlugin/1.0")
        Q_INTERFACES(CENTAUR_PLUGIN_NAMESPACE::IBase CENTAUR_PLUGIN_NAMESPACE::IExchange)

    public:
        explicit BinanceSpotPlugin(QObject *parent = nullptr);
        ~BinanceSpotPlugin() override;

    protected:
        void runMarketWS(const QString &symbol) noexcept;

        // IBase
    public:
        QObject *getPluginObject() noexcept override;
        void setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, CENTAUR_INTERFACE_NAMESPACE::ILongOperation *lOper) noexcept override;
        QString getPluginName() noexcept override;
        QString getPluginVersionString() noexcept override;
        uuid getPluginUUID() noexcept override;
        bool addMenuAction(QAction *action, const uuid &menuId) noexcept override;

        // IExchange
    public:
        bool initialization() noexcept override;
        CENTAUR_PLUGIN_NAMESPACE::StringIcon getSymbolListName() const noexcept override;
        CENTAUR_PLUGIN_NAMESPACE::StringIconVector getSymbolList() const noexcept override;
        bool addSymbol(const QString &name, int item) noexcept override;
        void removeSymbol(const QString &name) noexcept override;
        void updateOrderbook(const QString &symbol) noexcept override;
        void stopOrderbook(const QString &symbol) noexcept override;
        QString getQuoteFromSymbol(const QString &symbol) noexcept override;
        QString getBaseFromSymbol(const QString &symbol) noexcept override;
        QList<QPair<QString, uuid>> dynamicWatchListMenuItems() noexcept override;
        bool setDynamicWatchListMenuAction(QAction *action, const QString &symbolName, const uuid &id) noexcept override;

    public slots:
        void onTickerUpdate(const QString &symbol, quint64 receivedTime, double price) noexcept;
        void onSubscription(bool subscribe, bool status, int id) noexcept;
        void onDepthUpdate(const QString &symbol, quint64 eventTime, const binapi::StreamDepthUpdate &sdp) noexcept;
        void onSpotStatus() noexcept;
        void onCoinInformation() noexcept;
        void onSpotDepositHistory() noexcept;
        void onSpotWithdrawHistory() noexcept;
        void onGetAllOrders() noexcept;
        void onAccountTradeList() noexcept;
        void onDisplayBaseAssetInfo(const QString &asset) noexcept;
        void onDisplayCoinAssetDepositAddress(const QString &asset) noexcept;
        void onDisplayAssetDetail(const QString &asset) noexcept;
        void onShowFees(const QString &symbol = "") noexcept;

    signals:
        void snTickerUpdate(const QString &symbol, int symbolId, quint64 receivedTime, double price);
        void snOrderbookUpdate(const QString &source, const QString &symbol, quint64 receivedTime, const QMap<qreal, QPair<qreal, qreal>> &bids, const QMap<qreal, QPair<qreal, qreal>> &asks);

        // Resources
    protected:
        mutable QIcon m_exchIcon { ":/img/plugin" };
        QIcon m_cryptoIcon { ":/img/crypto" };

    private:
        CENTAUR_INTERFACE_NAMESPACE::ILogger *m_logger { nullptr };
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config { nullptr };
        CENTAUR_INTERFACE_NAMESPACE::ILongOperation *m_longOperation { nullptr };
        std::unique_ptr<SpotMarketWS> m_spotWS { nullptr };
        std::unique_ptr<std::thread> m_spotWSThread { nullptr };

    protected:
        CENTAUR_PLUGIN_NAMESPACE::StringIconVector m_symbols;

    private:
        std::unique_ptr<BINAPI_NAMESPACE::BinanceAPISpot> m_api { nullptr };
        BINAPI_NAMESPACE::SPOT::ExchangeInformation m_exchInfo;
        BINAPI_NAMESPACE::BinanceLimits m_limits;
        BINAPI_NAMESPACE::BinanceKeys m_keys;

    protected:
        uuid m_globalPluginUuid;
        std::map<QString, int> m_symbolId;
        std::map<int, QString> m_wsIds;
        std::unordered_map<QString, std::pair<bool, uint64_t>> m_symbolOrderbookSnapshot;

    protected:
        BINAPI_NAMESPACE::AllCoinsInformation m_coinInformation;
        BINAPI_NAMESPACE::SpotTradingFees m_fees;
    };

    class SpotMarketWS : public BINAPI_NAMESPACE::ws::WSSpotBinanceAPI
    {
    public:
        SpotMarketWS(std::promise<void> connected);
        ~SpotMarketWS() override;

    public:
        void initialize(BinanceSpotPlugin *obj, CENTAUR_INTERFACE_NAMESPACE::ILogger *logger);
        void connected() override;
        void close() override;
        void connectionError() override;
        void pingSent(bool success) noexcept override;

    protected:
        void subscribe(bool status, int id) override;
        void unsubscribe(bool status, int id) override;
        void individualSymbolMiniTicker(const std::string &symbol, uint64_t eventTime, const BINAPI_NAMESPACE::StreamIndividualSymbolMiniTicker &ticker) override;
        void depthUpdate(const std::string &symbol, uint64_t eventTime, const BINAPI_NAMESPACE::StreamDepthUpdate &sdp) override;

    private:
        CENTAUR_INTERFACE_NAMESPACE::ILogger *m_logger { nullptr };
        BinanceSpotPlugin *m_obj { nullptr };
        std::promise<void> m_connected;
    };

} // namespace CENTAUR_NAMESPACE

Q_DECLARE_METATYPE(BINAPI_NAMESPACE::StreamDepthUpdate)

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
