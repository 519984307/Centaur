/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "CentaurApp.hpp"
#include "../ui/ui_CentaurApp.h"
#include "CandleViewWidget.hpp"
#include "DepthChartDialog.hpp"
#include "FavoritesDialog.hpp"
#include "LogDialog.hpp"
#include "LoginDialog.hpp"
#include "OrderbookDialog.hpp"
#include "PluginsDialog.hpp"
#include "SettingsDialog.hpp"
#include "SplashDialog.hpp"
#include <QAreaSeries>
#include <QDateTimeAxis>
#include <QGraphicsBlurEffect>
#include <QImageReader>
#include <QMenu>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtCharts/QValueAxis>
#include <utility>

/// \brief This structure hold the 'data' set to Impl::orderbookDepth and Impl::depthChart to properly
/// handle the visualization of data
struct OrderBookDepthInformation
{
    OrderBookDepthInformation()                                             = default;
    ~OrderBookDepthInformation()                                            = default;
    OrderBookDepthInformation(const OrderBookDepthInformation &)            = default;
    OrderBookDepthInformation &operator=(const OrderBookDepthInformation &) = default;

    OrderBookDepthInformation(QString sym, QString src) :
        symbol { std::move(sym) },
        source { std::move(src) } { }

    QString symbol;
    QString source;
};
Q_DECLARE_METATYPE(OrderBookDepthInformation);

BEGIN_CENTAUR_NAMESPACE

CentaurApp *g_app = nullptr;

struct CandleViewTimeFrameActions
{
    explicit CandleViewTimeFrameActions(QObject *parent);

public:
    std::map<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame, QAction *> actions;
};

struct CentaurApp::Impl
{
    explicit Impl(QMainWindow *parent) :
        ui { new Ui::CentaurApp },
        candleActions { new CandleViewTimeFrameActions(parent) },
        orderbookDepth { new QAction(QCoreApplication::tr("Orderbook"), parent) },
        depthChart { new QAction(QCoreApplication::tr("Depth chart"), parent) }
    {
        ui->setupUi(parent);
        logDialog = new LogDialog(parent);
    }

    std::unique_ptr<Ui::CentaurApp> ui;
    std::unique_ptr<CandleViewTimeFrameActions> candleActions;

    LogDialog *logDialog { nullptr };
    QChart *last7Chart { nullptr };

    QAction *orderbookDepth;
    QAction *depthChart;

    std::pair<QString, QString> currentWatchListSelection;

    PluginConfigurationMap configurationInterface; /// Every PluginConfiguration mapped to the Plugin UUID
    PluginExchangesMap exchangeList;               /// Every ExchangeInformation (IExchanges plugin and their basic information) map
    QList<QPluginLoader *> pluginInstances;        /// All instances of the plugins
    std::map<uuid, QList<QAction *>> exchangeMenuActions;
    std::map<uuid, std::tuple<CENTAUR_PLUGIN_NAMESPACE::IStatus *, QToolButton *, CENTAUR_PLUGIN_NAMESPACE::IStatus::DisplayMode>> statusPlugins;
    std::map<CENTAUR_PLUGIN_NAMESPACE::ICandleView *, CandleViewSupport> candleViewSupport; /// Map of every ICandleView plugins with their supported timeframes and IExchange(s)

    QAreaSeries *last7SevenAreaSeries { nullptr };
    QSplineSeries *last7SevenLowSeries { nullptr };
    QSplineSeries *last7SevenUpSeries { nullptr };
};

class FavoritesDBManager final
{
public:
    inline FavoritesDBManager()
    {
        m_insert.prepare("INSERT INTO favorites(symbol,plugin) VALUES(:symbol, :plugin);");
        m_delete.prepare("DELETE FROM favorites WHERE symbol = :symbol AND plugin = :plugin;");
        m_select.prepare("SELECT symbol,plugin FROM favorites;");
    }
    ~FavoritesDBManager() = default;

public:
    inline void add(const QString &symbol, const QString &uuid)
    {
        m_insert.bindValue(":symbol", symbol);
        m_insert.bindValue(":plugin", uuid);

        if (!m_insert.exec())
        {
            logError("app", QString(QCoreApplication::tr("Could not insert the symbol to the favorites DB. %1").arg(m_insert.lastError().text())));
        }
    }

    inline QList<QPair<QString, QString>> selectAll()
    {
        QList<QPair<QString, QString>> data;
        if (!m_select.exec())
        {
            logError("app", QString(QCoreApplication::tr("Could not select the data from the favorites DB. %1").arg(m_select.lastError().text())));
            return data;
        }

        while (m_select.next())
        {
            const QSqlRecord currentRecord = m_select.record();
            data.emplaceBack(currentRecord.field("symbol").value().toString(), currentRecord.field("plugin").value().toString());
        }

        return data;
    }

    inline void del(const QString &symbol, const QString &uuid)
    {
        m_delete.bindValue(":symbol", symbol);
        m_delete.bindValue(":plugin", uuid);

        if (!m_delete.exec())
        {
            logError("app", QString(QCoreApplication::tr("Could not delete symbol from the favorites DB. %1").arg(m_delete.lastError().text())));
        }
    }

private:
    QSqlQuery m_insert;
    QSqlQuery m_delete;
    QSqlQuery m_select;
    QSqlDatabase m_db;
};

