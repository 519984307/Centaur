/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CENTAURPLUGIN_HPP
#define CENTAUR_CENTAURPLUGIN_HPP

#include "../../Centaur.hpp"
#include "../../Library/uuid/include/uuid.hpp"
#include <CentaurInterface.hpp>

#define CENTAUR_PLUGIN_VERSION_CODE(x, y, z) \
    (((x)*100000) + ((y)*100) + (z))

#ifndef DONT_INCLUDE_QT
#include <QObject>
#include <QStatusBar>
#include <QString>
#include <QtPlugin>
#endif /*DONT_INCLUDE_QT*/

/// Current Centaur plugin version 0.2.0
#define CENTAUR_PLUGIN_MAJOR_VERSION 0
#define CENTAUR_PLUGIN_MINOR_VERSION 2
#define CENTAUR_PLUGIN_PATCH_VERSION 0

#define CENTAUR_PLUGIN_VERSION CENTAUR_PLUGIN_VERSION_CODE(CENTAUR_PLUGIN_MAJOR_VERSION, CENTAUR_PLUGIN_MINOR_VERSION, CENTAUR_PLUGIN_PATCH_VERSION)

#ifndef CENTAUR_PLUGIN_NAMESPACE
#define CENTAUR_PLUGIN_NAMESPACE CENTAUR_NAMESPACE::plugin
#endif /*CENTAUR_PLUGIN_NAMESPACE*/

#ifndef DONT_INCLUDE_QT

#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif /*__clang__*/

/**
 * \brief
 * uuid namespace: a15c48b4-460b-4a79-a0a8-8ece90603f85
 */

namespace CENTAUR_PLUGIN_NAMESPACE
{

    /// General types
    using StringIcon       = std::pair<QString, QIcon *>;
    using StringIconVector = std::vector<StringIcon>;

    // Use this UUID to generate v5 UUID for the plugins as well as to identify the plugin compatibility with the UI
    constexpr char centaurUUID[] =  "{a15c48b4-460b-4a79-a0a8-8ece90603f85}";

    struct IBase
    {
        virtual ~IBase() = default;

        /// \brief Get the plugin QObject
        /// \return Pointer of the QObject
        /// \remarks Beware that this pointer will be use to access the signals required from some signals
        virtual QObject *getPluginObject() noexcept = 0;

        /// \brief The plugin name to be displayed in the plugins list in the the main app
        /// \return A String containing the plugin name
        C_NODISCARD virtual QString getPluginName() noexcept = 0;

        /// \brief The version string returned will be used to populate the plugins list in the main App
        /// \return A string containing the plugin version: example: "1.0.0" or "2.3.520.12323" or "2.3.rc1"
        C_NODISCARD virtual QString getPluginVersionString() noexcept = 0;

        /// \brief init the logger and the configuration interfaces
        /// \param logger Use to communicate plugin logs in the user interface
        /// \param config Configuration access from the configuration file. This structure wont hold data by the time this function is called. However, by the time the initialization methods
        ///               of an IBase derived class this structure will hold the plugin data
        virtual void setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config) noexcept = 0;

