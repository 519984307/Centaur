/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "CentaurApp.hpp"
#include "FavoritesDialog.hpp"
#include "HTMLDelegate.hpp"
#include "PluginsDialog.hpp"
#include <QAreaSeries>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtCharts/QValueAxis>

CENTAUR_NAMESPACE::CentaurApp *CENTAUR_NAMESPACE::g_app = nullptr;
namespace CENTAUR_NAMESPACE
{
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
                logError("app", QString(LS("error-fav-db-insert").arg(m_insert.lastError().text())));
            }
        }

        inline QList<QPair<QString, QString>> selectAll()
        {
            QList<QPair<QString, QString>> data;
            if (!m_select.exec())
            {
                logError("app", QString(LS("error-fav-db-select").arg(m_select.lastError().text())));
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
                logError("app", QString(LS("error-fav-db-delete").arg(m_delete.lastError().text())));
            }
        }

    private:
        QSqlQuery m_insert;
        QSqlQuery m_delete;
        QSqlQuery m_select;
        QSqlDatabase m_db;
    };
} // namespace CENTAUR_NAMESPACE

namespace
{
    constexpr char g_statusRed[] {
        R"(QLabel{
color: rgb(255, 255, 255);
background-color: rgb(172, 6, 0);
border-radius: 3px;
min-width: 85px;
qproperty-alignment: AlignCenter;
})"
    };

    constexpr char g_statusGreen[] {
        R"(QLabel{
color: rgb(255, 255, 255);
background-color: rgb(0, 104, 18);
border-radius: 3px;
min-width: 85px;
qproperty-alignment: AlignCenter;
})"
    };

} // namespace

#include <QImageReader>
CENTAUR_NAMESPACE::CentaurApp::CentaurApp(QWidget *parent) :
    QMainWindow(parent),
    m_ui { std::make_unique<Ui::CentaurApp>() }
{
    START_TIME(initializationTimeStart);

    g_app     = this;
    g_globals = new Globals;

    m_ui->setupUi(this);
    installEventFilter(this);

    m_ui->bidsChart->installEventFilter(this);
    m_ui->asksChart->installEventFilter(this);

#ifdef Q_OS_MAC
    CFURLRef appUrlRef       = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath      = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    g_globals->paths.appPath = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
    CFRelease(appUrlRef);
    CFRelease(macPath);
#else

#endif /*Q_OS_MAC*/

    g_globals->paths.installPath = "/Volumes/RicardoESSD/Projects/Centaur/local";
    g_globals->paths.pluginsPath = g_globals->paths.installPath + "/Plugin";
    g_globals->paths.resPath     = g_globals->paths.installPath + "/Resources";

    // Start logging service
    startLoggingService();

    // Load External XML Configuration Data
    loadConfigurationData();

    // Init the database internal file
    initializeDatabaseServices();

    // Restore file
    loadInterfaceState();

    // Start the interface
    initializeInterface();

    // Start the server
    startCommunicationsServer();

    // Load plugins
    loadPlugins();

    // Load the favorites list. Since all plugins must be loaded
    loadFavoritesWatchList();

    END_TIME_SEC(initializationTimeStart, initializationTimeEnd, initializationTime);
    logInfo("app", QString(LS("trace-initialize-time")).arg(initializationTime.count(), 0, 'f', 4));
}
CENTAUR_NAMESPACE::CentaurApp::~CentaurApp()
{
    for (auto &pli : m_configurationInterface)
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

void cen::CentaurApp::closeEvent(QCloseEvent *event)
{
    if (m_server)
    {
        // Stop the server
        m_server->close();
        m_server.reset();
    }

    for (const auto &plugins : m_pluginInstances)
    {
        if (plugins->isLoaded())
            plugins->unload();
    }

    QWidget::closeEvent(event);
}

bool CENTAUR_NAMESPACE::CentaurApp::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this && event->type() == QEvent::Close)
        saveInterfaceState();

    if (obj == m_ui->bidsChart && event->type() == QEvent::Resize && m_ui->bidsChart->chart() != nullptr)
    {
        auto resize    = dynamic_cast<QResizeEvent *>(event);
        auto bidsChart = m_ui->bidsChart->chart();
        auto size      = resize->size();
        bidsChart->setPlotArea(QRectF(70, 0, static_cast<qreal>(size.width()) - 70, static_cast<qreal>(size.height()) - 20));
    }

    if (obj == m_ui->asksChart && event->type() == QEvent::Resize && m_ui->asksChart->chart() != nullptr)
    {
        auto resize    = dynamic_cast<QResizeEvent *>(event);
        auto asksChart = m_ui->asksChart->chart();
        auto size      = resize->size();
        asksChart->setPlotArea(QRectF(0, 0, static_cast<qreal>(size.width()) - 70, static_cast<qreal>(size.height()) - 20));
    }

    return QObject::eventFilter(obj, event);
}

void cen::CentaurApp::initializeDatabaseServices() noexcept
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(g_globals->paths.resPath + "/Local/centaur.db");

    if (!db.open())
    {
        logFatal("app", LS("fatal-centaur-db"));
        QMessageBox::critical(this, LS("error-error"), LS("fatal-centaur-db"));
        exit(EXIT_FAILURE);
    }

    m_sqlFavorites = new FavoritesDBManager;
}