CandleViewTimeFrameActions::CandleViewTimeFrameActions(QObject *parent) :
    actions {
        { CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_1,   new QAction(QCoreApplication::tr("1 second"), parent)},
        { CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_5, new QAction(QCoreApplication::tr(" 5 seconds"), parent)},
        {CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_10, new QAction(QCoreApplication::tr("10 seconds"), parent)},
        {CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_30, new QAction(QCoreApplication::tr("30 seconds"), parent)},
        {CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_45, new QAction(QCoreApplication::tr("45 seconds"), parent)},
        { CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_1,   new QAction(QCoreApplication::tr("1 minute"), parent)},
        { CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_2,  new QAction(QCoreApplication::tr("2 minutes"), parent)},
        { CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_3,  new QAction(QCoreApplication::tr("3 minutes"), parent)},
        { CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_5,  new QAction(QCoreApplication::tr("5 minutes"), parent)},
        {CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_10, new QAction(QCoreApplication::tr("10 minutes"), parent)},
        {CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_15, new QAction(QCoreApplication::tr("15 minutes"), parent)},
        {CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_30, new QAction(QCoreApplication::tr("30 minutes"), parent)},
        {CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_45, new QAction(QCoreApplication::tr("45 minutes"), parent)},
        {   CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_1,     new QAction(QCoreApplication::tr("1 hour"), parent)},
        {   CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_2,    new QAction(QCoreApplication::tr("2 hours"), parent)},
        {   CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_4,    new QAction(QCoreApplication::tr("4 hours"), parent)},
        {   CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_6,    new QAction(QCoreApplication::tr("6 hours"), parent)},
        {   CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_8,    new QAction(QCoreApplication::tr("8 hours"), parent)},
        {  CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_12,   new QAction(QCoreApplication::tr("12 hours"), parent)},
        {    CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_1,      new QAction(QCoreApplication::tr("1 day"), parent)},
        {    CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_3,     new QAction(QCoreApplication::tr("3 days"), parent)},
        {   CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Weeks_1,     new QAction(QCoreApplication::tr("1 week"), parent)},
        {  CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Months_1,    new QAction(QCoreApplication::tr("1 month"), parent)}
}
{
}

namespace
{
    constexpr char g_statusRed[] {
        R"(QLabel{
color: rgb(255, 255, 255);
background-color: rgb(172, 6, 0);
border-radius: 3px;
min-width: 85px;
margin: 0 0 0 10;
qproperty-alignment: AlignCenter;
})"
    };

    constexpr char g_statusGreen[] {
        R"(QLabel{
color: rgb(255, 255, 255);
background-color: rgb(0, 104, 18);
border-radius: 3px;
min-width: 85px;
margin: 0 0 0 10;
qproperty-alignment: AlignCenter;
})"
    };
} // namespace

CentaurApp::CentaurApp(QWidget *parent) :
    QMainWindow(parent),
    _impl { new Impl(this) }
{
    auto *splashScreen = new SplashDialog(this);
    splashScreen->setDisplayText(tr("Setting up user interface"));
    splashScreen->setProgressRange(0, 7);
    splashScreen->setProgressPos(0);
    splashScreen->show();

    START_TIME(initializationTimeStart);

    g_app     = this;
    g_globals = new Globals;

    qRegisterMetaType<uuid>("uuid");
    qRegisterMetaType<plugin::ICandleView::CandleData>("plugin::ICandleView::CandleData");
    qRegisterMetaType<plugin::ICandleView::TimeFrame>("plugin::ICandleView::TimeFrame");
    qRegisterMetaType<plugin::ICandleView::Timestamp>("plugin::ICandleView::Timestamp");
    qRegisterMetaType<plugin::IStatus::DisplayRole>("plugin::IStatus::DisplayRole");

    installEventFilter(this);

    /* TODO: THIS
    ui()->bidsChart->installEventFilter(this);
    ui()->asksChart->installEventFilter(this);
*/
#ifdef Q_OS_MAC
    CFURLRef appUrlRef       = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath      = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    g_globals->paths.appPath = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
    CFRelease(appUrlRef);
    CFRelease(macPath);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    g_globals->paths.pluginsPath = g_globals->paths.appPath + "/Contents/Plugins";

#else

#endif /*Q_OS_MAC*/

    g_globals->paths.installPath = "/Volumes/RicardoESSD/Projects/Centaur/local";

    g_globals->paths.resPath = g_globals->paths.installPath + "/Resources";

    // Start logging service
    startLoggingService();
    splashScreen->step();

    // Init the database internal file
    initializeDatabaseServices();
    splashScreen->step();

    // Restore file
    loadInterfaceState();
    splashScreen->step();

    // Start the interface
    initializeInterface();
    splashScreen->step();

    // Start the server

    // Start the server
    splashScreen->setDisplayText(tr("Starting the communication server"));
    // startCommunicationsServer();
    splashScreen->step();

    // Load plugins
    loadPlugins(splashScreen);
    splashScreen->step();

    // Load the favorites list. Since all plugins must be loaded
    splashScreen->setDisplayText(tr("Adding favorites watchlist"));
    loadFavoritesWatchList();
    splashScreen->step();

    /*
                        onCandleView("ETHUSDT", m_candleViewSupport.begin()->first, plugin::ICandleView::TimeFrame::Minutes_1, pluginInformationFromBase(m_exchangeList.begin()->second.exchange));
                    */
    END_TIME_SEC(initializationTimeStart, initializationTimeEnd, initializationTime);
    logInfo("app", QString("%1").arg(initializationTime.count(), 0, 'f', 4));
    splashScreen->hide();
    delete splashScreen;

    if (SettingsDialog::isFirstTimeStarted())
    {
        const auto res = QMessageBox::question(this, tr("User"), tr("There is no user information. Would you like to set it?"), QMessageBox::Yes | QMessageBox::No);
        if (res == QMessageBox::No)
        {
            exit(EXIT_FAILURE);
        }
        else
        {
            onShowSettings();
        }
    }
    else
    {
        LoginDialog dlg(this);
        dlg.setNormalMode();
#ifndef TEST_LOGIN_MODE
        if (dlg.exec() != QDialog::Accepted)
        {
            exit(EXIT_FAILURE);
        }
#else
        dlg.onAccept();
#endif
        updateUserInformationStatus();
    }
}

