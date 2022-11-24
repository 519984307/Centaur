/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CENTAUR_APP_HPP
#define CENTAUR_CENTAUR_APP_HPP

#include "CentaurGlobal.hpp"
#include "CentaurInterface.hpp"
#include "CentaurPlugin.hpp"
#include "CentaurUIState.hpp"
#include "ConfigurationInterface.hpp"
#include "Globals.hpp"
#include "Logger.hpp"
#include "ProtocolServer.hpp"
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QPluginLoader>
#include <QSignalMapper>
#include <QToolButton>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <chrono>
#include <memory>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class CentaurApp;
} // namespace Ui

class FavoritesDBManager;
class LogDialog;
class OptionsTableWidget;
class SplashDialog;
class OrderbookDialog;
class CentaurApp final : public QMainWindow
{
    struct ExchangeInformation
    {
        uuid uuid;
        plugin::IExchange *exchange;
        OptionsTableWidget *list;
        QString listName;
    };

    using PluginConfigurationMap = std::unordered_map<uuid, PluginConfiguration *>;
    using PluginExchangesMap     = std::map<uuid, ExchangeInformation>;

    Q_OBJECT
public:
    explicit CentaurApp(QWidget *parent = nullptr);
    ~CentaurApp() override;

protected:
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
    void loadPlugins(SplashDialog *splash) noexcept;
    /// \brief Load the favorites watchlist
    void loadFavoritesWatchList() noexcept;

protected:
    /// \brief Add the symbol to the favorites Database
    /// \param symbol Symbol Name
    /// \param sender Plugin UUID
    void addFavoritesWatchListDB(const QString &symbol, const QString &sender) noexcept;

protected:
    bool initExchangePlugin(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange) noexcept;
    void initStatusPlugin(CENTAUR_PLUGIN_NAMESPACE::IStatus *status) noexcept;
    OptionsTableWidget *populateExchangeSymbolList(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange) noexcept;

protected:
    void plotSevenDaysChart(const QString &symbol, const QList<std::pair<quint64, qreal>> &values) noexcept;
    void updateUserInformationStatus() noexcept;

public:
    LogDialog *logDialog() noexcept;

private slots:
    void onShowPlugins() noexcept;
    void onShowLogDialog() noexcept;
    void onShowSettings() noexcept;
    void onStatusDisplayChanged(plugin::IStatus::DisplayRole mode);
    void onAddToWatchList(const QString &symbol, const QString &sender, bool addToDatabase) noexcept;
    void onSetWatchlistSelection(const QString &source, const QString &symbol) noexcept;
    void onRemoveWatchList(const QModelIndex &index) noexcept;
    void onTickerUpdate(const QString &symbol, const QString &uuid, quint64 receivedTime, double price) noexcept;

protected:
    Ui::CentaurApp *ui();

public:
    static CENTAUR_PLUGIN_NAMESPACE::PluginInformation pluginInformationFromBase(CENTAUR_PLUGIN_NAMESPACE::IBase *);

public:
    std::vector<CENTAUR_PLUGIN_NAMESPACE::IBase *> &getPluginBase() const noexcept;

protected:
    void mapPluginBase(CENTAUR_PLUGIN_NAMESPACE::IBase *base) noexcept;
    void mapConfigurationInterface(const uuid &id, PluginConfiguration *config);
    void mapExchangePlugin(const uuid &id, const ExchangeInformation &info);
    void mapPluginInstance(QPluginLoader *loader);
    void mapExchangePluginViewMenus(const uuid &plugin, const QList<QAction *> &actions);
    void mapStatusPlugins(const uuid &plugin, CENTAUR_PLUGIN_NAMESPACE::IStatus *status, QToolButton *button, CENTAUR_PLUGIN_NAMESPACE::IStatus::DisplayMode mode);

private:
    FavoritesDBManager *m_sqlFavorites { nullptr };

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};
extern CentaurApp *g_app;

END_CENTAUR_NAMESPACE

Q_DECLARE_METATYPE(cen::uuid)
Q_DECLARE_METATYPE(cen::plugin::IStatus::DisplayRole)
// Q_DECLARE_METATYPE(cen::plugin::CandleData)
Q_DECLARE_METATYPE(cen::plugin::TimeFrame)
Q_DECLARE_METATYPE(cen::plugin::IExchange::Timestamp)

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

#endif // CENTAUR_CENTAUR_APP_HPP