void CENTAUR_NAMESPACE::CentaurApp::initializeInterface() noexcept
{
    logTrace("app", "CENTAUR_NAMESPACE::CentaurApp::initializeInterface()");

    auto *aboutMenu         = new QMenu,
         *preferencesMenu   = new QMenu,
         *pluginsMenu       = new QMenu;
    auto *aboutAction       = new QAction(tr("About Centaur"));
    auto *preferencesAction = new QAction(tr("Preferences"));
    auto *pluginsAction     = new QAction(tr("Plugins"));

    aboutAction->setMenuRole(QAction::MenuRole::AboutQtRole);
    preferencesAction->setMenuRole(QAction::MenuRole::PreferencesRole);
    pluginsAction->setMenuRole(QAction::MenuRole::ApplicationSpecificRole);
    aboutMenu->addAction(aboutAction);
    preferencesMenu->addAction(preferencesAction);
    pluginsMenu->addAction(pluginsAction);
    m_ui->menuBar->addMenu(aboutMenu);
    m_ui->menuBar->addMenu(preferencesMenu);
    m_ui->menuBar->addMenu(pluginsMenu);

    connect(pluginsAction, SIGNAL(triggered()), this, SLOT(onPlugins()));

    // Connect favorites action
    connect(m_ui->actionWatchlistFavorites,
        &QAction::triggered,
        this,
        [&](C_UNUSED bool triggered) {
            FavoritesDialog dlg(this);
            dlg.setModal(true);
            dlg.exec();
        });

    m_ui->statusBar->setStyleSheet("QStatusBar::item { border: 2px; }");

    m_ui->tabSymbols->setTabText(m_ui->tabSymbols->indexOf(m_ui->tabWatchList), LS("ui-docks-watchlist"));
    // Menu actions
    connect(m_ui->actionTileWindows, &QAction::triggered, this, &CentaurApp::onActionTileWindowsTriggered);
    connect(m_ui->actionCascadeWindows, &QAction::triggered, this, &CentaurApp::onActionCascadeWindowsTriggered);
    connect(m_ui->actionAsks, &QAction::toggled, this, &CentaurApp::onActionAsksToggled);
    connect(m_ui->actionBalances, &QAction::toggled, this, &CentaurApp::onActionBalancesToggled);
    connect(m_ui->actionBids, &QAction::toggled, this, &CentaurApp::onActionBidsToggled);
    connect(m_ui->actionDepth, &QAction::toggled, this, &CentaurApp::onActionDepthToggled);
    connect(m_ui->actionLogging, &QAction::toggled, this, &CentaurApp::onActionLoggingToggled);
    connect(m_ui->actionSymbols, &QAction::toggled, this, &CentaurApp::onActionSymbolsToggled);

    // Remove the first index
    m_ui->dockSymbols->setWindowTitle(LS("ui-docks-symbols"));
    m_ui->tabSymbols->removeTab(1);

    // Show or hide the docking windows according to the store status of the Gm_ui
    m_ui->actionSymbols->setChecked(!m_ui->dockSymbols->isHidden());
    m_ui->actionLogging->setChecked(!m_ui->dockLogging->isHidden());
    m_ui->actionBalances->setChecked(!m_ui->dockBalances->isHidden());
    m_ui->actionBids->setChecked(!m_ui->dockOrderbookBids->isHidden());
    m_ui->actionAsks->setChecked(!m_ui->dockOrderbookAsks->isHidden());
    m_ui->actionDepth->setChecked(!m_ui->dockDepth->isHidden());

    // Init the watchlist QLineEdit
    m_ui->editWatchListFilter->setPlaceholderText(LS("ui-docks-search"));
    m_ui->editWatchListFilter->addAction(g_globals->icons.searchIcon, QLineEdit::LeadingPosition);
    m_watchlistItemModel = new QStandardItemModel(0, 4, m_ui->tabWatchList);
    auto sortProxyModel  = new QSortFilterProxyModel(m_ui->tabWatchList);
    sortProxyModel->setSourceModel(m_watchlistItemModel);
    m_ui->listWatchList->setModel(sortProxyModel);
    m_ui->listWatchList->setRemove();
    m_ui->listWatchList->allowClickMessages();
    connect(m_ui->listWatchList, &CENTAUR_NAMESPACE::CenListCtrl::snRemoveWatchList, this, &CentaurApp::onRemoveWatchList);
    connect(m_ui->listWatchList, &CENTAUR_NAMESPACE::CenListCtrl::snSetSelection, this, &CentaurApp::onSetWatchlistSelection);
    connect(m_ui->listWatchList, &CENTAUR_NAMESPACE::CenListCtrl::snRemoveSelection, this, &CentaurApp::onWatchlistRemoveSelection);

    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(m_ui->editWatchListFilter, &QLineEdit::textChanged, sortProxyModel, &QSortFilterProxyModel::setFilterFixedString);

    m_ui->listWatchList->verticalHeader()->setFont(g_globals->fonts.symbolsDock.headerFont);
    m_ui->listWatchList->sortByColumn(0, Qt::AscendingOrder);
    m_ui->listWatchList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_watchlistItemModel->setHorizontalHeaderLabels(
        { LS("ui-docks-symbol"),
            LS("ui-docks-price"),
            LS("ui-docks-source"),
            LS("ui-docks-latency"),
            "UUID" });

    m_watchlistItemModel->horizontalHeaderItem(0)->setFont(g_globals->fonts.symbolsDock.headerFont);
    m_watchlistItemModel->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_watchlistItemModel->horizontalHeaderItem(1)->setFont(g_globals->fonts.symbolsDock.headerFont);
    m_watchlistItemModel->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_watchlistItemModel->horizontalHeaderItem(2)->setFont(g_globals->fonts.symbolsDock.headerFont);
    m_watchlistItemModel->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_watchlistItemModel->horizontalHeaderItem(3)->setFont(g_globals->fonts.symbolsDock.headerFont);
    m_watchlistItemModel->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    // This column, although hidden, will be used to find uuid source of all watchlist items
    m_watchlistItemModel->horizontalHeaderItem(4)->setFont(g_globals->fonts.symbolsDock.headerFont);
    m_watchlistItemModel->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_ui->listWatchList->setColumnHidden(4, true);

    // Balances Tree
    m_ui->ctrlBalances->setHeaderLabels({ LS("ui-docks-balances-name"), LS("ui-docks-balances-value") });
    m_ui->ctrlBalances->setIconSize(QSize(28, 28));

    // Init the logging window
    QTableWidget *logger = m_ui->logsTable;
    logger->setHorizontalHeaderLabels({ "Date", "User", "Session", "Type", "Source", "Message" });
    logger->horizontalHeaderItem(0)->setFont(QFont("Arial", 10));
    logger->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    logger->horizontalHeaderItem(1)->setFont(QFont("Arial", 10));
    logger->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);
    logger->horizontalHeaderItem(2)->setFont(QFont("Arial", 10));
    logger->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignCenter);
    logger->horizontalHeaderItem(3)->setFont(QFont("Arial", 10));
    logger->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignCenter);
    logger->horizontalHeaderItem(4)->setFont(QFont("Arial", 10));
    logger->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignCenter);
    logger->horizontalHeaderItem(5)->setFont(QFont("Arial", 10));
    logger->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    logger->setItemDelegateForColumn(5, new HTMLDelegate);

    // Orderbook asks
    //    QString styleSheet = "::section {" // "QHeaderView::section {"
    //                        "font-family: arial;"
    //                       "font-size: 10px; }";

    m_ui->asksTable->setHorizontalHeaderLabels({ "Price", "Amount", "Total" });
    m_ui->asksTable->horizontalHeaderItem(0)->setFont(QFont("Arial", 10));
    m_ui->asksTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_ui->asksTable->horizontalHeaderItem(1)->setFont(QFont("Arial", 10));
    m_ui->asksTable->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight);
    m_ui->asksTable->horizontalHeaderItem(2)->setFont(QFont("Arial", 10));
    m_ui->asksTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignRight);

    //   m_ui->asksTable->verticalHeader()->setStyleSheet(styleSheet);

    // Orderbook bids
    m_ui->bidsTable->setHorizontalHeaderLabels({ "Price", "Amount", "Total" });
    m_ui->bidsTable->horizontalHeaderItem(0)->setFont(QFont("Arial", 10));
    m_ui->bidsTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    m_ui->bidsTable->horizontalHeaderItem(1)->setFont(QFont("Arial", 10));
    m_ui->bidsTable->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight);
    m_ui->bidsTable->horizontalHeaderItem(2)->setFont(QFont("Arial", 10));
    m_ui->bidsTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignRight);

    //  m_ui->bidsTable->verticalHeader()->setStyleSheet(styleSheet);

    m_ui->bidsChart->setRenderHint(QPainter::Antialiasing);
    m_bidsDepth = new QSplineSeries;
    m_bidsDepth->setUseOpenGL(true);
    m_bidsDepthFill = new QSplineSeries;
    m_bidsDepthFill->setUseOpenGL(true);
    auto bidsAreaSeries = new QAreaSeries(m_bidsDepth, m_bidsDepthFill);
    bidsAreaSeries->setPen(QPen(QColor(0, 255, 0), 2));
    bidsAreaSeries->setColor(QColor(0, 255, 0, 80));
    auto bidsChart = new QChart;
    bidsChart->legend()->hide();
    bidsChart->addSeries(bidsAreaSeries);
    bidsChart->createDefaultAxes();
    bidsChart->setBackgroundBrush(QBrush(QColor(20, 20, 20, 0)));
    bidsChart->setPlotAreaBackgroundBrush(QBrush(QColor(0, 255, 50, 0)));
    bidsChart->setPlotAreaBackgroundVisible(true);
    auto yAxisBids = qobject_cast<QValueAxis *>(bidsChart->axes(Qt::Vertical).first());
    auto xAxisBids = qobject_cast<QValueAxis *>(bidsChart->axes(Qt::Horizontal).first());
    xAxisBids->setTickType(QValueAxis::TickType::TicksDynamic);
    xAxisBids->setReverse(true);
    xAxisBids->setLinePen(QPen(QBrush(QColor(80, 80, 80)), 2));
    xAxisBids->setGridLineVisible(false);
    xAxisBids->setLabelsColor(QColor(200, 200, 200));
    yAxisBids->setTickType(QValueAxis::TickType::TicksDynamic);
    yAxisBids->setGridLineVisible(false);
    yAxisBids->setLinePen(QPen(QBrush(QColor(80, 80, 80)), 2));
    yAxisBids->setLabelsColor(QColor(200, 200, 200));
    bidsChart->setMargins(QMargins(0, 0, 0, 0));
    m_ui->bidsChart->setChart(bidsChart);

    m_ui->asksChart->setRenderHint(QPainter::Antialiasing);
    m_asksDepth = new QSplineSeries;
    m_asksDepth->setUseOpenGL(true);
    m_asksDepthFill = new QSplineSeries;
    m_asksDepthFill->setUseOpenGL(true);
    auto asksAreaSeries = new QAreaSeries(m_asksDepth, m_asksDepthFill);
    asksAreaSeries->setPen(QPen(QColor(255, 0, 0), 2));
    asksAreaSeries->setColor(QColor(255, 0, 0, 80));
    auto asksChart = new QChart;
    asksChart->legend()->hide();
    asksChart->addSeries(asksAreaSeries);
    asksChart->createDefaultAxes();
    asksChart->setBackgroundBrush(QBrush(QColor(20, 20, 20, 0)));
    asksChart->setPlotAreaBackgroundBrush(QBrush(QColor(255, 50, 50, 0)));
    asksChart->setPlotAreaBackgroundVisible(true);

    auto yAxisAsksDefault = qobject_cast<QValueAxis *>(asksChart->axes(Qt::Vertical).first());
    asksChart->removeAxis(yAxisAsksDefault);

    auto yAxisAsks = new QValueAxis(this);
    asksChart->addAxis(yAxisAsks, Qt::AlignRight);
    asksAreaSeries->attachAxis(yAxisAsks);

    auto xAxisAsks = qobject_cast<QValueAxis *>(asksChart->axes(Qt::Horizontal).first());
    xAxisAsks->setTickType(QValueAxis::TickType::TicksDynamic);
    xAxisAsks->setLinePen(QPen(QBrush(QColor(80, 80, 80)), 2));
    xAxisAsks->setLabelsColor(QColor(200, 200, 200));
    xAxisAsks->setGridLineVisible(false);
    xAxisAsks->setLabelsColor(QColor(200, 200, 200));
    yAxisAsks->setTickType(QValueAxis::TickType::TicksDynamic);
    yAxisAsks->setGridLineVisible(false);
    yAxisAsks->setLinePen(QPen(QBrush(QColor(80, 80, 80)), 2));
    yAxisAsks->setLabelsColor(QColor(200, 200, 200));

    asksChart->setMargins(QMargins(0, 0, 0, 0));

    m_ui->asksChart->setChart(asksChart);
}

