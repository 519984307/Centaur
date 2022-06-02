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
#include "XMLHelper.hpp"

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

        Q_OBJECT
    public:
        explicit CentaurApp(QWidget *parent = nullptr);
        ~CentaurApp() override;

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;

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

    protected:
        bool initExchangePlugin(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange) noexcept;
        CenListCtrl *populateExchangeSymbolList(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange, const QString &uuidString) noexcept;

    protected:
        void plotDepth(const QMap<QString, QPair<QString, QString>> &asks, const QMap<QString, QPair<QString, QString>> &bids) noexcept;
        void clearOrderbookListsAndDepth() noexcept;

    private slots:
        void onActionTileWindowsTriggered();
        void onActionCascadeWindowsTriggered();
        void onActionSymbolsToggled(bool status);
        void onActionLoggingToggled(bool status);
        void onActionBalancesToggled(bool status);
        void onActionAsksToggled(bool status);
        void onActionBidsToggled(bool status);
        void onActionDepthToggled(bool status);
        void onLog(const unsigned long long &date, const int &session, const int &level, const QString &usr, const QString &source, const QString &msg) noexcept;
        void onAddToWatchList(const QString &symbol, const QString &sender, bool addToDatabase) noexcept;
        void onWatchlistRemoveSelection() noexcept;
        void onSetWatchlistSelection(const QString &source, const QString &symbol) noexcept;
        void onRemoveWatchList(const int &row) noexcept;
        void onTickerUpdate(const QString &symbol, const int &id, const quint64 &receivedTime, const double &price) noexcept;
        void onOrderbookUpdate(const QString &source, const QString &symbol, const quint64 &receivedTime, const QMap<QString, QPair<QString, QString>> &bids, const QMap<QString, QPair<QString, QString>> &asks) noexcept;
        void onPlugins() noexcept;

        // General application state
    private:
        std::unique_ptr<Ui::CentaurApp> m_ui;
        UserInterfaceState m_uiState;

    private:
        std::unique_ptr<std::thread> m_loggerThread { nullptr };

        // Plugins
    private:
        std::unordered_map<QString, PluginConfiguration *> m_configurationInterface;
        std::map<QString, ExchangeInformation> m_exchangeList;
        std::map<int, WatchlistInformation> m_watchlistItems; // id associated with the item
        std::pair<QString, QString> m_currentViewOrderbookSymbol;

        // Watchlist
    private:
        QStandardItemModel *m_watchlistItemModel { nullptr };

        // Plotting
    private:
        // QwtPlotCurve *m_plotAsksDepth { nullptr };
        // QwtPlotCurve *m_plotBidsDepth { nullptr };

    private:
        // It will just be incremented
        int m_sessionIds { 0 };

    private:
        FavoritesDBManager *m_sqlFavorites { nullptr };

    public:
        std::vector<CENTAUR_PLUGIN_NAMESPACE::IBase *> m_pluginsData;
    };
    extern CentaurApp *g_app;
} // namespace CENTAUR_NAMESPACE

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
