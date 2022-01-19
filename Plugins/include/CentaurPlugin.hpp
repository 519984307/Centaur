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

#include <CentaurInterface.hpp>

#define CENTAUR_PLUGIN_VERSION_CODE(x, y, z) \
    (((x)*100000) + ((y)*100) + (z))

#ifndef DONT_INCLUDE_QT
#include <QObject>
#include <QStatusBar>
#include <QString>
#include <QtPlugin>
#endif /*DONT_INCLUDE_QT*/

/// Current Centaur plugin version 0.2
#define CENTAUR_PLUGIN_MAJOR_VERSION 0
#define CENTAUR_PLUGIN_MINOR_VERSION 2
#define CENTAUR_PLUGIN_PATCH_VERSION 0

#define CENTAUR_PLUGIN_VERSION CENTAUR_PLUGIN_VERSION_CODE(CENTAUR_PLUGIN_MAJOR_VERSION, CENTAUR_PLUGIN_MINOR_VERSION, CENTAUR_PLUGIN_PATCH_VERSION)

namespace cent::plugin
{
    constexpr unsigned int CentaurId0   = 0x55ee88d7;
    constexpr unsigned short CentaurId1 = 0x44aa;

#if defined(CENTAUR_PLUGIN_VERSION)
    // It is guaranteed that VersionStrings Id's will not collide
#if CENTAUR_PLUGIN_VERSION == 200
    constexpr unsigned char CentaurPluginVersionStringId = { 0xe3 };
    constexpr char CentaurPluginVersionString[]          = { "0.2.0" };
#endif /*CENTAUR_PLUGIN_VERSION*/
#endif /*CENTAUR_PLUGIN_VERSION*/

    /// \brief Plugin identification
    /// PluginUUID Specification
    /// {ZZZZZZZZ-ZZZZ-ABPP-PPEF-YYYYYYYYYYYY}
    /// Z: Plugin Developer identification
    /// AB: 8-bit unsigned number identifying the Centaur String Version
    /// P: Random numbers identifying the plugin
    /// EF: 8-bit unsigned number identifying the Centaur Plugin Version
    /// Y: Random numbers identifying the plugin

    struct PluginUUID
    {
        bool operator==(const PluginUUID &rhs) const
        {
            return dev0.dev0_u == rhs.dev0.dev0_u && dev1.dev1_s == rhs.dev1.dev1_s && sp0.sp0_s == rhs.sp0.sp0_s && sp1.sp1_s == rhs.sp1.sp1_s && plg.plg0.plg0_s == rhs.plg.plg0.plg0_s && plg.plg1.plg1_u == rhs.plg.plg1.plg1_u;
        }
        bool operator!=(const PluginUUID &rhs) const
        {
            return !(rhs == *this);
        }

        union dev0
        {
            unsigned char dev0_c[4];
            unsigned int dev0_u;
        } dev0;
        union dev1
        {
            unsigned char dev1_c[2];
            unsigned short dev1_s;
        } dev1;

        union sp0
        {
            unsigned char sp0_c[2];
            unsigned short sp0_s;
        } sp0;

        union sp1
        {
            unsigned char sp1_c[2];
            unsigned short sp1_s;
        } sp1;

        struct plg_
        {
            union plg0
            {
                unsigned char plg0_c[2];
                unsigned short plg0_s;
            } plg0;
            union plg1
            {
                unsigned char plg1_c[4];
                unsigned int plg1_u;
            } plg1;
        } plg; /// Plugin unique id
    };

    // Wraps the PluginUUID and can be associated with a string
    template <const std::size_t len = 45>
    struct GlobalPluginUUID_t
    {
        inline GlobalPluginUUID_t() noexcept
        {
            memset(id, 0, len * sizeof(char));
        }

        inline GlobalPluginUUID_t(const PluginUUID uuid_, const char *id_) :
            uuid { uuid_ }
        {
            assign(id_);
        }

        PluginUUID uuid;
        char id[len];

    public:
        inline void assign(const char *str) noexcept
        {
#if defined(WIN32) || defined(WIN64)
            strcpy_s(id, len, str);
#else
            strcpy(id, str);
#endif
        }
    };
    using GlobalPluginUUID = GlobalPluginUUID_t<>;
} // namespace cent::plugin