void CENTAUR_NAMESPACE::CentaurApp::saveInterfaceState() noexcept
{
    logTrace("app", "CentaurApp::saveInterfaceState()");

    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("mainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();

    settings.beginGroup("watchlistListState");
    settings.setValue("geometry", m_ui->listWatchList->saveGeometry());
    settings.setValue("h-geometry", m_ui->listWatchList->horizontalHeader()->saveGeometry());
    settings.setValue("state", m_ui->listWatchList->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("loggingListState");
    settings.setValue("geometry", m_ui->logsTable->saveGeometry());
    settings.setValue("h-geometry", m_ui->logsTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", m_ui->logsTable->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("BalancesTreeState");
    settings.setValue("geometry", m_ui->ctrlBalances->saveGeometry());
    settings.setValue("h-geometry", m_ui->ctrlBalances->header()->saveGeometry());
    settings.setValue("state", m_ui->ctrlBalances->header()->saveState());
    settings.endGroup();

    settings.beginGroup("OrderbookAsksState");
    settings.setValue("geometry", m_ui->asksTable->saveGeometry());
    settings.setValue("h-geometry", m_ui->asksTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", m_ui->asksTable->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("OrderbookBidsState");
    settings.setValue("geometry", m_ui->bidsTable->saveGeometry());
    settings.setValue("h-geometry", m_ui->bidsTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", m_ui->bidsTable->horizontalHeader()->saveState());
    settings.endGroup();

    logInfo("app", "UI state saved");
}

void CENTAUR_NAMESPACE::CentaurApp::loadInterfaceState() noexcept
{
    logTrace("app", "CentaurApp::loadInterfaceState()");

    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("mainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("watchlistListState");
    m_ui->listWatchList->restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->listWatchList->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    m_ui->listWatchList->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("loggingListState");
    m_ui->logsTable->restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->logsTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    m_ui->logsTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("BalancesTreeState");
    m_ui->ctrlBalances->restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->ctrlBalances->header()->restoreGeometry(settings.value("h-geometry").toByteArray());
    m_ui->ctrlBalances->header()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("OrderbookAsksState");
    m_ui->asksTable->restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->asksTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    m_ui->asksTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("OrderbookBidsState");
    m_ui->bidsTable->restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->bidsTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    m_ui->bidsTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    logInfo("app", "UI state loaded");
}

void cen::CentaurApp::loadFavoritesWatchList() noexcept
{
    auto data = m_sqlFavorites->selectAll();

    if (data.isEmpty())
    {
        logInfo("app", LS("info-fav-db-empty"));
        return;
    }

    for (const auto &[sym, plid] : data)
    {
        if (sym.isEmpty())
        {
            logWarn("app", LS("warning-fav-symbol-empty"));
            continue;
        }

        if (plid.isEmpty())
        {
            logWarn("app", LS("warning-fav-uuid-empty"));
            continue;
        }

        // Insert the element
        onAddToWatchList(sym, plid, false);
    }
}

void CENTAUR_NAMESPACE::CentaurApp::startLoggingService() noexcept
{
    g_logger = new CentaurLogger;
    // Init the logger
    m_loggerThread = std::make_unique<std::thread>(&CENTAUR_NAMESPACE::CentaurLogger::run, g_logger);
    try
    {
        g_logger->setApplication(this);
        g_logger->setUser("root");
        g_logger->setSession(0);
        logInfo("app", QString("TraderSys ") + QString(CENTAUR_NAMESPACE::CentaurVersionString));
    } catch (const std::runtime_error &ex)
    {
        QMessageBox::critical(this,
            tr("Fatal error"),
            ex.what());
        QApplication::quit();
    }
}

void CENTAUR_NAMESPACE::CentaurApp::startCommunicationsServer() noexcept
{
    m_serverStatus = new QLabel("Server", m_ui->statusBar);
    m_ui->statusBar->addPermanentWidget(m_serverStatus, 0);
    m_server = std::make_unique<ProtocolServer>(this);
    if (m_server->isListening())
        m_serverStatus->setStyleSheet(g_statusGreen);
    else
        m_serverStatus->setStyleSheet(g_statusRed);
}

void CENTAUR_NAMESPACE::CentaurApp::onActionTileWindowsTriggered()
{
    m_ui->m_mdiArea->tileSubWindows();
}

void CENTAUR_NAMESPACE::CentaurApp::onActionCascadeWindowsTriggered()
{
    m_ui->m_mdiArea->cascadeSubWindows();
}

void CENTAUR_NAMESPACE::CentaurApp::onActionSymbolsToggled(bool status)
{
    status ? m_ui->dockSymbols->show() : m_ui->dockSymbols->hide();
}

void CENTAUR_NAMESPACE::CentaurApp::onActionLoggingToggled(bool status)
{
    status ? m_ui->dockLogging->show() : m_ui->dockLogging->hide();
}

void CENTAUR_NAMESPACE::CentaurApp::onActionBalancesToggled(bool status)
{
    status ? m_ui->dockBalances->show() : m_ui->dockBalances->hide();
}

void CENTAUR_NAMESPACE::CentaurApp::onActionAsksToggled(bool status)
{
    status ? m_ui->dockOrderbookAsks->show() : m_ui->dockOrderbookAsks->hide();
}

void CENTAUR_NAMESPACE::CentaurApp::onActionBidsToggled(bool status)
{
    status ? m_ui->dockOrderbookBids->show() : m_ui->dockOrderbookBids->hide();
}

void CENTAUR_NAMESPACE::CentaurApp::onActionDepthToggled(bool status)
{
    status ? m_ui->dockDepth->show() : m_ui->dockDepth->hide();
}

void CENTAUR_NAMESPACE::CentaurApp::onAddToWatchList(const QString &symbol, const QString &sender, bool addToDatabase) noexcept
{
    logTrace("watchlist", "CentaurApp::onAddToWatchList()");

    auto interface = m_exchangeList.find(sender);

    if (interface == m_exchangeList.end())
    {
        logError("watchlist", QString(tr("The sender '%1' is not registered.")).arg(sender));
        return;
    }

    auto watchInterface = interface->second.exchange;

    // Generate a unique-id
    const int id = ++m_sessionIds;

    if (watchInterface->addSymbol(symbol, id))
    {
        auto itemSymbol  = new QStandardItem(symbol);
        auto itemPrice   = new QStandardItem("$ 0.00");
        auto itemSource  = new QStandardItem(interface->second.listName);
        auto itemLatency = new QStandardItem("0");
        auto itemUUID    = new QStandardItem(interface->second.uuid.to_string().c_str());
        int curRow       = m_watchlistItemModel->rowCount();
        itemSymbol->setFont(g_globals->fonts.symbolsDock.tableFont);
        itemSymbol->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemPrice->setFont(g_globals->fonts.symbolsDock.tableFont);
        itemPrice->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemSource->setFont(g_globals->fonts.symbolsDock.tableFont);
        itemSource->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemLatency->setFont(g_globals->fonts.symbolsDock.tableFont);
        itemLatency->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_watchlistItemModel->insertRow(curRow, { itemSymbol, itemPrice, itemSource, itemLatency, itemUUID });
        m_watchlistItems[id] = { itemSymbol, 0. };

        // Add to the database
        if (addToDatabase)
            addFavoritesWatchListDB(symbol, sender);
    }
    else
    {
        logError("watchlist", QString("Symbol %1 was not added to the watchlist").arg(symbol));
    }
}
void CENTAUR_NAMESPACE::CentaurApp::onTickerUpdate(const QString &symbol, const int &id, const quint64 &receivedTime, const double &price) noexcept
{
    // Find the item in the Watchlist items
    auto itemIter         = m_watchlistItems.find(id);
    auto &itemInformation = itemIter->second;

    if (itemIter == m_watchlistItems.end())
    {
        logError("wlTickerUpdate", QString("Watchlist item for the symbol '%1' was not found").arg(symbol));
        return;
    }

    const double previousPrice = [&]() -> double {
        double p                      = itemInformation.previousPrice;
        itemInformation.previousPrice = price; // Update the price
        return p;
    }();

    auto item         = itemInformation.listItem;
    const int itemRow = item->row();

    // Get the latency item
    auto itemLatency = m_watchlistItemModel->item(itemRow, 3);
    // Get the price item
    auto itemPrice = m_watchlistItemModel->item(itemRow, 1);
    if (price > previousPrice)
    {
        item->setIcon(g_globals->icons.upArrow);
        itemPrice->setData(QBrush(g_globals->colors.symbolsDockColors.priceUp), Qt::ForegroundRole);
    }
    else if (price < previousPrice)
    {
        item->setIcon(g_globals->icons.downArrow);
        itemPrice->setData(QBrush(g_globals->colors.symbolsDockColors.priceDown), Qt::ForegroundRole);
    }
    else
    {
        item->setIcon(g_globals->icons.downArrow);
        itemPrice->setData(QBrush(g_globals->colors.symbolsDockColors.priceNeutral), Qt::ForegroundRole);
    }
    itemPrice->setText("$ " + QLocale(QLocale::English).toString(price, 'f', 5));

    // Calculate latency
    const auto ms      = static_cast<quint64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto latency = receivedTime > ms ? 0ull : ms - receivedTime;

    if (latency >= g_globals->params.symbolsDockParameters.latencyLowMin && latency <= g_globals->params.symbolsDockParameters.latencyLowMax)
    {
        itemLatency->setData(QBrush(g_globals->colors.symbolsDockColors.latencyLow), Qt::ForegroundRole);
    }
    else if (latency >= g_globals->params.symbolsDockParameters.latencyMediumMin && latency <= g_globals->params.symbolsDockParameters.latencyMediumMax)
    {
        itemLatency->setData(QBrush(g_globals->colors.symbolsDockColors.latencyMedium), Qt::ForegroundRole);
    }
    else if (latency >= g_globals->params.symbolsDockParameters.latencyHighMin && latency <= g_globals->params.symbolsDockParameters.latencyHighMax)
    {
        itemLatency->setData(QBrush(g_globals->colors.symbolsDockColors.latencyHigh), Qt::ForegroundRole);
    }
    itemLatency->setText(QString("%1 ms").arg(latency));
}

void CENTAUR_NAMESPACE::CentaurApp::onRemoveWatchList(const int &row) noexcept
{
    logTrace("watchlist", "CentaurApp::onRemoveWatchList");
    /// Retrieve the IExchange from the row based on the 4 column which has the PluginUUID Source
    auto index         = m_watchlistItemModel->item(row, 0);
    QString itemSymbol = index->text();
    QString itemSource = m_watchlistItemModel->item(row, 4)->text();

    auto interfaceIter = m_exchangeList.find(itemSource);
    if (interfaceIter == m_exchangeList.end())
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

    exchInfo.exchange->removeSymbol(itemSymbol);

    // Remove the row
    if (!m_watchlistItemModel->removeRow(row))
        logError("watchlist", QString(tr("%1 was not removed from the UI list")).arg(itemSymbol));
    else
        logInfo("watchlist", QString(tr("%1 was removed from the UI list")).arg(itemSymbol));

    // Find the item id
    int id { -1 };
    for (auto &m_watchlistItem : m_watchlistItems)
    {
        if (m_watchlistItem.second.listItem == index)
        {
            id = m_watchlistItem.first;
            logDebug("watchlist", QString("QStandardItem found with id %1").arg(id));
            break;
        }
    }
    // Remove the data associated
    if (id >= 0)
    {
        m_watchlistItems.erase(id);
        logInfo("watchlist", QString(tr("%1 from %2 removed from the watchlist")).arg(itemSymbol, itemSource));
        // Remove from the favorites DB
        removeFavoritesWatchListDB(itemSymbol, itemSource);

        if (m_watchlistItems.empty())
        {
            // Clear all the data
            clearOrderbookListsAndDepth();
        }
    }
    else
        logError("watchlist", "could not locate the item to remove");
}

void CENTAUR_NAMESPACE::CentaurApp::onSetWatchlistSelection(const QString &source, const QString &symbol) noexcept
{
    auto itemIter = m_exchangeList.find(source);
    if (itemIter == m_exchangeList.end())
    {
        logError("wlOrderbookSend", QString("Watchlist item for the symbol %1 was not found").arg(symbol));
        return;
    }

    const auto &[curSource, curSymbol] = m_currentViewOrderbookSymbol;

    if (curSource == source && curSymbol == symbol)
    {
        logInfo("wlOrderbookSend", "Symbol data is already being received");
        return;
    }

    auto &interface = itemIter->second.exchange;
    interface->updateOrderbook(symbol);
    m_currentViewOrderbookSymbol = { source, symbol };
    m_ui->bidsSymbol->setText(QString("%1 - %2").arg(symbol, itemIter->second.listName));
    m_ui->asksSymbol->setText(QString("%1 - %2").arg(symbol, itemIter->second.listName));
}

void CENTAUR_NAMESPACE::CentaurApp::onWatchlistRemoveSelection() noexcept
{
    auto &[source, symbol] = m_currentViewOrderbookSymbol;
    if (source.isEmpty() || symbol.isEmpty())
        return;

    auto itemIter = m_exchangeList.find(source);
    if (itemIter == m_exchangeList.end())
    {
        logError("wlOrderbookStop", QString("Watchlist item for the symbol %1 was not found").arg(symbol));
        return;
    }
    auto &interface = itemIter->second.exchange;
    interface->stopOrderbook(symbol);
    clearOrderbookListsAndDepth();
}

void CENTAUR_NAMESPACE::CentaurApp::onOrderbookUpdate(const QString &source, const QString &symbol, const quint64 &receivedTime, const QMap<qreal, QPair<qreal, qreal>> &bids, const QMap<qreal, QPair<qreal, qreal>> &asks) noexcept
{
    const auto &[curSource, curSymbol] = m_currentViewOrderbookSymbol;

    if (curSource != source || curSymbol != symbol)
        return;

    auto insertTable = [](const QString &text, QTableWidget *ui, int row, int col, int type) {
        QTableWidgetItem *item = ui->item(row, col);
        if (item == nullptr)
        {
            item = new QTableWidgetItem(text);

            item->setFont(QFont("Arial", 10));

            switch (col)
            {
                case 1:
                    item->setTextAlignment(Qt::AlignCenter);
                    break;
                case 2:
                    item->setTextAlignment(Qt::AlignRight);
                    break;
                case 0:
                default:
                    item->setTextAlignment(Qt::AlignLeft);
                    if (type == 1)
                        item->setForeground(QBrush(QColor(0xFFA7AC)));
                    else
                        item->setForeground(QBrush(QColor(0x9CFFB4)));
                    break;
            }
            item->setTextAlignment(Qt::AlignVCenter);
            ui->setItem(row, col, item);
        }

        // Quite an expensive function
        item->setText(text);
    };

    int nRowIndex = -1;

    m_ui->asksTable->setRowCount(static_cast<int>(asks.size()));
    nRowIndex = 0;
    for (auto iter = asks.begin(); iter != asks.end(); ++iter)
    {

        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.key(), 'f', 5)), m_ui->asksTable, nRowIndex, 0, 1);
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().first, 'f', 5)), m_ui->asksTable, nRowIndex, 1, 1);
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().second, 'f', 5)), m_ui->asksTable, nRowIndex, 2, 1);
        ++nRowIndex;
    }

    m_ui->bidsTable->setRowCount(static_cast<int>(bids.size()));
    nRowIndex = 0;
    for (auto iter = bids.begin(); iter != bids.end(); ++iter)
    {
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.key(), 'f', 5)), m_ui->bidsTable, nRowIndex, 0, 0);
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().first, 'f', 5)), m_ui->bidsTable, nRowIndex, 1, 0);
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().second, 'f', 5)), m_ui->bidsTable, nRowIndex, 2, 0);
        ++nRowIndex;
    }

    // Calculate latency
    const auto ms      = static_cast<quint64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto latency = receivedTime > ms ? 0ull : ms - receivedTime;

    auto changeColor   = [](QLabel *ctr, const QColor &color) {
        QPalette pal = ctr->palette();
        pal.setColor(ctr->foregroundRole(), color);
        ctr->setPalette(pal);
    };

    if (latency >= g_globals->params.orderBookParameters.asksSide.latencyLowMin <= g_globals->params.orderBookParameters.asksSide.latencyLowMax)
    {
        changeColor(m_ui->asksLatency, g_globals->colors.orderBookDockColors.asksSide.latencyLow);
    }
    else if (latency >= g_globals->params.orderBookParameters.asksSide.latencyMediumMin && latency <= g_globals->params.orderBookParameters.asksSide.latencyMediumMax)
    {
        changeColor(m_ui->asksLatency, g_globals->colors.orderBookDockColors.asksSide.latencyMedium);
    }
    else if (latency >= g_globals->params.orderBookParameters.asksSide.latencyHighMin && latency <= g_globals->params.orderBookParameters.asksSide.latencyHighMax)
    {
        changeColor(m_ui->asksLatency, g_globals->colors.orderBookDockColors.asksSide.latencyHigh);
    }

    if (latency >= g_globals->params.orderBookParameters.bidsSide.latencyLowMin <= g_globals->params.orderBookParameters.bidsSide.latencyLowMax)
    {
        changeColor(m_ui->bidsLatency, g_globals->colors.orderBookDockColors.bidsSide.latencyLow);
    }
    else if (latency >= g_globals->params.orderBookParameters.bidsSide.latencyMediumMin && latency <= g_globals->params.orderBookParameters.bidsSide.latencyMediumMax)
    {
        changeColor(m_ui->bidsLatency, g_globals->colors.orderBookDockColors.bidsSide.latencyMedium);
    }
    else if (latency >= g_globals->params.orderBookParameters.bidsSide.latencyHighMin && latency <= g_globals->params.orderBookParameters.bidsSide.latencyHighMax)
    {
        changeColor(m_ui->bidsLatency, g_globals->colors.orderBookDockColors.bidsSide.latencyHigh);
    }

    m_ui->asksLatency->setText(QString("%1 ms").arg(latency));
    m_ui->bidsLatency->setText(QString("%1 ms").arg(latency));

    if (m_ui->dockDepth->isVisible())
    {
        plotDepth(asks, bids);
    }
}

