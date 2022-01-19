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

#include "Centaur.hpp"
#include "CentaurInterface.hpp"
#include "CentaurPlugin.hpp"
#include "CentaurUIState.hpp"
#include "Globals.hpp"
#include "Logger.hpp"
#include <QDomDocument>
#include <memory>

namespace cent
{
    QT_BEGIN_NAMESPACE
    namespace Ui
    {
        class CentaurApp;
        class CenListCtrl;
    } // namespace Ui
    QT_END_NAMESPACE
    class CentaurApp final : public QMainWindow
    {
        struct ExchangeInformation
        {
            cent::plugin::GlobalPluginUUID uuid;
            cent::plugin::IExchange *exchange;
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
        /// \brief Store the user interface geometry and state before closing
        void saveInterfaceState() noexcept;
        /// \brief Load the user interface geometry and state
        void loadInterfaceState() noexcept;
        /// \brief Starts the Logging thread
        void startLoggingService() noexcept;
        /// \brief Load plugin information
        void loadPlugins() noexcept;

    protected:
        /// \brief Update the menus from the plugins.xml file
        void updatePluginsMenu(const cent::plugin::PluginUUID &uuid, const QDomDocument &doc, plugin::IBase *base) noexcept;

    protected:
        bool initExchangePlugin(cent::plugin::IExchange *exchange) noexcept;
        cent::CenListCtrl *populateExchangeSymbolList(cent::plugin::IExchange *exchange, const QString &uuidString) noexcept;

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
        void onAddToWatchList(const QString &symbol, const QString &sender) noexcept;
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

        // Icons
    private:
        QIcon m_icnUpArrow { ":/img/res/img/upArrow.svg" };
        QIcon m_icnDownArrow { ":/img/res/img/downArrow.svg" };
        QIcon m_icnSearch { ":/img/res/img/Search.svg" };

        // Plugins
    private:
        std::map<QString /*uuid string*/, ExchangeInformation> m_exchangeList;
        std::map<int, WatchlistInformation> m_watchlistItems; // id associated with the item
        std::pair<QString, QString> m_currentViewOrderbookSymbol;

        // Watchlist
    private:
        QStandardItemModel *m_watchlistItemModel { nullptr };

        // Plotting
    private:
        QwtPlotCurve *m_plotAsksDepth { nullptr };
        QwtPlotCurve *m_plotBidsDepth { nullptr };

    private:
        // It will just be incremented
        int m_sessionIds { 0 };

    public:
        std::vector<cent::plugin::IBase *> m_pluginsData;
    };
    extern CentaurApp *g_app;
} // namespace cent

// Helper macros
#define logInfo(x, y) \
    g_logger->info(x, y)

#define logWarn(x, y) \
    g_logger->warning(x, y)

#if !defined(NDEBUG)
#define logTrace(x, y) \
    g_logger->trace(x, y)
#define logDebug(x, y) \
    g_logger->debug(x, y)
#else
#define logTrace(x, y) \
    ((void *)0)
#define logDebug(x, y) \
    ((void *)0)
#endif /**/

#define logError(x, y) \
    g_logger->error(x, y)

#define logFatal(x, y) \
    g_logger->fatal(x, y)

#endif // CENTAUR_CENTAURAPP_HPP
