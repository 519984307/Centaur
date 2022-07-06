/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CENTAURAPP_HPP
#define CENTAUR_CENTAURAPP_HPP

#include "../ui/ui_CentaurApp.h"
#include "CenListCtrl.hpp"
#include "CentaurGlobal.hpp"
#include "CentaurInterface.hpp"
#include "CentaurPlugin.hpp"
#include "CentaurUIState.hpp"
#include "ConfigurationInterface.hpp"
#include "Globals.hpp"
#include "Logger.hpp"
#include "LongOperation.hpp"
#include "ProtocolServer.hpp"
#include "XMLHelper.hpp"
#include <QPluginLoader>
#include <QSignalMapper>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <chrono>
#include <memory>

namespace CENTAUR_NAMESPACE
{
    class FavoritesDBManager;
    class CentaurApp final : public QMainWindow
    {
        struct ExchangeInformation
        {
            uuid uuid;
            plugin::IExchange *exchange;
            CenListCtrl *list;
            QString listName;
        };

        struct WatchlistInformation
        {
            QStandardItem *listItem;
            double previousPrice;
        };

        struct CandleViewSupport
        {
            QList<CENTAUR_PLUGIN_NAMESPACE::PluginInformation> info;
            QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame> timeframes;
        };

        struct CandleViewTimeFrameActions
        {
            explicit CandleViewTimeFrameActions(QObject *parent);

        public:
            QAction *aSeconds_1 { nullptr };
            QAction *aSeconds_5 { nullptr };
            QAction *aSeconds_10 { nullptr };
            QAction *aSeconds_30 { nullptr };
            QAction *aSeconds_45 { nullptr };
            QAction *aMinutes_1 { nullptr };
            QAction *aMinutes_2 { nullptr };
            QAction *aMinutes_3 { nullptr };
            QAction *aMinutes_5 { nullptr };
            QAction *aMinutes_10 { nullptr };
            QAction *aMinutes_15 { nullptr };
            QAction *aMinutes_30 { nullptr };
            QAction *aMinutes_45 { nullptr };
            QAction *aHours_1 { nullptr };
            QAction *aHours_2 { nullptr };
            QAction *aHours_4 { nullptr };
            QAction *aHours_6 { nullptr };
            QAction *aHours_8 { nullptr };
            QAction *aHours_12 { nullptr };
            QAction *aDays_1 { nullptr };
            QAction *aDays_3 { nullptr };
            QAction *aWeeks_1 { nullptr };
            QAction *aMonths_1 { nullptr };
        };

        /// \brief General data types
        using PluginInstanceList      = QList<QPluginLoader *>;
        using PluginConfigurationMap  = std::unordered_map<QString, PluginConfiguration *>;
        using PluginExchangesMap      = std::map<QString, ExchangeInformation>;
        using PluginCandleViewSupport = std::map<CENTAUR_PLUGIN_NAMESPACE::ICandleView *, CandleViewSupport>;
        using PluginCandleViewDisplay = std::unordered_map<uuid, std::tuple<QMdiSubWindow *, QString, CENTAUR_PLUGIN_NAMESPACE::ICandleView *, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>;
        using WatchListInformationMap = std::unordered_map<int, WatchlistInformation>;
        using OrderbookDisplay        = std::pair<QString, QString>;
        using PluginCandleEmitter     = std::tuple<CENTAUR_PLUGIN_NAMESPACE::PluginInformation, QString, CENTAUR_PLUGIN_NAMESPACE::ICandleView *>;

        Q_OBJECT
    public:
        explicit CentaurApp(QWidget *parent = nullptr);
        ~CentaurApp() override;

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;

        void closeEvent(QCloseEvent *event) override;