#ifndef DONT_INCLUDE_QT

#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif /*__clang__*/
namespace cent::plugin
{

    /// General types
    using StringIcon       = std::pair<QString, QIcon *>;
    using StringIconVector = std::vector<StringIcon>;
    using FuncPointer      = void (QObject::*)(void);

    struct IBase
    {
        virtual ~IBase() = default;

        /// \brief Get the plugin QObject
        /// \return Pointer of the QObject
        /// \remarks Beware that this pointer will be use to access the signals required from some signals
        virtual QObject *getPluginObject() noexcept = 0;

        /// \brief The plugin name to be displayed in the plugins list in the the main app
        /// \return A String containing the plugin name
        [[nodiscard]] virtual QString getPluginName() noexcept = 0;

        /// \brief The version string returned will be used to populate the plugins list in the main App
        /// \return A string containing the plugin version: example: "1.0.0" or "2.3.520.12323" or "2.3.rc1"
        [[nodiscard]] virtual QString getPluginVersionString() noexcept = 0;

        /// \brief init the logger and the configuration interfaces
        /// \param logger Use to set the logging capabilities from the main application
        /// \param config Configuration access from the configuration file
        virtual void setPluginInterfaces(cent::interface::ILogger *logger, cent::interface::IConfiguration *config) noexcept = 0;

        /// \brief Get the plugin UUID
        /// \return Beware that versions are check in order to run the plugin
        virtual PluginUUID getPluginUUID() noexcept = 0;

        /// \brief This function is called in order to the plugin connects a menu action of the user interface to a specific plugin action
        /// \param identifier The identifier name that is declared in the XML Plugins file
        /// \return nullptr on failure, otherwise it must return a pointer to the member function to call.
        ///         Can be reinterpret_cast<cent::plugin::FuncPointer>(&class::memberfunc);
        virtual FuncPointer connectMenu(const QString &identifier) noexcept = 0;
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
        [[nodiscard]] virtual StringIcon getSymbolListName() const noexcept = 0;

        /// \brief getSymbolList returns all the symbols handled by the exchange in order to populate the "Symbol list tab".
        /// \return A vector containing the names of the symbols link to an icon
        [[nodiscard]] virtual StringIconVector getSymbolList() const noexcept = 0;

        /// \brief addSymbol Add a symbol to watchlist
        /// \param name Name of the symbol
        /// \param item Keep track of this variable and link it to the symbol and pass
        /// it with updateSymbol. NEVER CHANGE IT'S VALUE \return True on success;
        /// false on error
        [[nodiscard]] virtual bool addSymbol(const QString &name, const int &item) noexcept = 0;

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

} // namespace cent::plugin

#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic pop
#endif /*__clang__*/

#define IBase_iid "com.centaur-project.plugin.IBase/1.0"
Q_DECLARE_INTERFACE(cent::plugin::IBase, IBase_iid)

#define IStatus_iid "com.centaur-project.plugin.IStatus/1.0"
Q_DECLARE_INTERFACE(cent::plugin::IStatus, IStatus_iid)

#define IExchange_iid "com.centaur-project.plugin.IExchange/1.0"
Q_DECLARE_INTERFACE(cent::plugin::IExchange, IExchange_iid)

#define ICandleView_iid "com.centaur-project.plugin.ICandleView/1.0"
Q_DECLARE_INTERFACE(cent::plugin::ICandleView, ICandleView_iid)

#define IDrawingGroup_iid "com.centaur-project.plugin.IDrawingGroup/1.0"
Q_DECLARE_INTERFACE(cent::plugin::IDrawingGroup, IDrawingGroup_iid)

#define IIndicator_iid "com.centaur-project.plugin.IIndicator/1.0"
Q_DECLARE_INTERFACE(cent::plugin::IIndicator, IIndicator_iid)

#define IStrategyClient_iid "com.centaur-project.plugin.IStrategyClient/1.0"
Q_DECLARE_INTERFACE(cent::plugin::IStrategyClient, IStrategyClient_iid)

#endif /*DONT_INCLUDE_QT*/

#endif // CENTAUR_CENTAURPLUGIN_HPP
