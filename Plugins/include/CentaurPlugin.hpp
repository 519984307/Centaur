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
#include <QToolBar>
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
    constexpr char centaurUUID[] = "{a15c48b4-460b-4a79-a0a8-8ece90603f85}";

    struct PluginInformation
    {
        uuid id;
        QString name;
        QString version;
    };

    inline bool operator==(const PluginInformation &pi1, const PluginInformation &pi2)
    {
        return (pi1.id == pi2.id && pi1.name == pi2.name && pi1.version == pi2.version);
    }

    inline bool operator!=(const PluginInformation &pi1, const PluginInformation &pi2)
    {
        return !(pi1 == pi2);
    }

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
        /// \param lOper Allows access to the LongOperation dialog
        virtual void setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, CENTAUR_INTERFACE_NAMESPACE::ILongOperation *lOper) noexcept = 0;

        /// \brief Get the plugin UUID
        /// \return Beware that versions are check in order to run the plugin
        virtual uuid getPluginUUID() noexcept = 0;

        /// \brief When a menu action is defined in the UI file.
        /// \param action Action to be connected
        /// \param menuId As specified by the UI file
        /// \return true on success. false on fail and the menu will be grayed
        /// \remarks Generally what you will do is: connect(action, &QAction::triggered, (pluginObjectPointer), plugin member function);
        C_NODISCARD virtual bool addMenuAction(QAction *action, const uuid &menuId) noexcept = 0;
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

        /// \brief On forex and crypto markets this is the quote name. On Stock this should return the symbol
        /// \param symbol The symbol name
        /// \return Quote from symbol
        C_NODISCARD virtual QString getQuoteFromSymbol(const QString &symbol) noexcept = 0;

        /// \brief On forex and crypto markets this is the base name. On Stock this should return the symbol.
        /// \param symbol The symbol name
        /// \return Base from symbol.
        C_NODISCARD virtual QString getBaseFromSymbol(const QString &symbol) noexcept = 0;

        /// \brief When the user makes a right click on the watchlist. This list is used to populate the popup menu
        /// \return A list with the name and an associated uuid. This uuid will be used to call setDynamicWatchListMenuAction to assign the menu an action
        C_NODISCARD virtual QList<QPair<QString, uuid>> dynamicWatchListMenuItems() noexcept = 0;

        /// \brief Assign a menu action to the popup menu in the dynamic watchlist
        /// \param action The menu action. This is created by the UI and passed to the plugin to connect with a slot
        /// \param symbolName The name of the symbol that the user clicked on
        /// \param id This uuid is the id retrieved from dynamicWatchListMenuItems and repassed to this function
        /// \return return true if the menu was implemented and the
        C_NODISCARD virtual bool setDynamicWatchListMenuAction(QAction *action, const QString &symbolName, const uuid &id) noexcept = 0;

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
    /// Each ICandleView object is a candle object
    struct ICandleView : public IBase
    {
        /// \brief All charts are to 4320 candles at all time. This means 72 minutes in candles of one seconds, 72-hours(3-days) in candles of 1 minute or 18-days on 1 hour candles
        /// \remarks No more than candleLimit is going to be held in the UI. Is up to the Plugin to keep more information on memory if that is considered prudent.
        /// Remember that calls to getCandlesByPeriod are not asynchronous so having some information
        static constexpr uint64_t candleLimit = 4320;

        /// \brief When an interface emits snRealTimeCandleUpdate without a previous request this is the limit before a disconnection will be set
        /// \related See the documentation for snRealTimeCandleUpdate and acquire and disengage
        static constexpr uint64_t unwantedSignalLimit = 1000;

        /// \brief All timestamps must be in milliseconds
        using Timestamp = uint64_t;

        /// \brief SupportedTimeFrames indicates the timeframes supported by the UI. This not necessarily means that this is supported
        /// by the exchange
        enum class TimeFrame
        {
            nullTime, // When returning from supportedTimeFrames you can use this enumerator to insert a Menu separator
            Seconds_1,
            Seconds_5,
            Seconds_10,
            Seconds_30,
            Seconds_45,
            Minutes_1,
            Minutes_2,
            Minutes_3,
            Minutes_5,
            Minutes_10,
            Minutes_15,
            Minutes_30,
            Minutes_45,
            Hours_1,
            Hours_2,
            Hours_4,
            Hours_6,
            Hours_8,
            Hours_12,
            Days_1,
            Days_3,
            Weeks_1,
            Months_1
        };

        /// \brief Data for plotting the Candles
        struct CandleData
        {
            double high;
            double open;
            double close;
            double low;
            double volume;
        };

        /// \brief All exchange information comes from plugin. So this functions indicates the UI which plugins this ICandleView can receive information
        /// \return A List of supported plugins
        C_NODISCARD virtual QList<PluginInformation> supportedExchanges() noexcept = 0;


        /// \brief Must return a list of the timeframes supported by this interface
        /// \return The list of time frames
        C_NODISCARD virtual QList<TimeFrame> supportedTimeFrames() noexcept = 0;


        /// \brief When the realtime data is retrieved by WebSockets, this function will be called whenever the user request the information of a symbol
        /// This will guarantee that the interface sends data that will ultimately be ignored
        /// Effectively, when the user needs the data a new window will be created
        /// \param pi Plugin information calling for the symbol
        /// \param symbol Symbol
        /// \param frame Timeframe of the request
        /// \param id A unique identifier that the UI uses. Use this id in disengage and snReframe.
        /// \remarks The id is used to keep track of multiple window with the same timeframe
        virtual void acquire(const PluginInformation &pi, const QString &symbol, TimeFrame frame, const uuid &id) noexcept = 0;

        /// \brief As opposed to acquire, when the user no longer needs the data by means of a window close this function will be called
        /// Once again, if the interface emits snRealTimeCandleUpdate when this function is called the UI will disconnect the signal when unwantedSignalLimit is reached
        /// \param id The id to be deleted. In order for resetStoredZoom to work, keep this ID
        /// \param lastTimeframeEnd When the window is closed this is the last beginning of the view interval
        /// \param lastTimeframeStart When the window is closed this is the last ending of the view interval
        /// \remarks The UI keeps track the last window frame because if the user reopens the same symbol with the same timeframe
        /// The UI will attempt to reopen the window with the same zooming level. So the UI informs of this situation in order to the interface to keep record
        /// Of the data in case the user needs it once again. When a call to resetStoredZoom the interface can delete this information
        virtual void disengage(const uuid &id, uint64_t lastTimeframeStart, uint64_t lastTimeframeEnd) noexcept = 0;

        /// \brief See disengage remarks
        /// When a certain period of time has elapsed and the user does not reopen a symbol stored with same data, the UI calls this function
        /// To safely free the memory of the candle data when disengage is called IF the plugin indeed saved this information, otherwise is safe to ignore this function
        virtual void resetStoredZoom(const uuid &id) noexcept = 0;

        /// \brief When zooming, this function will be called to asked the interface for the necessary to populate the CandleView Chart.
        /// \param symbol Name of the symbol to update
        /// \param start Start of the requested time
        /// \param end End of the requester period of time
        /// \param frame Time frame requested
        /// \return Must return a list with the data requested. If the requested candle of an specific timestamp is not available, do not put it in the list
        /// \remarks 0. Although, zooming is a user-based input, the start and end are calculated by the UI
        /// \remarks 1. Calls to getCandlesByPeriod are meant to retrieve contiguous chunks of candle information based on the time frame.
        /// If the information in the UI are fragmented expect multiple calls to getCandlesByPeriod to fill in the gaps
        /// \remarks 2. All candles retrieved by this function will assume that the candles are closed, this means, the closing time is in the past
        C_NODISCARD virtual QList<QPair<Timestamp, CandleData>> getCandlesByPeriod(const QString &symbol, Timestamp start, Timestamp end, TimeFrame frame) noexcept = 0;

        /// \brief When the plugin is loaded, it will call this function.
        /// IMPORTANT: If this function returns true the UI will attempt to make a connection with snRealTimeCandleUpdate and snDynamicCandleClose
        /// This signals are meant to update the candle chart at real time
        /// \return if dynamic plot is allow or not
        virtual bool realtimePlotAllowed() noexcept = 0;

        /// \brief If this function returns true, the signal snReframe is going to be connected
        /// \return True to allow the plugin to change its time frame dynamically
        virtual bool dynamicReframePlot() noexcept = 0;

        /// \brief This function must create a set of toolbars to be display in each window with the candles chart
        /// \return A list of toolbars that the plugin wish to be
        /// \remarks All toolbars will be re-parent to a UI specific Widget. This means that the UI will take ownership of all toolbars returned
        /// \remarks Floating will be disable from the toolbars
        C_NODISCARD virtual QList<QToolBar *> getPluginBasedToolBar() noexcept = 0;
        /**
        signals:
           /// \brief Emit this signal to notify the UI that a candle must be update
           /// \param id The id sent by acquire
           /// \param currentCandle This is the candle to be updated. however, internally, the UI will access the last element directly
           /// \param candle New candle data
           /// \param frame Time frame
           /// \remarks 0. The UI will assume that the Timestamp is always the last one in the internal candle buffer and it will be stop to be updated once snRealTimeCandleClose is called
           /// \remarks 1. If this signal is emitted without a previous acquire called the signal will be disconnected from the interface when unwantedSignalLimit is reached
           /// nd the interface can't do anything to reconnect.
           /// \remarks 2. The same case applies if a call to disengage is made and the interface keeps pushing the data
           void snRealTimeCandleUpdate(const uuid &id, Timestamp currentCandle, const CandleData &candle);

           /// \brief Emit this signal to notify that a candle has been closed
           /// \param id The id sent by acquire
           /// \param currentCandle Current timeframe. If currentCandle and the currentCandle in the time
           /// \param candle New candle data
           /// \param frame Time frame
           /// \remarks IMPORTANT: KEEP IN MIND THAT THE CANDLE LIST IS LIMITED TO ICandleView::candleLimit
           void snRealTimeCandleClose(const uuid &id, Timestamp currentCandle, const CandleData &candle, TimeFrame frame);

           /// \brief Emit this signal if the plot has changed the time frame
           /// \param id The id sent by acquire
           /// \remarks Expect a call to getCandlesByPeriod to fill in the gaps
           /// \remarks Due to latency issues some snRealTimeCandleUpdate event may be triggered after an snReframe, avoid sending this information because all internal buffers will be erased
           /// And snRealTimeCandleUpdate may poison the internal data
           void snReframe(const uuid &id);
           */
    };

    /// \brief Implements a ToolBar-like in the
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