CentaurApp::~CentaurApp()
{
    for (auto &pli : _impl->configurationInterface)
    {
        delete pli.second;
    }
    if (m_sqlFavorites != nullptr)
    {
        delete m_sqlFavorites;
        m_sqlFavorites = nullptr;
    }
    if (g_logger != nullptr)
    {
        g_logger->terminate();
        m_loggerThread->join();
    }

    delete g_globals;
}

Ui::CentaurApp *CentaurApp::ui()
{
    return _impl->ui.get();
}

LogDialog *CentaurApp::logDialog() noexcept
{
    return _impl->logDialog;
}

void CentaurApp::mapConfigurationInterface(const uuid &id, PluginConfiguration *config)
{
    _impl->configurationInterface[id] = config;
}

void CentaurApp::mapExchangePlugin(const uuid &id, const ExchangeInformation &info)
{
    _impl->exchangeList[id] = info;
}

void CentaurApp::mapPluginInstance(QPluginLoader *loader)
{
    _impl->pluginInstances.push_back(loader);
}

void CentaurApp::mapExchangePluginViewMenus(const uuid &plugin, const QList<QAction *> &actions)
{
    _impl->exchangeMenuActions[plugin] = actions;
}

void CentaurApp::mapStatusPlugins(const uuid &plugin, CENTAUR_PLUGIN_NAMESPACE::IStatus *status, QToolButton *button, CENTAUR_PLUGIN_NAMESPACE::IStatus::DisplayMode mode)
{
    _impl->statusPlugins[plugin] = { status, button, mode };
}

void CentaurApp::mapCandleViewSupport(CENTAUR_PLUGIN_NAMESPACE::ICandleView *candle, const CandleViewSupport &view)
{
    _impl->candleViewSupport[candle] = view;
}

CENTAUR_PLUGIN_NAMESPACE::ICandleView *CentaurApp::getSupportedCandleViewPlugins(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &id)
{
    for (auto &cvs : _impl->candleViewSupport)
    {
        for (auto &spexch : cvs.second.info)
        {
            if (spexch == id)
                return cvs.first;
        }
    }

    return nullptr;
}

std::optional<std::reference_wrapper<const QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>> CentaurApp::getCandleViewTimeframeSupport(CENTAUR_PLUGIN_NAMESPACE::ICandleView *id) const
{
    auto it = _impl->candleViewSupport.find(id);
    if (it == _impl->candleViewSupport.end())
        return std::nullopt;

    return std::optional<std::reference_wrapper<const QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>> { std::cref(it->second.timeframes) };
}

void CentaurApp::closeEvent(QCloseEvent *event)
{
    if (m_server)
    {
        // Stop the server
        m_server->close();
        m_server.reset();
    }

    for (const auto &plugins : _impl->pluginInstances)
    {
        if (plugins->isLoaded())
            plugins->unload();
    }

    saveInterfaceState();

    event->accept();
}

bool CentaurApp::eventFilter(QObject *obj, QEvent *event)
{
    /* TODO: THIS
    if (obj == ui()->bidsChart && event->type() == QEvent::Resize && ui()->bidsChart->chart() != nullptr)
    {
        auto resize    = dynamic_cast<QResizeEvent *>(event);
        auto bidsChart = ui()->bidsChart->chart();
        auto size      = resize->size();
        bidsChart->setPlotArea(QRectF(70, 0, static_cast<qreal>(size.width()) - 70, static_cast<qreal>(size.height()) - 20));
    }

    if (obj == ui()->asksChart && event->type() == QEvent::Resize && ui()->asksChart->chart() != nullptr)
    {
        auto resize    = dynamic_cast<QResizeEvent *>(event);
        auto asksChart = ui()->asksChart->chart();
        auto size      = resize->size();
        asksChart->setPlotArea(QRectF(0, 0, static_cast<qreal>(size.width()) - 70, static_cast<qreal>(size.height()) - 20));
    }
*/
    return QObject::eventFilter(obj, event);
}

void CentaurApp::initializeDatabaseServices() noexcept
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(g_globals->paths.resPath + "/Local/centaur.db");

    if (!db.open())
    {
        logFatal("app", tr("The Centaur database could not be opened"));
        QMessageBox::critical(this, tr("Error"), tr("The Centaur database could not be opened"));
        exit(EXIT_FAILURE);
    }

    m_sqlFavorites = new FavoritesDBManager;
}