    private:
        /// \brief initializeInterface Set interface status
        void initializeInterface() noexcept;
        /// \brief Load the internal centaur database file
        void initializeDatabaseServices() noexcept;
        /// \brief Store the user interface geometry and state before closing
        void saveInterfaceState() noexcept;
        /// \brief Load the user interface geometry and state
        void loadInterfaceState() noexcept;
        /// \brief Starts the Logging thread
        void startLoggingService() noexcept;
        /// Start the server
        void startCommunicationsServer() noexcept;
        /// \brief Load plugin information
        void loadPlugins() noexcept;
        /// \brief Initialize the XML Xerces C++ library and call load the functions that load external XML files
        void loadConfigurationData() noexcept;
        /// \brief Load Strings
        void loadLocaleData() noexcept;
        /// \brief Load the general personalized data to visualized the user interface
        void loadVisualsUI() noexcept;
        /// \brief Load the favorites watchlist
        void loadFavoritesWatchList() noexcept;

    protected:
        /// \brief Update the menus from the plugins.xml file
        void updatePluginsMenus(const uuid &uuid, xercesc::DOMDocument *doc, plugin::IBase *base) noexcept;
        /// \brief Load the local data
        void loadPluginLocalData(const uuid &uuid, xercesc::DOMDocument *doc, PluginConfiguration *config) noexcept;

    protected:
        /// \brief Add the symbol to the favorites Database
        /// \param symbol Symbol Name
        /// \param sender Plugin UUID
        void addFavoritesWatchListDB(const QString &symbol, const QString &sender) noexcept;
        /// \brief Remove the symbol from the favorites Database
        /// \param symbol Symbol Name
        /// \param sender Plugin UUID
        void removeFavoritesWatchListDB(const QString &symbol, const QString &sender) noexcept;

    public:
        plugin::IExchange *exchangeFromWatchlistRow(int row) noexcept;
        plugin::IExchange *exchangeFromWatchlistRow(const QString &sender) noexcept;

    protected:
        bool initExchangePlugin(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange) noexcept;
        bool initCandleViewPlugin(CENTAUR_PLUGIN_NAMESPACE::ICandleView *candleView) noexcept;
        CenListCtrl *populateExchangeSymbolList(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange, const QString &uuidString) noexcept;

    protected:
        void plotDepth(const QMap<qreal, QPair<qreal, qreal>> &asks, const QMap<qreal, QPair<qreal, qreal>> &bids) noexcept;
        void clearOrderbookListsAndDepth() noexcept;

    private slots:
        void onActionSymbolsToggled(bool status);
        void onActionLoggingToggled(bool status);
        void onActionBalancesToggled(bool status);
        void onActionAsksToggled(bool status);
        void onActionBidsToggled(bool status);
        void onActionDepthToggled(bool status);
        void onLog(unsigned long long date, int session, int level, const QString &usr, const QString &source, const QString &msg) noexcept;
        void onAddToWatchList(const QString &symbol, const QString &sender, bool addToDatabase) noexcept;
        void onWatchlistRemoveSelection() noexcept;
        void onSetWatchlistSelection(const QString &source, const QString &symbol) noexcept;
        void onRemoveWatchList(int row) noexcept;
        void onTickerUpdate(const QString &symbol, int id, quint64 receivedTime, double price) noexcept;
        void onOrderbookUpdate(const QString &source, const QString &symbol, quint64 receivedTime, const QMap<qreal, QPair<qreal, qreal>> &bids, const QMap<qreal, QPair<qreal, qreal>> &asks) noexcept;
        void onPlugins() noexcept;
        void onRealTimeCandleUpdate(const cen::uuid &id, quint64 eventTime, cen::plugin::ICandleView::Timestamp ts, const cen::plugin::ICandleView::CandleData &cd) noexcept;

        void onCandleView(const QString &symbol, CENTAUR_PLUGIN_NAMESPACE::ICandleView *view, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf, const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &emitter) noexcept;

    public:
        inline QTreeWidget *getBalancesTree() noexcept { return m_ui->ctrlBalances; }
        inline QLabel *getTotalLabel() noexcept { return m_ui->totalBalances; }

