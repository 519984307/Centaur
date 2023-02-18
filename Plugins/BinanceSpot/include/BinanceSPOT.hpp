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
#include <QDate>
#include <QIcon>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QPair>
#include <QThread>
#include <future>
#include <memory>
#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace CENTAUR_NAMESPACE
{

    class SpotMarketWS;

    class BinanceSpotPlugin : public QObject,
                              public CENTAUR_PLUGIN_NAMESPACE::IExchange
    {

        Q_OBJECT

        Q_PLUGIN_METADATA(IID "com.centaur-project.plugin.BinanceSpotPlugin/1.0")
        Q_INTERFACES(CENTAUR_PLUGIN_NAMESPACE::IBase CENTAUR_PLUGIN_NAMESPACE::IExchange CENTAUR_PLUGIN_NAMESPACE::IStatus)

    public:
        explicit BinanceSpotPlugin(QObject *parent = nullptr);
        ~BinanceSpotPlugin() override;

    protected:
        void runMarketWS(const QString &symbol) noexcept;

    protected:
        C_NODISCARD QString getUUIDString() const noexcept;

        // IBase
    public:
        QObject *getPluginObject() noexcept override;
        void setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config) noexcept override;
        QString getPluginName() const noexcept override;
        QString getPluginVersionString() const noexcept override;
        uuid getPluginUUID() const noexcept override;

        // IExchange
    public:
        bool initialization() noexcept override;
        CENTAUR_PLUGIN_NAMESPACE::StringIcon getSymbolListName() const noexcept override;
        CENTAUR_PLUGIN_NAMESPACE::StringIconVector getSymbolList() const noexcept override;
        QPair<bool, QPixmap> addSymbolToWatchlist(const QString &name) noexcept override;
        void removeSymbolFromWatchlist(const QString &name) noexcept override;
        void updateOrderbook(const QString &symbol) noexcept override;
        void stopOrderbook(const QString &symbol) noexcept override;
        QString getQuoteFromSymbol(const QString &symbol) noexcept override;
        QString getBaseFromSymbol(const QString &symbol) noexcept override;
        QList<QAction *> dynamicWatchListMenuItems() noexcept override;
        QList<std::tuple<qreal, qreal, QString>> getWatchlist24hrPriceChange() noexcept override;
        QList<std::pair<quint64, qreal>> get7dayData(const QString &symbol) noexcept override;
        QList<CENTAUR_PLUGIN_NAMESPACE::TimeFrame> supportedTimeFrames() noexcept override;
        void acquire(const plugin::PluginInformation &pi, const QString &symbol, plugin::TimeFrame frame, const uuid &id) noexcept override;
        void disengage(const uuid &id, uint64_t lastTimeframeStart, uint64_t lastTimeframeEnd) noexcept override;
        void resetStoredZoom(const uuid &id) noexcept override;
        QList<QPair<Timestamp, CENTAUR_PLUGIN_NAMESPACE::CandleData>> getCandlesByPeriod(const QString &symbol, Timestamp start, Timestamp end, plugin::TimeFrame frame) noexcept override;
        bool realtimePlotAllowed() noexcept override;
        bool dynamicReframePlot() noexcept override;
        void reframe(plugin::TimeFrame frame) noexcept override;

    public:
        DisplayMode initialize() noexcept override;
        QString text() noexcept override;
        QPixmap image() noexcept override;
        QFont font() noexcept override;
        QBrush brush(DisplayRole role) noexcept override;
        QAction *action() noexcept override;

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
        void onStatusButtonClicked(bool status) noexcept;

    public:
        void setExchangeInformation(const BINAPI_NAMESPACE::SPOT::ExchangeInformation &data);

    signals:
        void snTickerUpdate(const QString &symbol, const QString &sourceUUID, quint64 receivedTime, double price);
        void snOrderbookUpdate(const QString &source, const QString &symbol, quint64 receivedTime, const QMap<qreal, QPair<qreal, qreal>> &bids, const QMap<qreal, QPair<qreal, qreal>> &asks);
        void displayChange(plugin::IStatus::DisplayRole dr);

        // Resources
    protected:
        mutable QIcon m_exchIcon { ":/bspot/general/binance" };
        QIcon m_cryptoIcon { ":/bspot/general/crypto_currency" };

    private:
        CENTAUR_INTERFACE_NAMESPACE::ILogger *m_logger { nullptr };
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config { nullptr };
        std::unique_ptr<SpotMarketWS> m_spotWS { nullptr };
        std::unique_ptr<std::thread> m_spotWSThread { nullptr };

    protected:
        CENTAUR_PLUGIN_NAMESPACE::StringIconVector m_symbols;

    private:
        std::unique_ptr<BINAPI_NAMESPACE::BinanceAPISpot> m_bAPI { nullptr };
        BINAPI_NAMESPACE::SPOT::ExchangeInformation m_exchInfo;
        BINAPI_NAMESPACE::BinanceLimits m_limits;
        BINAPI_NAMESPACE::BinanceKeys m_keys;

        // SEVEN-DAY CACHE CHART
    private:
        QDate m_sevenDayLastUpdate;
        QMap<QString, QList<std::pair<quint64, qreal>>> m_sevenDayCache;

    protected:
        std::map<int, QString> m_wsIds;
        std::unordered_set<QString> m_symbolsWatch;
        std::unordered_map<QString, std::pair<bool, uint64_t>> m_symbolOrderbookSnapshot;

    protected:
        BINAPI_NAMESPACE::AllCoinsInformation m_coinInformation;
        BINAPI_NAMESPACE::SpotTradingFees m_fees;

        // IStatus data
    protected:
        QPixmap m_image;
        QAction *m_statusAction;
    };

    class SpotMarketWS : public BINAPI_NAMESPACE::ws::WSSpotBinanceAPI
    {
    public:
        explicit SpotMarketWS(std::promise<void> connected);
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
