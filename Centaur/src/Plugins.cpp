/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// General aspects of plugins

#include "../ui/ui_CentaurApp.h"
#include "CentaurApp.hpp"
#include "SplashDialog.hpp"
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QPluginLoader>
#include <QString>
#include <QUrl>

namespace
{
    constexpr char exchangeListButton[]     = { R"(QPushButton{
font-size: 12px; font-weight: 600;
color:  #cccccc; border: 0px;
border-top-left-radius: 0px;
border-top-right-radius: 0px;
border-bottom-left-radius: 10px;
border-bottom-right-radius: 10px;
background-color: rgba(28,35,43, 0);
text-align: center; padding:  0 2 0 2; }
QPushButton:default{font-weight: 800;
color:  #0d3C61; border: 0px;
border-radius:5px;
border: 2px solid rgba(144,203,249,100);
background-color: #2196f3;}
QPushButton:hover, QPushButton:hover:default{background-color:  rgb(55,61,68);}
QPushButton:pressed{background-color: rgb(62,68,75);}
QPushButton::on{background-color: rgb(51,57,64);}
QPushButton:pressed:default{background-color:  rgb(51,57,64);}
QPushButton:disabled{color: #c0c0c0;background-color: #9D9EA4;})" };
    constexpr char exchangeSearchLineEdit[] = { R"(QLineEdit{border: 0px;
color: black; background-color: rgb(220,220,220);
border-radius:  0px; min-height: 25px;padding: 0 0 0 5; }
QLineEdit:hover{ background-color: rgb(232,232,232); border: 2px solid #339ef4 }
QLineEdit:focus{ background-color:  rgb(240,240,240); border: 2px solid #56aff6;})" };
    constexpr char exchangeTable[]          = { R"(QTableView { color: white; background-color: rgba(46,53,59, 0); border: 0px; }
QTableView QTableCornerButton::section { background: #5299D4; border: 0px;}
QTableView::item { border: 0px; height: 20px; }
QTableView::item:hover{ background-color: rgb(79,85,90); }
QTableView::item:hover:selected{ background-color: rgb(104,109,113); font-weight: 800; }
QTableView::item:selected{ color: white; font-weight: 600; background-color: rgb(91,97,102);}
QHeaderView::section:checked { background-color:  #3387CD; color: white; }
QHeaderView::section { font: normal 800 12px Roboto; padding-left: 4px; border: 0px; height: 20px; background-color: #0069c0;
color: white; padding-left: 4px; border: 0px; }
QHeaderView::section:hover { background-color: #5299D4; color: white; })" };
} // namespace

void CENTAUR_NAMESPACE::CentaurApp::loadPlugins(SplashDialog *splash) noexcept
{
    logTrace("plugins", "CentaurApp::loadPlugins()");

    m_longOperation = std::make_unique<LongOperation>();

    logInfo("plugins", tr("Plugins local data loaded and parsed."));

    QString pluginPath = g_globals->paths.pluginsPath;
    QDir pluginsDir(pluginPath);

    auto range = splash->getProgressRange();
    splash->setProgressRange(0, range.second + static_cast<int>(pluginsDir.entryList(QDir::Files).size()));

    for (const auto &plFile : pluginsDir.entryList(QDir::Files))
    {
        QString realFile = pluginsDir.absoluteFilePath(plFile);
        // splash->setDisplayText(QString(tr("Looking for... %1")).arg(plFile));
        //  Detect if the file is a symbolic link
        QFileInfo info(realFile);
        if (info.isSymLink() || info.isSymbolicLink()
#ifdef Q_OS_WIN
            || info.isShortcut()
#endif /* Q_OS_WIN */
        )
        {
            realFile = info.symLinkTarget();
        }

        auto loader     = new QPluginLoader(realFile);
        QObject *plugin = loader->instance();

        if (plugin)
        {
            // Add to the list
            auto baseInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IBase *>(plugin);

            if (baseInterface == nullptr)
                logError("plugin", tr("The file is not a plugin"));
            else
            {
                splash->setDisplayText(QString(tr("Initializing: %1 (%2)")).arg(baseInterface->getPluginName(), baseInterface->getPluginVersionString()));

                m_pluginsData.push_back(baseInterface);

                logInfo("plugin", QString(tr("Plugin found in file: ##F2FEFF#%1#")).arg(plFile));

                // Init the plugin
                auto pluginConfig = new PluginConfiguration(baseInterface->getPluginUUID().to_string().c_str());

                // tr            loadPluginLocalData(baseInterface->getPluginUUID(), doc, pluginConfig);

                baseInterface->setPluginInterfaces(g_logger,
                    static_cast<CENTAUR_INTERFACE_NAMESPACE::IConfiguration *>(pluginConfig),
                    static_cast<CENTAUR_INTERFACE_NAMESPACE::ILongOperation *>(m_longOperation.get()));

                // Generate the plugin data
                mapConfigurationInterface(baseInterface->getPluginUUID(), pluginConfig);

                if (auto exInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IExchange *>(plugin); exInterface)
                {
                    logInfo("plugin", QString(tr("IExchange plugin found in file: ##F2FEFF#%1#")).arg(plFile));
                    if (!initExchangePlugin(exInterface))
                    {
                        loader->unload();
                        logWarn("plugin", QString(tr("Plugin IExchange in file: ##F2FEFF#%1#, was unloaded")).arg(plFile));
                    }
                }
                else if (auto stInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IStatus *>(plugin); stInterface)
                {
                    logInfo("plugin", QString(LS("info-plugin-istatus")).arg(plFile));
                    // Init the plugin
                    // stInterface->initialization(ui()->statusBar);
                }
                else if (auto cvInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::ICandleView *>(plugin); cvInterface)
                {
                    logInfo("plugin", QString(LS("info-plugin-icandleview")).arg(plFile));
                    if (!initCandleViewPlugin(cvInterface))
                    {
                        loader->unload();
                    }
                }

                if (loader->isLoaded())
                {
                    mapPluginInstance(loader);
                }
            }
        }
        else
            loader->unload();

        splash->step();
    }
}

void CENTAUR_NAMESPACE::CentaurApp::loadPluginLocalData(const CENTAUR_NAMESPACE::uuid &uuid, xercesc::DOMDocument *doc, PluginConfiguration *config) noexcept
{
}

bool CENTAUR_NAMESPACE::CentaurApp::initExchangePlugin(CENTAUR_NAMESPACE::plugin::IExchange *exchange) noexcept
{
    logTrace("plugins", "CentaurApp::initExchangePlugin");

    const auto uuid = exchange->getPluginUUID();

    if (!exchange->initialization())
    {
        logError("plugins", tr("Failed to initialize an IExchange-plugin"));
        return false;
    }

    auto list = populateExchangeSymbolList(exchange, uuid.to_string().c_str());

    mapExchangePlugin(uuid, ExchangeInformation { uuid, exchange, list, exchange->getSymbolListName().first });

    // clang-format off
    connect(exchange->getPluginObject(), SIGNAL(snTickerUpdate(QString,int,quint64,double)), this, SLOT(onTickerUpdate(QString,int,quint64,double)));
    connect(exchange->getPluginObject(), SIGNAL(snOrderbookUpdate(QString,QString,quint64,QMap<qreal,QPair<qreal,qreal> >,QMap<qreal,QPair<qreal,qreal> >)), this, SLOT(onOrderbookUpdate(QString, QString, quint64, QMap<qreal,QPair<qreal,qreal> >,QMap<qreal,QPair<qreal,qreal> >)));
    // clang-format on

    mapExchangePluginViewMenus(uuid, exchange->dynamicWatchListMenuItems());

    return true;
}

CENTAUR_NAMESPACE::OptionsTableWidget *CENTAUR_NAMESPACE::CentaurApp::populateExchangeSymbolList(CENTAUR_NAMESPACE::plugin::IExchange *exchange, const QString &uuidString) noexcept
{
    logTrace("plugins", "CentaurApp::populateExchangeSymbolList");

    const auto [name, icon] = exchange->getSymbolListName();

    QPushButton *button;
    // Add the button
    if (icon != nullptr)
        button = new QPushButton(*icon, name, ui()->scrollAreaWidgetContents);
    else
        button = new QPushButton(name, ui()->scrollAreaWidgetContents);
    button->setMinimumSize(QSize(0, 30));
    button->setCheckable(true);
    button->setAutoExclusive(true);
    button->setStyleSheet(exchangeListButton);
    button->setIconSize(QSize { 16, 16 });
    ui()->exchangesButtonsLayout->addWidget(button);

    auto widget         = new QWidget();
    auto verticalLayout = new QVBoxLayout(widget);
    verticalLayout->setSpacing(2);
    verticalLayout->setContentsMargins(2, 2, 2, 2);

    auto editCtrl = new SearchLineEdit(widget);
    editCtrl->setPlaceholderText(tr("Search..."));
    editCtrl->setClearButtonEnabled(true);
    editCtrl->setStyleSheet(exchangeSearchLineEdit);
    verticalLayout->addWidget(editCtrl);

    auto symbolsList = new OptionsTableWidget(widget);
    symbolsList->setStyleSheet(exchangeTable);
    symbolsList->initialize(editCtrl, 1, -1, -1, nullptr);

    symbolsList->sortByColumn(0, Qt::AscendingOrder);
    symbolsList->getModel()->setHorizontalHeaderLabels({ tr("Symbols") });
    symbolsList->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    symbolsList->horizontalHeader()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);

    symbolsList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    symbolsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    symbolsList->setGridStyle(Qt::NoPen);
    symbolsList->setSortingEnabled(true);
    symbolsList->sortByColumn(0, Qt::AscendingOrder);
    symbolsList->verticalHeader()->setVisible(false);

    verticalLayout->addWidget(symbolsList);

    ui()->stackedWidget->addWidget(widget);

    connect(button, &QPushButton::clicked, this, [&, widget = widget](bool clicked) {
        if (clicked)
            ui()->stackedWidget->setCurrentWidget(widget);
    });

    connect(symbolsList, &OptionsTableWidget::customContextMenuRequested, this, [&, symbolsList, exchange](const QPoint &pos) {
        // Show the context menu
        QModelIndex index = symbolsList->indexAt(pos).siblingAtColumn(0);
        auto itemData     = index.data(Qt::DisplayRole).toString();

        if (!itemData.isEmpty())
        {
            QMenu contextMenu("Context menu", this);

            QAction action(QString(tr("Add '%1' to the watchlist")).arg(itemData), this);
            contextMenu.addAction(&action);

            connect(&action, &QAction::triggered, this,
                [&]() { onAddToWatchList(itemData, exchange->getPluginUUID().to_qstring(true), true); });

            contextMenu.exec(symbolsList->mapToGlobal(pos));
        }
    });

    auto symbols = exchange->getSymbolList();

    for (const auto &[sym, icon] : symbols)
    {

        auto item  = new QStandardItem(sym);
        int curRow = symbolsList->getRowCount();
        item->setFont(g_globals->fonts.symbolsDock.tableFont);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        const QString base = exchange->getBaseFromSymbol(sym);
        QPixmap img;
        if (g_globals->symIcons.find(16, base, &img, 0))
            item->setIcon({ img });
        else
            item->setIcon(*icon);

        symbolsList->insertRowWithOptions(curRow, { item }, false);
    }

    logInfo("plugins", "Exchange list populated");

    return symbolsList;
}

#include <QMetaMethod>
bool cen::CentaurApp::initCandleViewPlugin(cen::plugin::ICandleView *candleView) noexcept
{
    auto obj = candleView->getPluginObject();

    if (candleView->realtimePlotAllowed())
    {
        auto meta = obj->metaObject();

        if (meta->indexOfSignal(QMetaObject::normalizedSignature("snRealTimeCandleUpdate(const cen::uuid &, quint64, cen::plugin::ICandleView::Timestamp, const cen::plugin::ICandleView::CandleData &)")) == -1)
        {
            logError("plugins", QString(LS("error-candle-plugin-signature-update")).arg(candleView->getPluginName()));
            return false;
        }

        // clang-format off
        connect(obj,
            SIGNAL(snRealTimeCandleUpdate(cen::uuid,quint64,cen::plugin::ICandleView::Timestamp,cen::plugin::ICandleView::CandleData)),
            this,
            SLOT(onRealTimeCandleUpdate(cen::uuid,quint64,cen::plugin::ICandleView::Timestamp,cen::plugin::ICandleView::CandleData)));
        // clang-format on
    }

    // Get supported time frames
    m_candleViewSupport[candleView] = { candleView->supportedExchanges(), candleView->supportedTimeFrames() };

    return true;
}

CENTAUR_PLUGIN_NAMESPACE::PluginInformation cen::CentaurApp::pluginInformationFromBase(cen::plugin::IBase *base)
{
    return {
        base->getPluginUUID(),
        base->getPluginName(),
        base->getPluginVersionString()
    };
}

CENTAUR_PLUGIN_NAMESPACE::ICandleView *cen::CentaurApp::getSupportedCandleViewPlugins(const CENTAUR_PLUGIN_NAMESPACE::PluginInformation &id)
{
    for (auto &cvs : m_candleViewSupport)
    {
        for (auto &spexch : cvs.second.info)
        {
            if (spexch == id)
                return cvs.first;
        }
    }

    return nullptr;
}

std::optional<std::reference_wrapper<const QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>> cen::CentaurApp::getCandleViewTimeframeSupport(CENTAUR_PLUGIN_NAMESPACE::ICandleView *id) const
{
    auto it = m_candleViewSupport.find(id);
    if (it == m_candleViewSupport.end())
        return std::nullopt;

    return std::optional<std::reference_wrapper<const QList<CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame>>> { std::cref(it->second.timeframes) };
}