void CENTAUR_NAMESPACE::CentaurApp::plotDepth(const QMap<qreal, QPair<qreal, qreal>> &asks, const QMap<qreal, QPair<qreal, qreal>> &bids) noexcept
{
    m_ui->asksChart->setUpdatesEnabled(false);
    m_ui->bidsChart->setUpdatesEnabled(false);

    auto generatePoints = [](const QMap<qreal, QPair<qreal, qreal>> &data, QList<QPointF> &prices, QList<QPointF> &fill) {
        double prevQuant = 0.0;
        for (auto iter = data.begin(); iter != data.end(); ++iter)
        {
            // const auto &obprices = iter.key();
            // auto quantity        = iter.value().first;
            /*
            if (!quantity.isEmpty() && !quantity[0].isDigit())
            {
                quantity = quantity.remove(-0, 1).mid(0);
                quantity.replace(",", "");
            }*/

            //  const auto &price = obprices.toDouble();

            const auto quant = iter.value().first + prevQuant;

            prices.append({ iter.key(), quant });
            fill.append({ iter.key(), 0.0 });

            // increments.push_back(quant);
            // prices.push_back(price);

            prevQuant = quant;
        }
    };

    using minMaxType    = std::pair<std::tuple<double, double, double>, std::tuple<double, double, double>>;
    auto generateMinMax = [](const QList<QPointF> &points) -> minMaxType {
        const double minY   = points.first().y(),
                     maxY   = points.last().y() * 1.1;
        const double stepsY = (maxY - minY) / 10.0;

        const double minX   = points.first().x(),
                     maxX   = points.last().x();
        const double stepsX = (maxX - minX) / 5.0;

        return {
            { minX, maxX, stepsX },
            { minY, maxY, stepsY }
        };
    };

    QList<QPointF> bidsPoints, bidsFill, asksPoints, asksFill;

    generatePoints(asks, asksPoints, asksFill);
    generatePoints(bids, bidsPoints, bidsFill);

    if (asksPoints.isEmpty() || bidsPoints.isEmpty())
    {
        // No need to continue
        return;
    }

    auto [rangeAsksX, rangeAsksY]          = generateMinMax(asksPoints);
    auto [rangeBidsX, rangeBidsY]          = generateMinMax(bidsPoints);

    auto &[asksMinX, asksMaxX, asksStepsX] = rangeAsksX;
    auto &[asksMinY, asksMaxY, asksStepsY] = rangeAsksY;

    auto &[bidsMinX, bidsMaxX, bidsStepsX] = rangeBidsX;
    auto &[bidsMinY, bidsMaxY, bidsStepsY] = rangeBidsY;

    auto yAxisBids                         = dynamic_cast<QValueAxis *>(m_ui->bidsChart->chart()->axes(Qt::Vertical).first());
    auto xAxisBids                         = dynamic_cast<QValueAxis *>(m_ui->bidsChart->chart()->axes(Qt::Horizontal).first());

    yAxisBids->setRange(bidsMinY, bidsMaxY);
    yAxisBids->setTickInterval(bidsStepsY);
    xAxisBids->setRange(bidsMinX, bidsMaxX);
    xAxisBids->setTickInterval(bidsStepsX);

    m_bidsDepthFill->clear();
    m_bidsDepth->clear();

    m_bidsDepthFill->append(bidsFill);
    m_bidsDepth->append(bidsPoints);

    auto yAxisAsks = dynamic_cast<QValueAxis *>(m_ui->asksChart->chart()->axes(Qt::Vertical).first());
    auto xAxisAsks = dynamic_cast<QValueAxis *>(m_ui->asksChart->chart()->axes(Qt::Horizontal).first());

    yAxisAsks->setRange(asksMinY, asksMaxY);
    yAxisAsks->setTickInterval(asksStepsY);
    xAxisAsks->setRange(asksMinX, asksMaxX);
    xAxisAsks->setTickInterval(asksStepsX);

    m_asksDepthFill->clear();
    m_asksDepth->clear();

    m_asksDepthFill->append(asksFill);
    m_asksDepth->append(asksPoints);

    m_ui->asksChart->setUpdatesEnabled(true);
    m_ui->bidsChart->setUpdatesEnabled(true);
}