void CentaurApp::initializeInterface() noexcept
{
    logTrace("app", "CentaurApp::initializeInterface()");

    ui()->closeButton->setButtonClass(SystemPushButton::ButtonClass::close);
    ui()->minimizeButton->setButtonClass(SystemPushButton::ButtonClass::minimize);
#ifdef Q_OS_MAC
    ui()->maximizeButton->setButtonClass(SystemPushButton::ButtonClass::fullscreen);
    ui()->maximizeButton->linkFullScreen(ui()->closeButton, ui()->minimizeButton);
#else
    ui()->maximizeButton->setButtonClass(SystemPushButton::ButtonClass::maximize);
    ui()->maximizeButton->linkMaximize(ui()->closeButton, ui()->minimizeButton);
#endif

    connect(ui()->settingsButton, &QPushButton::released, this, &CentaurApp::onShowSettings);
    connect(ui()->pluginsViewButton, &QPushButton::released, this, &CentaurApp::onShowPlugins);
    connect(ui()->logsViewButtton, &QPushButton::released, this, &CentaurApp::onShowLogDialog);

    // Connect favorites action
    connect(ui()->actionWatchlistFavorites,
        &QAction::triggered,
        this,
        [&](C_UNUSED bool triggered) {
            FavoritesDialog dlg(this);
            dlg.setModal(true);
            dlg.exec();
        });

    connect(ui()->watchlistButton, &QPushButton::clicked, this, [&](bool clicked) {
        if (clicked)
            ui()->stackedWidget->setCurrentWidget(ui()->watchListPage);
    });

    connect(_impl->orderbookDepth, &QAction::triggered, this, [&, action = _impl->orderbookDepth](C_UNUSED bool trigger) {
        const auto actionData = _impl->orderbookDepth->data().value<OrderBookDepthInformation>();
        QString objectName    = QString("%1%2orderbook").arg(actionData.source, actionData.symbol);

        auto *dlgExists = this->findChild<OrderbookDialog *>(objectName);
        if (dlgExists != nullptr)
        {
            // Bring upfront
            dlgExists->show();
            dlgExists->setWindowState(Qt::WindowState::WindowActive);
        }
        else
        {
            auto *dlg = new OrderbookDialog(actionData.symbol, _impl->exchangeList[uuid(actionData.source.toStdString())].exchange, this);
            dlg->setObjectName(objectName);
            connect(dlg, &OrderbookDialog::closeButtonPressed, this, [&, dlg]() {
                delete dlg;
            });

            dlg->show();
        }
    });

    connect(_impl->depthChart, &QAction::triggered, this, [&, action = _impl->depthChart](C_UNUSED bool trigger) {
        const auto actionData       = _impl->depthChart->data().value<OrderBookDepthInformation>();
        QString orderBookObjectName = QString("%1%2orderbook").arg(actionData.source, actionData.symbol);

        auto *orderBookExists = this->findChild<OrderbookDialog *>(orderBookObjectName);
        if (orderBookExists == nullptr)
        {
            QMessageBox::warning(this, tr("Warning"), tr("The orderbook has to be displayed"), QMessageBox::Ok);
            return;
        }

        QString objectName = QString("%1%2depth").arg(actionData.source, actionData.symbol);
        auto *dlgExists    = this->findChild<DepthChartDialog *>(objectName);
        if (dlgExists != nullptr)
        {
            // Bring upfront
            dlgExists->show();
            dlgExists->setWindowState(Qt::WindowState::WindowActive);
        }
        else
        {
            auto *dlg = new DepthChartDialog(actionData.symbol, this);
            dlg->setObjectName(objectName);

            connect(orderBookExists, &OrderbookDialog::redirectOrderbook, dlg, &DepthChartDialog::onOrderBookDepth);

            connect(dlg, &DepthChartDialog::closeButtonPressed, this, [&, dlg, orderBookExists]() {
                // disconnect the orderbook signal
                disconnect(orderBookExists, &OrderbookDialog::redirectOrderbook, nullptr, nullptr);
                delete dlg;
            });

            dlg->show();
        }
    });

    ui()->watchListTable->initialize(ui()->watchListSearch, 6, -1, 5, nullptr);
    ui()->watchListTable->sortByColumn(0, Qt::AscendingOrder);
    ui()->watchListTable->getModel()->setHorizontalHeaderLabels({ tr("Symbol"), tr("Price"), tr("Source"), tr("Latency"), tr("UUID"), tr("Options") });
    ui()->watchListTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui()->watchListTable->horizontalHeader()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);
    ui()->watchListTable->setButtons(OptionsWidget::view | OptionsWidget::del);

    ui()->watchListTable->setDeletionColumnData(0);
    connect(ui()->watchListTable, &OptionsTableWidget::viewItemPressed, this, [&](QStandardItem *item) {
        auto index     = ui()->watchListTable->getItemIndexFromSource(item);
        QString symbol = ui()->watchListTable->getItemIndex(index.row(), 0).data(Qt::DisplayRole).toString();
        QString source = ui()->watchListTable->getItemIndex(index.row(), 4).data(Qt::DisplayRole).toString();
        auto actions   = _impl->exchangeMenuActions[uuid { source.toStdString() }];

        QMenu menu(this);

        auto exchBase = dynamic_cast<CENTAUR_PLUGIN_NAMESPACE::IBase *>(_impl->exchangeList[uuid { source.toStdString() }].exchange);
        if (exchBase != nullptr)
        {
            const auto pluginInformation = pluginInformationFromBase(exchBase);
            auto *candleView             = getSupportedCandleViewPlugins(pluginInformation);
            if (candleView != nullptr)
            {
            }
        }

        OrderBookDepthInformation obdi { symbol, source };

        _impl->orderbookDepth->setData(QVariant::fromValue(obdi));
        _impl->depthChart->setData(QVariant::fromValue(obdi));

        menu.addAction(_impl->orderbookDepth);
        menu.addAction(_impl->depthChart);
        menu.addSeparator();

        for (auto &action : actions)
        {
            // Update the symbol name
            action->setData(symbol);
            menu.addAction(action);
        }

        auto top   = ui()->watchListTable->mapToGlobal(ui()->watchListTable->visualRect(index).topLeft()); // To get the top
        auto right = ui()->watchListTable->mapToGlobal(ui()->watchListTable->frameGeometry().topRight());  // To get the right

        menu.exec({ right.x(), top.y() });
    });

    connect(ui()->watchListTable, &OptionsTableWidget::deleteItemPressed, this, [&](C_UNUSED const QString &itemData) {
        onRemoveWatchList(ui()->watchListTable->getRemovedItem());
    });

    ui()->watchListTable->setColumnHidden(4, true);

    connect(ui()->watchListTable, &OptionsTableWidget::itemSelectionChanged, this,
        [&](const QModelIndex &index) {
            if (index.isValid())
            {
                const auto source    = ui()->watchListTable->getItemIndex(index.row(), 4).data().toString();
                const QString symbol = [&]() -> QString {
                    if (index.column() == 0)
                        return index.data().toString();
                    else
                        return ui()->watchListTable->getItemIndex(index.row(), 0).data().toString();
                }();
                onSetWatchlistSelection(source, symbol);
            }
        });

    // Init Last-7-day chart
    _impl->last7Chart = new QChart;
    _impl->last7Chart->setAnimationOptions(QChart::AllAnimations);
    _impl->last7Chart->setBackgroundPen(Qt::NoPen);
    _impl->last7Chart->setBackgroundBrush(Qt::NoBrush);
    _impl->last7Chart->setTitleBrush(QColor(200, 200, 200));
    ui()->sevenDayGraph->setChart(_impl->last7Chart);
    ui()->sevenDayGraph->setRenderHint(QPainter::Antialiasing);
    _impl->last7SevenLowSeries  = new QSplineSeries;
    _impl->last7SevenUpSeries   = new QSplineSeries;
    _impl->last7SevenAreaSeries = new QAreaSeries(_impl->last7SevenLowSeries, _impl->last7SevenUpSeries);
    _impl->last7SevenAreaSeries->setPen(Qt::NoPen);
    _impl->last7Chart->legend()->hide();

    _impl->last7Chart->addSeries(_impl->last7SevenAreaSeries);

    auto *axisX = new QDateTimeAxis;
    axisX->setTickCount(7);
    axisX->setFormat("MMM d");
    axisX->setTitleVisible(false);
    axisX->setGridLineVisible(false);
    axisX->setLabelsColor(QColor(200, 200, 200));
    _impl->last7Chart->addAxis(axisX, Qt::AlignBottom);
    _impl->last7SevenAreaSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis;
    axisY->setLabelFormat("$ %.2f");
    axisY->setTitleVisible(false);
    axisY->setGridLineVisible(false);
    axisY->setLabelsColor(QColor(200, 200, 200));
    _impl->last7Chart->addAxis(axisY, Qt::AlignLeft);
    _impl->last7SevenAreaSeries->attachAxis(axisY);

    _impl->last7Chart->setMargins(QMargins(0, 0, 0, 0));

    /* TODO: THIS
    // Balances Tree
    ui()->ctrlBalances->setHeaderLabels({ LS("ui-docks-balances-name"), LS("ui-docks-balances-value") });
    ui()->ctrlBalances->setIconSize(QSize(28, 28));

     */
}