        /// \brief Get the plugin UUID
        /// \return Beware that versions are check in order to run the plugin
        virtual uuid getPluginUUID() noexcept = 0;
    };

    /// \brief Provides access to the status bar
    struct IStatus : public IBase
    {
        ~IStatus() override = default;

        /// \brief Provides access to the status bar
        /// \param bar The status bar object
        virtual void initialization(QStatusBar *bar) noexcept = 0;
    };

    /// \brief Handles an exchange
    /// Controls the Symbols list
    /// Controls the Balances list
    /// Controls the Orderbook list
    /// Controls the Watchlist
    struct IExchange : public IBase
    {
        ~IExchange() override = default;

        /// \brief Initialize the plugin
        /// \return true on success; false on failure
        virtual bool initialization() noexcept = 0;

        /// \brief getSymbolListName Get name and the icon that will be displayed on "Symbol List Tab" and the icon in order to add a new tab
        /// \return If the QString part of StringIcon is empty. A new will not be created
        C_NODISCARD virtual StringIcon getSymbolListName() const noexcept = 0;

        /// \brief getSymbolList returns all the symbols handled by the exchange in order to populate the "Symbol list tab".
        /// \return A vector containing the names of the symbols link to an icon
        C_NODISCARD virtual StringIconVector getSymbolList() const noexcept = 0;

        /// \brief addSymbol Add a symbol to watchlist
        /// \param name Name of the symbol
        /// \param item Keep track of this variable and link it to the symbol and pass
        /// it with updateSymbol. NEVER CHANGE IT'S VALUE \return True on success;
        /// false on error
        C_NODISCARD virtual bool addSymbol(const QString &name, const int &item) noexcept = 0;

        /// \brief removeSymbol The symbol was removed from the watchlist in the UI
        /// \param name Name of the symbol deleted
        virtual void removeSymbol(const QString &name) noexcept = 0;

        /// \brief updateOrderbook Tell the interface to send the orderbook data to the user interface
        /// \param symbol Symbol data to send
        virtual void updateOrderbook(const QString &symbol) noexcept = 0;

        /// \brief stopOrderbook Tell the interface to stop sending the orderbook data to the user interface
        /// \param symbol Symbol data to stop
        virtual void stopOrderbook(const QString &symbol) noexcept = 0;

        /**
         *
    signals:
         \brief Emit to the UI to update the the ticker
         receivedTime will be used to calculate the latency
        void sgTickerUpdate(const QString &symbol, const int &symbolId, const quint64 &receivedTime, const double &price);

         \brief Emit to the UI to update the orderbook list
        void sgOrderbookUpdate(const QString &source, const QString &symbol, const quint64 &receivedTime, const QMap<QString, QPair<QString, QString>> &bids, const QMap<QString, QPair<QString, QString>> &asks);
         */
    };

    /// \brief Handles the necessary data to access the candles chart view
    struct ICandleView : public IBase
    {
    };

    struct IDrawingGroup : public IBase
    {
    };

    /// \brief Implements a drawing tool. A drawing tool can be part of a IDrawingGroup or an individual tool
    struct IDrawingTool : public IBase
    {
    };

    /// \brief Handles a new indicator in the chart view
    struct IIndicator : public IBase
    {
    };

    /// \brief Handles how an strategy client sends and receives data
    /// Due to the overhead of the GUI application all strategies are encourage to be implemented as
    /// a separate executable and communicates via the CentaurServer protocol
    struct IStrategyClient : public IBase
    {
    };

} // namespace CENTAUR_PLUGIN_NAMESPACE

#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic pop
#endif /*__clang__*/

#define IBase_iid "com.centaur-project.plugin.IBase/0.2"
Q_DECLARE_INTERFACE(CENTAUR_PLUGIN_NAMESPACE::IBase, IBase_iid)

#define IStatus_iid "com.centaur-project.plugin.IStatus/0.2"
Q_DECLARE_INTERFACE(CENTAUR_PLUGIN_NAMESPACE::IStatus, IStatus_iid)

#define IExchange_iid "com.centaur-project.plugin.IExchange/1.0"
Q_DECLARE_INTERFACE(CENTAUR_PLUGIN_NAMESPACE::IExchange, IExchange_iid)

#define ICandleView_iid "com.centaur-project.plugin.ICandleView/1.0"
Q_DECLARE_INTERFACE(CENTAUR_PLUGIN_NAMESPACE::ICandleView, ICandleView_iid)

#define IDrawingGroup_iid "com.centaur-project.plugin.IDrawingGroup/1.0"
Q_DECLARE_INTERFACE(CENTAUR_PLUGIN_NAMESPACE::IDrawingGroup, IDrawingGroup_iid)

#define IIndicator_iid "com.centaur-project.plugin.IIndicator/1.0"
Q_DECLARE_INTERFACE(CENTAUR_PLUGIN_NAMESPACE::IIndicator, IIndicator_iid)

#define IStrategyClient_iid "com.centaur-project.plugin.IStrategyClient/1.0"
Q_DECLARE_INTERFACE(CENTAUR_PLUGIN_NAMESPACE::IStrategyClient, IStrategyClient_iid)

#endif /*DONT_INCLUDE_QT*/

#endif // CENTAUR_CENTAURPLUGIN_HPP