void CENTAUR_NAMESPACE::CentaurApp::clearOrderbookListsAndDepth() noexcept
{
    m_currentViewOrderbookSymbol = { "", "" };
    m_ui->bidsSymbol->setText("");
    m_ui->asksSymbol->setText("");
    m_ui->bidsLatency->setText("");
    m_ui->asksLatency->setText("");
    m_ui->asksTable->setRowCount(0);
    m_ui->bidsTable->setRowCount(0);

    m_bidsDepthFill->clear();
    m_bidsDepth->clear();

    m_asksDepthFill->clear();
    m_asksDepth->clear();
}

void CENTAUR_NAMESPACE::CentaurApp::onPlugins() noexcept
{
    CENTAUR_NAMESPACE::PluginsDialog dlg(this);
    dlg.exec();
}

void CENTAUR_NAMESPACE::CentaurApp::addFavoritesWatchListDB(const QString &symbol, const QString &sender) noexcept
{
    m_sqlFavorites->add(symbol, sender);
}

void CENTAUR_NAMESPACE::CentaurApp::removeFavoritesWatchListDB(const QString &symbol, const QString &sender) noexcept
{
    m_sqlFavorites->del(symbol, sender);
}

CENTAUR_PLUGIN_NAMESPACE::IExchange *CENTAUR_NAMESPACE::CentaurApp::exchangeFromWatchlistRow(const int &row) noexcept
{
    const QString itemSource = m_watchlistItemModel->item(row, 4)->text();
    return exchangeFromWatchlistRow(itemSource);
}

CENTAUR_PLUGIN_NAMESPACE::IExchange *CENTAUR_NAMESPACE::CentaurApp::exchangeFromWatchlistRow(const QString &sender) noexcept
{
    auto interface = m_exchangeList.find(sender);

    if (interface == m_exchangeList.end())
    {
        logError("watchlist", QString(tr("The sender '%1' is not registered.")).arg(sender));
        return nullptr;
    }

    return interface->second.exchange;
}