void CentaurApp::saveInterfaceState() noexcept
{
    logTrace("app", "CentaurApp::saveInterfaceState()");

    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("mainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();

    settings.beginGroup("watchlistListState");
    settings.setValue("geometry", ui()->watchListTable->saveGeometry());
    settings.setValue("h-geometry", ui()->watchListTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->watchListTable->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("LogDialog");
    settings.setValue("geometry", _impl->logDialog->saveGeometry());
    settings.endGroup();

    settings.beginGroup("loggingListState");
    settings.setValue("geometry", _impl->logDialog->tableWidget()->saveGeometry());
    settings.setValue("h-geometry", _impl->logDialog->tableWidget()->horizontalHeader()->saveGeometry());
    settings.setValue("state", _impl->logDialog->tableWidget()->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("Splitter0");
    settings.setValue("geometry", ui()->splitter->saveGeometry());
    settings.setValue("state", ui()->splitter->saveState());
    settings.endGroup();

    /* TODO: THIS
    settings.beginGroup("BalancesTreeState");
    settings.setValue("geometry", ui()->ctrlBalances->saveGeometry());
    settings.setValue("h-geometry", ui()->ctrlBalances->header()->saveGeometry());
    settings.setValue("state", ui()->ctrlBalances->header()->saveState());
    settings.endGroup();


*/
    logInfo("app", "UI state saved");
}

void CentaurApp::loadInterfaceState() noexcept
{
    logTrace("app", "CentaurApp::loadInterfaceState()");

    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("mainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("watchlistListState");
    ui()->watchListTable->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->watchListTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->watchListTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("Splitter0");
    ui()->splitter->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->splitter->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    /* TODO: THIS
    settings.beginGroup("BalancesTreeState");
    ui()->ctrlBalances->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->ctrlBalances->header()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->ctrlBalances->header()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

*/
    logInfo("app", "UI state loaded");
}

void CentaurApp::onStatusDisplayChanged(plugin::IStatus::DisplayRole mode)
{
    using namespace CENTAUR_PLUGIN_NAMESPACE;
    plugin::IStatus *sndr = qobject_cast<plugin::IStatus *>(sender());
    if (sndr == nullptr)
    {
        logError("IStatus", "The IStatus sender could not be determinate");
        return;
    }

    auto data = _impl->statusPlugins.find(sndr->getPluginUUID());
    if (data == _impl->statusPlugins.end())
    {
        logError("IStatus", "The IStatus data could not be found");
        return;
    }

    if (mode == plugin::IStatus::DisplayRole::Foreground)
    {
        QToolButton *button = std::get<1>(data->second);
        button->setFont(sndr->font());
        button->setText(sndr->text());

        constexpr static char stylesheet[] = { R"(QToolButton{border-radius: 0px; border: 0px; }
QToolButton:hover{background-color: qlineargradient(x1:0.5, y1: 0, x2:0.5, y2:1, stop: 0  rgb(58,58,58), stop: 1 rgb(68,68,68)); border-radius: 0px;}
QToolButton:pressed{background-color: qlineargradient(x1:0.5, y1: 0, x2:0.5, y2:1, stop: 1  rgb(85,85,85), stop: 0 rgb(95,95,95)); border-radius: 0px;})" };

        const auto foregroundBrush = sndr->brush(IStatus::DisplayRole::Foreground);
        const auto backgroundBrush = sndr->brush(IStatus::DisplayRole::Background);

        QPalette palette = button->palette();
        if (foregroundBrush != Qt::NoBrush)
        {
            palette.setBrush(QPalette::ColorRole::ButtonText, sndr->brush(IStatus::DisplayRole::Foreground));
        }
        if (backgroundBrush != Qt::NoBrush)
        {
            button->setStyleSheet("");
            button->setAutoFillBackground(true);
            palette.setBrush(QPalette::ColorRole::Button, sndr->brush(IStatus::DisplayRole::Background));
        }
        else
        {
            button->setStyleSheet(stylesheet);
            button->setAutoFillBackground(false);
        }
        button->setPalette(palette);
    }
}

void CentaurApp::loadFavoritesWatchList() noexcept
{
    auto data = m_sqlFavorites->selectAll();

    if (data.isEmpty())
    {
        logInfo("app", tr("No favorites in the list"));
        return;
    }

    for (const auto &[sym, plid] : data)
    {
        if (sym.isEmpty())
        {
            logWarn("app", tr("The symbol name in the favorites DB is empty"));
            continue;
        }

        if (plid.isEmpty())
        {
            logWarn("app", tr("The symbol id in the favorites DB is empty"));
            continue;
        }

        // Insert the element
        onAddToWatchList(sym, plid, false);
    }
}

void CentaurApp::startLoggingService() noexcept
{
    g_logger = new CentaurLogger;
    // Init the logger
    m_loggerThread = std::make_unique<std::thread>(&CentaurLogger::run, g_logger);
    try
    {
        g_logger->setApplication(this);
        g_logger->setUser("root");
        g_logger->setSession(0);
        logInfo("app", QString("TraderSys ") + QString(CentaurVersionString));
    } catch (const std::runtime_error &ex)
    {
        QMessageBox::critical(this,
            tr("Fatal error"),
            ex.what());
        QApplication::quit();
    }
}

void CentaurApp::startCommunicationsServer() noexcept
{
    /*
    m_serverStatus = new QLabel("Server", ui()->statusBar);
    ui()->statusBar->addPermanentWidget(m_serverStatus, 0);
    m_server = std::make_unique<ProtocolServer>(this);
    if (m_server->isListening())
        m_serverStatus->setStyleSheet(g_statusGreen);
    else
        m_serverStatus->setStyleSheet(g_statusRed);*/
}

void CentaurApp::onAddToWatchList(const QString &symbol, const QString &sender, bool addToDatabase) noexcept
{
    logTrace("watchlist", "CentaurApp::onAddToWatchList()");

    auto interface = _impl->exchangeList.find(uuid { sender.toStdString() });

    if (interface == _impl->exchangeList.end())
    {
        logError("watchlist", QString(tr("The sender '%1' is not registered.")).arg(sender));
        return;
    }

    auto watchInterface = interface->second.exchange;

    // Generate a unique-id
    const int id = ++m_sessionIds;

    if (watchInterface->addSymbolToWatchlist(symbol, id))
    {
        auto itemSymbol  = new QStandardItem(symbol);
        auto itemPrice   = new QStandardItem("$ 0.00");
        auto itemSource  = new QStandardItem(interface->second.listName);
        auto itemLatency = new QStandardItem("0");
        auto itemUUID    = new QStandardItem(interface->second.uuid.to_string().c_str());

        itemSymbol->setData(id, Qt::UserRole + 1); // To find the item
        itemPrice->setData(0., Qt::UserRole + 1);  // To compare to previous values

        itemSymbol->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemPrice->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        itemSource->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemLatency->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui()->watchListTable->insertRowWithOptions(
            ui()->watchListTable->getRowCount(),
            { itemSymbol, itemPrice, itemSource, itemLatency, itemUUID, new QStandardItem() },
            false);

        // Add to the database
        if (addToDatabase)
            addFavoritesWatchListDB(symbol, sender);

        QLinearGradient gr_p({ 0, 0 }, { 1, 1 });
        gr_p.setCoordinateMode(QGradient::CoordinateMode::ObjectMode);
        gr_p.setColorAt(0, QColor(1, 166, 7));
        gr_p.setColorAt(0.5, QColor(26, 175, 32));
        gr_p.setColorAt(1, QColor(1, 133, 6));

        QLinearGradient gr_n({ 0, 0 }, { 1, 1 });
        gr_n.setCoordinateMode(QGradient::CoordinateMode::ObjectMode);
        gr_n.setColorAt(0, QColor(167, 29, 7));
        gr_n.setColorAt(0.5, QColor(176, 52, 32));
        gr_n.setColorAt(1, QColor(134, 23, 6));

        auto dataList = watchInterface->getWatchlist24hrPriceChange();

        std::transform(dataList.begin(), dataList.end(), dataList.begin(), [](const std::pair<qreal, QString> &data) -> std::pair<qreal, QString> {
            return { data.first, QString(R"(<font size="12"><b>%1</b></font><br>%2 %)").arg(data.second).arg(data.first, 'f', '4') };
        });

        ui()->widget->setData(dataList, gr_n, gr_p);
    }
    else
    {
        logError("watchlist", QString("Symbol %1 was not added to the watchlist").arg(symbol));
    }
}
void CentaurApp::onTickerUpdate(const QString &symbol, int id, quint64 receivedTime, double price) noexcept
{
    QString source;
    QStandardItem *item = nullptr;
    for (int r = 0; r < ui()->watchListTable->getRowCount(); ++r)
    {
        item = ui()->watchListTable->getModel()->item(r, 0);

        if (item->data() == id)
        {
            source = ui()->watchListTable->getModel()->item(r, 4)->text();
            break;
        }
    }
    if (item == nullptr)
    {
        logError("wlTickerUpdate", QString("Watchlist item for the symbol '%1' was not found").arg(symbol));
        return;
    }

    const int itemRow = ui()->watchListTable->getItemIndexFromSource(item).row();

    if (itemRow == -1)
    {
        // Item is not visible
        return;
    }

    const double previousPrice = ui()->watchListTable->getItemData(itemRow, 1, Qt::UserRole + 1).toReal();

    if (price > previousPrice)
    {
        item->setIcon(g_globals->icons.upArrow);
        ui()->watchListTable->setItemData(itemRow, 1, Qt::ForegroundRole, QBrush(QColor(0, 255, 0)));
    }
    else if (price < previousPrice)
    {
        item->setIcon(g_globals->icons.downArrow);
        ui()->watchListTable->setItemData(itemRow, 1, Qt::ForegroundRole, QBrush(QColor(255, 129, 112)));
    }
    else
    {
        item->setIcon(g_globals->icons.downArrow);
        ui()->watchListTable->setItemData(itemRow, 1, Qt::ForegroundRole, QBrush(QColor(238, 238, 238)));
    }

    ui()->watchListTable->setItemText(itemRow, 1, "$ " + QLocale(QLocale::English).toString(price, 'f', 2));
    ui()->watchListTable->setItemData(itemRow, 1, Qt::UserRole + 1, price); // Store the current since it will become the previous in the next call

    // Calculate latency
    const auto ms      = static_cast<quint64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto latency = receivedTime > ms ? 0ull : ms - receivedTime;

    if (latency <= 85)
    {
        ui()->watchListTable->setItemData(itemRow, 3, Qt::ForegroundRole, QBrush(QColor(0, 255, 0)));
    }
    else if (latency >= 86 && latency <= 185)
    {
        ui()->watchListTable->setItemData(itemRow, 3, Qt::ForegroundRole, QBrush(QColor(255, 215, 0)));
    }
    else if (latency >= 186 && latency <= 10000000)
    {
        ui()->watchListTable->setItemData(itemRow, 3, Qt::ForegroundRole, QBrush(QColor(255, 129, 112)));
    }
    ui()->watchListTable->setItemText(itemRow, 3, QString("%1 ms").arg(latency));

    // Determinate if the OrderBook dialog is being shown
    QString orderBookObjectName = QString("%1%2orderbook").arg(source, symbol);
    auto *orderbookDlg          = this->findChild<OrderbookDialog *>(orderBookObjectName);
    if (orderbookDlg != nullptr)
        orderbookDlg->setPrice(price);

    QString depthCharDepthObjectName = QString("%1%2depth").arg(source, symbol);
    auto *depthDlg                   = this->findChild<DepthChartDialog *>(depthCharDepthObjectName);
    if (depthDlg != nullptr)
        depthDlg->setPrice(price);
}

void CentaurApp::onRemoveWatchList(const QModelIndex &index) noexcept
{
    logTrace("watchlist", "CentaurApp::onRemoveWatchList");
    /// Retrieve the IExchange from the row based on the 5 column which has the PluginUUID Source

    ui()->watchListTable->getItemText(index.row(), 0);

    QString itemSymbol = ui()->watchListTable->getItemText(index.row(), 0);
    QString itemSource = ui()->watchListTable->getItemText(index.row(), 4);

    auto interfaceIter = _impl->exchangeList.find(uuid(itemSource.toStdString()));
    if (interfaceIter == _impl->exchangeList.end())
    {
        QString message = QString(tr("Failed to locate the symbol interface."));
        logError("wlRemove", message);
        QMessageBox box;
        box.setText(tr("Could remove the symbol"));
        box.setInformativeText(message);
        box.setIcon(QMessageBox::Critical);
        box.exec();
        return;
    }

    // Call the plugin to inform that it must not send data of the symbol anymore
    auto &exchInfo = interfaceIter->second;

    exchInfo.exchange->removeSymbolFromWatchlist(itemSymbol);

    // Remove the row
    if (!ui()->watchListTable->removeItemRow(index))
        logError("watchlist", QString(tr("%1 was not removed from the UI list")).arg(itemSymbol));
    else
    {
        logInfo("watchlist", QString(tr("%1 was removed from the UI list")).arg(itemSymbol));
        m_sqlFavorites->del(itemSymbol, itemSource);
    }
}

void CentaurApp::onSetWatchlistSelection(const QString &source, const QString &symbol) noexcept
{
    std::pair<QString, QString> selection = { source, symbol };

    if (_impl->currentWatchListSelection == selection)
        return;

    auto itemIter = _impl->exchangeList.find(uuid(source.toStdString()));
    if (itemIter == _impl->exchangeList.end())
    {
        logError("wlOrderbookSend", QString("Watchlist item for the symbol %1 was not found").arg(symbol));
        return;
    }

    const auto &exchInfo = itemIter->second;

    plotSevenDaysChart(symbol, exchInfo.exchange->get7dayData(symbol));
    _impl->currentWatchListSelection = selection;
}

void CentaurApp::plotSevenDaysChart(const QString &symbol, const QList<std::pair<quint64, qreal>> &values) noexcept
{
    // Prepare the data
    if (values.size() < 7)
    {
        logWarn("7plot", "items less than 7");
        return;
    }

    if (values.size() > 7)
    {
        logWarn("7plot", "list is larger than 7; data will be truncated");
    }

    _impl->last7SevenLowSeries->clear();
    _impl->last7SevenUpSeries->clear();

    const auto yMinPrice = std::min_element(values.begin(), values.end(),
        [](const std::pair<quint64, qreal> &p1, const std::pair<quint64, qreal> &p2) {
            return p1.second < p2.second;
        })->second;
    const auto yMaxPrice = std::max_element(values.begin(), values.end(),
        [](const std::pair<quint64, qreal> &p1, const std::pair<quint64, qreal> &p2) {
            return p1.second < p2.second;
        })->second;

    const auto yMinTime = std::min_element(values.begin(), values.end(),
        [](const std::pair<quint64, qreal> &p1, const std::pair<quint64, qreal> &p2) {
            return p1.first < p2.first;
        })->first;
    const auto yMaxTime = std::max_element(values.begin(), values.end(),
        [](const std::pair<quint64, qreal> &p1, const std::pair<quint64, qreal> &p2) {
            return p1.first < p2.first;
        })->first;

    // Leave a space in the Y axis
    const auto minPos = yMinPrice - ((yMaxPrice - yMinPrice) / 2);

    qreal averageChange = 0.0;
    for (qsizetype i = values.size() - 7; i < values.size(); ++i) // If list size > 7 the -7 will truncate it to only the last 7
    {
        const auto &[time, price] = values[i];

        *_impl->last7SevenLowSeries << QPointF { static_cast<qreal>(time), minPos };
        *_impl->last7SevenUpSeries << QPointF { static_cast<qreal>(time), price };

        if (i > 0)
        {
            averageChange += price - values[i - 1].second;
        }
    }

    averageChange /= 6;

    if (averageChange > 0)
    {
        QLinearGradient gr_p({ 0.5, 0 }, { 0.5, 1 });
        gr_p.setCoordinateMode(QGradient::CoordinateMode::ObjectMode);
        gr_p.setColorAt(0, QColor(1, 166, 7, 255));
        gr_p.setColorAt(.8, QColor(1, 133, 6, 128));
        _impl->last7SevenAreaSeries->setBrush(gr_p);
    }
    else
    {
        QLinearGradient gr_n({ 0.5, 0 }, { 0.5, 1 });
        gr_n.setCoordinateMode(QGradient::CoordinateMode::ObjectMode);
        gr_n.setColorAt(0, QColor(167, 29, 7, 255));
        gr_n.setColorAt(.8, QColor(167, 29, 7, 128));
        _impl->last7SevenAreaSeries->setBrush(gr_n);
    }

    _impl->last7Chart->setTitle(QString(tr("%1 7-days")).arg(symbol));

    auto *xAxis = qobject_cast<QDateTimeAxis *>(_impl->last7Chart->axes(Qt::Horizontal).first());
    xAxis->setRange(
        QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(yMinTime)),
        QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(yMaxTime)));
    _impl->last7Chart->axes(Qt::Vertical).first()->setRange(minPos, yMaxPrice);
}

void CentaurApp::onShowPlugins() noexcept
{
    QGraphicsBlurEffect blur;
    setGraphicsEffect(&blur);

    PluginsDialog dlg(this);
    dlg.exec();
}

void CentaurApp::onShowLogDialog() noexcept
{
    // Just show the dialog
    if (!_impl->logDialog->isVisible())
        _impl->logDialog->show();
}

void CentaurApp::onShowSettings() noexcept
{
    SettingsDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted)
    {
        updateUserInformationStatus();
    }
}

void CentaurApp::updateUserInformationStatus() noexcept
{
    if (!g_globals->session.image.isNull())
    {
        QPixmap displayedImage = QPixmap::fromImage(g_globals->session.image.scaled({ 48, 48 }));
        QBitmap map(48, 48);
        map.fill(Qt::color0);
        QPainter painter(&map);
        painter.setBrush(Qt::color1);
        painter.setRenderHint(QPainter::RenderHint::Antialiasing);
        painter.drawRoundedRect(0, 0, 48, 48, 24, 24);
        displayedImage.setMask(map);

        ui()->userImage->setPixmap(displayedImage);
    }

    QString userDataString = QString("%1 (%2)").arg(g_globals->session.display, g_globals->session.email);
    ui()->userData->setText(userDataString);
}

void CentaurApp::addFavoritesWatchListDB(const QString &symbol, const QString &sender) noexcept
{
    m_sqlFavorites->add(symbol, sender);
}

CENTAUR_PLUGIN_NAMESPACE::IExchange *CentaurApp::exchangeFromWatchlistRow(int row) noexcept
{
    // TODO: THIS
    // const QString itemSource = m_watchlistItemModel->item(row, 4)->text();
    // return exchangeFromWatchlistRow(itemSource);

    return nullptr;
}

CENTAUR_PLUGIN_NAMESPACE::IExchange *CentaurApp::exchangeFromWatchlistRow(const QString &sender) noexcept
{
    /*
    auto interface = m_exchangeList.find(sender);

    if (interface == m_exchangeList.end())
    {
        logError("watchlist", QString(tr("The sender '%1' is not registered.")).arg(sender));
        return nullptr;
    }

    return interface->second.exchange;*/
    return nullptr;
}

void CentaurApp::onCandleView(const QString &symbol, plugin::ICandleView *view, plugin::ICandleView::TimeFrame tf, const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &emitter) noexcept
{ /* TODO: THIS
     auto subWindow = new QMdiSubWindow;

     bool unique = false;
     uuid uid { uuid::generate() };

     while (!unique)
     {
         if (m_candleViewDisplay.contains(uid))
             uid = uuid::generate();
         else
         {
             m_candleViewDisplay[uid] = { subWindow, symbol, view, tf };
             unique                   = true;
         }
     }

     subWindow->setWidget(new CandleViewWidget(emitter, uid, symbol, view, tf, subWindow));

     subWindow->setAttribute(Qt::WA_DeleteOnClose);
     ui()->mdiArea->addSubWindow(subWindow);
     subWindow->show();
     */
}

void CentaurApp::onRealTimeCandleUpdate(const uuid &id, quint64 eventTime, plugin::ICandleView::Timestamp ts, const plugin::ICandleView::CandleData &cd) noexcept
{
    auto iter = m_candleViewDisplay.find(id);
    if (iter == m_candleViewDisplay.end())
    {
        logError("onRealTimeCandleUpdate", tr("Can not find the CandleView"));
        return;
    }
    auto &info  = iter->second;
    auto widget = std::get<0>(info)->widget();

    QMetaObject::invokeMethod(widget,
        "onUpdateCandle",
        Qt::QueuedConnection,
        Q_ARG(quint64, eventTime),
        Q_ARG(plugin::ICandleView::Timestamp, ts),
        Q_ARG(plugin::ICandleView::CandleData, cd));
}

END_CENTAUR_NAMESPACE
