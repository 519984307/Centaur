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

    struct CandleViewSupport
    {
        QList<CENTAUR_PLUGIN_NAMESPACE::PluginInformation> info;
        QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame> timeframes;
    };

    using PluginConfigurationMap = std::unordered_map<uuid, PluginConfiguration *>;
    using PluginExchangesMap     = std::map<uuid, ExchangeInformation>;

    using PluginCandleViewDisplay = std::unordered_map<uuid, std::tuple<QMdiSubWindow *, QString, CENTAUR_PLUGIN_NAMESPACE::ICandleView *, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>;
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
    void loadPlugins(SplashDialog *splash) noexcept;
    /// \brief Load the favorites watchlist
    void loadFavoritesWatchList() noexcept;

protected:
    /// \brief Add the symbol to the favorites Database
    /// \param symbol Symbol Name
    /// \param sender Plugin UUID
    void addFavoritesWatchListDB(const QString &symbol, const QString &sender) noexcept;

public:
    plugin::IExchange *exchangeFromWatchlistRow(int row) noexcept;
    plugin::IExchange *exchangeFromWatchlistRow(const QString &sender) noexcept;

protected:
    bool initExchangePlugin(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange) noexcept;
    bool initCandleViewPlugin(CENTAUR_PLUGIN_NAMESPACE::ICandleView *candleView) noexcept;
    void initStatusPlugin(CENTAUR_PLUGIN_NAMESPACE::IStatus *status) noexcept;
    OptionsTableWidget *populateExchangeSymbolList(CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange, const QString &uuidString) noexcept;

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

    void onRealTimeCandleUpdate(const cen::uuid &id, quint64 eventTime, cen::plugin::ICandleView::Timestamp ts, const cen::plugin::ICandleView::CandleData &cd) noexcept;

    void onCandleView(const QString &symbol, CENTAUR_PLUGIN_NAMESPACE::ICandleView *view, CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame tf, const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &emitter) noexcept;

protected:
    Ui::CentaurApp *ui();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

public:
    static CENTAUR_PLUGIN_NAMESPACE::PluginInformation pluginInformationFromBase(CENTAUR_PLUGIN_NAMESPACE::IBase *);
    CENTAUR_PLUGIN_NAMESPACE::ICandleView *getSupportedCandleViewPlugins(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &id);
    std::optional<std::reference_wrapper<const QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>> getCandleViewTimeframeSupport(CENTAUR_PLUGIN_NAMESPACE::ICandleView *id) const;

public:
    PluginCandleEmitter m_candleEmitter; /// Information of the item of the candle requested of each symbol (The candle emitter)

private:
    std::unique_ptr<std::thread> m_loggerThread { nullptr };

protected:
    void mapConfigurationInterface(const uuid &id, PluginConfiguration *config);
    void mapExchangePlugin(const uuid &id, const ExchangeInformation &info);
    void mapPluginInstance(QPluginLoader *loader);
    void mapExchangePluginViewMenus(const uuid &plugin, const QList<QAction *> &actions);
    void mapStatusPlugins(const uuid &plugin, CENTAUR_PLUGIN_NAMESPACE::IStatus *status, QToolButton *button, CENTAUR_PLUGIN_NAMESPACE::IStatus::DisplayMode mode);
    void mapCandleViewSupport(CENTAUR_PLUGIN_NAMESPACE::ICandleView *candle, const CandleViewSupport &view);

    // Plugins
private:
    PluginCandleViewDisplay m_candleViewDisplay; /// Current candles on display

    // Communication Server
private:
    std::unique_ptr<ProtocolServer> m_server;

private:
    FavoritesDBManager *m_sqlFavorites { nullptr };

public:
    std::vector<CENTAUR_PLUGIN_NAMESPACE::IBase *> m_pluginsData;
};
extern CentaurApp *g_app;

END_CENTAUR_NAMESPACE

Q_DECLARE_METATYPE(cen::uuid)
Q_DECLARE_METATYPE(cen::plugin::ICandleView::Timestamp)
Q_DECLARE_METATYPE(cen::plugin::IStatus::DisplayRole)
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