    public:
        static CENTAUR_PLUGIN_NAMESPACE::PluginInformation pluginInformationFromBase(CENTAUR_PLUGIN_NAMESPACE::IBase *);
        CENTAUR_PLUGIN_NAMESPACE::ICandleView *getSupportedCandleViewPlugins(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &id);
        std::optional<std::reference_wrapper<const QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>> getCandleViewTimeframeSupport(CENTAUR_PLUGIN_NAMESPACE::ICandleView *id) const;

    public:
        std::unique_ptr<CandleViewTimeFrameActions> m_candleViewTimeFrameActions;
        PluginCandleEmitter m_candleEmitter; /// Information of the item of the candle requested of each symbol (The candle emitter)

        // General application state
    private:
        std::unique_ptr<Ui::CentaurApp> m_ui;

    private:
        std::unique_ptr<std::thread> m_loggerThread { nullptr };

        // Plugins
    private:
        PluginInstanceList m_pluginInstances;            /// All instances of the plugins
        PluginConfigurationMap m_configurationInterface; /// Every PluginConfiguration mapped to the Plugin UUID
        PluginExchangesMap m_exchangeList;               /// Every ExchangeInformation (IExchanges plugin and their basic information) map
        PluginCandleViewSupport m_candleViewSupport;     /// Map of every ICandleView plugins with their supported timeframes and IExchange(s)
        PluginCandleViewDisplay m_candleViewDisplay;     /// Current candles on display
        WatchListInformationMap m_watchlistItems;        /// Each WatchlistInformation (price info and item list) mapped to ther corresponding id
        OrderbookDisplay m_currentViewOrderbookSymbol;   /// Each item being displayed on the Orderbook

        // Long Operation Dialog
    private:
        std::unique_ptr<LongOperation> m_longOperation { nullptr };

        // Watchlist
    private:
        QStandardItemModel *m_watchlistItemModel { nullptr };

        // Plotting
    private:
        QSplineSeries *m_asksDepth { nullptr };
        QSplineSeries *m_asksDepthFill { nullptr };
        QSplineSeries *m_bidsDepth { nullptr };
        QSplineSeries *m_bidsDepthFill { nullptr };

    private:
        // It will just be incremented
        int m_sessionIds { 0 };

        // Communication Server
    private:
        QLabel *m_serverStatus { nullptr };
        std::unique_ptr<ProtocolServer> m_server;

    private:
        FavoritesDBManager *m_sqlFavorites { nullptr };

    public:
        std::vector<CENTAUR_PLUGIN_NAMESPACE::IBase *> m_pluginsData;
    };
    extern CentaurApp *g_app;

} // namespace CENTAUR_NAMESPACE

Q_DECLARE_METATYPE(cen::uuid)
Q_DECLARE_METATYPE(cen::plugin::ICandleView::Timestamp)
// Q_DECLARE_METATYPE(cen::plugin::ICandleView::CandleData)
Q_DECLARE_METATYPE(cen::plugin::ICandleView::TimeFrame)

#define START_TIME(x)                    \
    auto x                               \
    {                                    \
        std::chrono::steady_clock::now() \
    }

#define END_TIME_SEC(x, y, z)                               \
    auto y { std::chrono::steady_clock::now() };            \
    auto z                                                  \
    {                                                       \
        std::chrono::duration<double, std::ratio<1>>(y - x) \
    }

#define END_TIME_MS(x, y, z)                                      \
    auto y { std::chrono::steady_clock::now() };                  \
    auto z                                                        \
    {                                                             \
        std::chrono::duration<double, std::ratio<1, 1000>>(y - x) \
    }

#define END_TIME_US(x, y, z)                                         \
    auto y { std::chrono::steady_clock::now() };                     \
    auto z                                                           \
    {                                                                \
        std::chrono::duration<double, std::ratio<1, 1000000>>(y - x) \
    }

#define END_TIME_MIN(x, y, z)                                   \
    auto y { std::chrono::steady_clock::now() };                \
    auto z                                                      \
    {                                                           \
        std::chrono::duration<double, std::ratio<60, 1>>(y - x) \
    }

#endif // CENTAUR_CENTAURAPP_HPP
