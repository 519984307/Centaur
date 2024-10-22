/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// General aspects of plugins

#include "../../Library/cui/include/OptionsTableWidget.hpp"
#include "../ui/ui_CentaurApp.h"
#include "CentaurApp.hpp"
#include "DAL.hpp"
#include "SplashDialog.hpp"

#include <QCryptographicHash>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QPluginLoader>

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

    logInfo("plugins", tr("Plugins local data loaded and parsed."));

    QString pluginPath = g_globals->paths.pluginsPath;
    QDir pluginsDir(pluginPath);

    auto range = splash->getProgressRange();
    splash->setProgressRange(0, range.second + 2 * static_cast<int>(pluginsDir.entryList(QDir::Files).size()));

#ifdef NO_PLUGIN_CHECKSUM_CHECK
    logWarn("loadPlugins", "No checksum verification for plugins");
#endif /**/

    for (const auto &plFile : pluginsDir.entryList(QDir::Files))
    {
        QString realFile = pluginsDir.absoluteFilePath(plFile);

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

        splash->setDisplayText(QString(tr("Loading: %1")).arg(plFile));

#ifndef NO_PLUGIN_CHECKSUM_CHECK
        const QString checksum = [&]() {
            QFile ckFile(realFile);
            if (!ckFile.open(QIODevice::ReadOnly))
            {
                logError("loadPlugins", "The file could not be opened for checksum check");
                return QString("");
            }
            const auto data   = ckFile.readAll();
            const QString ret = QCryptographicHash::hash(data, QCryptographicHash::Sha224).toHex();
            ckFile.close();
            return ret;
        }();
        if (checksum.isEmpty())
            continue;
#endif /*NO_PLUGIN_CHECKSUM_CHECK*/

        auto loader     = new QPluginLoader(realFile);
        QObject *plugin = loader->instance();
        splash->step();

        if (plugin)
        {
            // Add to the list
            auto baseInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IBase *>(plugin);

            if (baseInterface == nullptr)
                logError("loadPlugins", tr("The file is not a plugin"));
            else
            {
                splash->setDisplayText(tr("Initializing: %1 (%2)").arg(baseInterface->getPluginName(), baseInterface->getPluginVersionString()));

                // Check data
                const auto plDBInfo = dal::DataAccess::pluginInformation(baseInterface->getPluginUUID().to_qstring(false));

                if (!plDBInfo.has_value())
                {
                    logError("loadPlugins", tr("Plugin %1 found in the filesystem but not in the installed database").arg(plFile));
                    continue;
                }

                // Check the dynamic file name
                if ((*plDBInfo).dynamic != plFile)
                {
                    logError("loadPlugins", tr("Plugin %1 filename and DB file discrepancies").arg(plFile));
                    continue;
                }

                // Check UI Version
                if ((*plDBInfo).centaur_uuid != CENTAUR_PLUGIN_NAMESPACE::centaurUUID)
                {
                    logError("loadPlugins", tr("Plugin %1 not supported").arg(plFile));
                    continue;
                }

                if (!(*plDBInfo).enabled)
                {
                    logInfo("loadPlugins", tr("Plugin %1 is disabled").arg(plFile));
                    continue;
                }

                if ((*plDBInfo).name != baseInterface->getPluginName())
                {
                    logError("loadPlugins", tr("Plugin %1 name and DB name discrepancies").arg(plFile));
                    continue;
                }

                if ((*plDBInfo).version != baseInterface->getPluginVersionString())
                {
                    logError("loadPlugins", tr("Plugin %1 version string and DB version string discrepancies").arg(plFile));
                    continue;
                }

#ifndef NO_PLUGIN_CHECKSUM_CHECK
                if ((*plDBInfo).checksum != checksum)
                {
                    logError("loadPlugins", tr("Plugin %1 invalid checksum").arg(plFile));
                    continue;
                }
#endif /*NO_PLUGIN_CHECKSUM_CHECK*/

                mapPluginBase(baseInterface);

                logInfo("loadPlugins", tr("Plugin found in file: ##F2FEFF#%1#").arg(plFile));

                // Init the plugin
                auto pluginConfig = new PluginConfiguration(baseInterface->getPluginUUID().to_string().c_str());

                baseInterface->setPluginInterfaces(g_logger,
                    static_cast<CENTAUR_INTERFACE_NAMESPACE::IConfiguration *>(pluginConfig));

                // Generate the plugin data
                mapConfigurationInterface(baseInterface->getPluginUUID(), pluginConfig);

                if (auto exInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IExchange *>(plugin); exInterface)
                {
                    logInfo("loadPlugins", tr("IExchange plugin found in file: ##F2FEFF#%1#").arg(plFile));
                    if (!initExchangePlugin(exInterface))
                    {
                        loader->unload();
                        logWarn("loadPlugins", tr("Plugin IExchange in file: ##F2FEFF#%1#, was unloaded").arg(plFile));
                        removeLastPluginBase();
                        continue;
                    }
                }

                if (auto stInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IStatus *>(plugin); stInterface)
                {
                    logInfo("loadPlugins", tr("IStatus plugin found in file: ##F2FEFF#%1#").arg(plFile));
                    // Init the plugin
                    initStatusPlugin(stInterface);
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

bool CENTAUR_NAMESPACE::CentaurApp::initExchangePlugin(CENTAUR_NAMESPACE::plugin::IExchange *exchange) noexcept
{
    logTrace("plugins", "CentaurApp::initExchangePlugin");

    const auto uuid = exchange->getPluginUUID();

    if (!exchange->initialization())
    {
        logError("plugins", tr("Failed to initialize an IExchange-plugin"));
        return false;
    }

    auto list = populateExchangeSymbolList(exchange);

    mapExchangePlugin(uuid, ExchangeInformation { uuid, exchange, list, exchange->getSymbolListName().first });

    // clang-format off
    connect(exchange->getPluginObject(), SIGNAL(snTickerUpdate(QString,QString,quint64,double)), this, SLOT(onTickerUpdate(QString,QString,quint64,double)));
    // clang-format on

    mapExchangePluginViewMenus(uuid, exchange->dynamicWatchListMenuItems());

    /*
        auto data = exchange->getCandlesByPeriod("BTCUSDT", 1659225600000, 1665792000000, cen::plugin::TimeFrame::Hours_1);

        std::cout << "QList<QPair<CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp, CENTAUR_PLUGIN_NAMESPACE::CandleData>> data = {";
        for (const auto &[ts, cd] : data)
        {
            std::cout << "{ " << ts << ", {" << cd.high << ", " << cd.open << ", " << cd.close << ", " << cd.low << ", " << cd.volume << "} },";
        }
    */
    return true;
}

CENTAUR_NAMESPACE::OptionsTableWidget *CENTAUR_NAMESPACE::CentaurApp::populateExchangeSymbolList(CENTAUR_NAMESPACE::plugin::IExchange *exchange) noexcept
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

    connect(button, &QPushButton::clicked, this, [&, widget, symbolsList, exchange](bool clicked) {
        if (clicked)
        {
            if (symbolsList->getRowCount() == 0)
            {

                auto symbols = exchange->getSymbolList();
                for (const auto &[sym, icon] : symbols)
                {

                    auto item  = new QStandardItem(sym);
                    int curRow = symbolsList->getRowCount();
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

                    item->setIcon(*icon);

                    symbolsList->insertRowWithOptions(curRow, { item }, false);
                }
            }

            ui()->stackedWidget->setCurrentWidget(widget);
        }
    });

    connect(symbolsList, &OptionsTableWidget::customContextMenuRequested, this, [&, symbolsList, exchange](const QPoint &pos) {
        // Show the context menu
        QModelIndex index = symbolsList->indexAt(pos).siblingAtColumn(0);
        auto itemData     = index.data(Qt::DisplayRole).toString();

        if (!itemData.isEmpty())
        {
            QMenu contextMenu("Context menu", this);

            QAction action(tr("Add '%1' to the watchlist").arg(itemData), this);
            contextMenu.addAction(&action);

            connect(&action, &QAction::triggered, this,
                [&]() { onAddToWatchList(itemData, exchange->getPluginUUID().to_qstring(true), true); });

            contextMenu.exec(symbolsList->mapToGlobal(pos));
        }
    });

    logInfo("plugins", "Exchange list populated");

    return symbolsList;
}

void cen::CentaurApp::initStatusPlugin(CENTAUR_PLUGIN_NAMESPACE::IStatus *status) noexcept
{
    using namespace CENTAUR_PLUGIN_NAMESPACE;
    auto mode = status->initialize();

    constexpr static char stylesheet[] = { R"(QToolButton{border-radius: 0px; border: 0px; }
    QToolButton:hover{background-color: qlineargradient(x1:0.5, y1: 0, x2:0.5, y2:1, stop: 0  rgb(58,58,58), stop: 1 rgb(68,68,68)); border-radius: 0px;}
    QToolButton:pressed{background-color: qlineargradient(x1:0.5, y1: 0, x2:0.5, y2:1, stop: 1  rgb(85,85,85), stop: 0 rgb(95,95,95)); border-radius: 0px;})" };

    auto *button = new QToolButton(ui()->frameStatusPluginsFrame);

    ui()->frameStatusPluginsFrameLayout->addWidget(button);

    button->setStyleSheet(stylesheet);
    button->setCheckable(false);

    auto textMode = [&]() {
        button->setMinimumSize(QSize(0, 20));
        button->setMaximumSize(QSize(16777215, 20));
        button->setText(status->text());
        button->setFont(status->font());
    };
    auto imageMode = [&]() {
        button->setMinimumSize(QSize(20, 20));
        button->setMaximumSize(QSize(20, 20));
        button->setIconSize(QSize(18, 18));
        button->setIcon(QIcon(status->image().scaled(18, 18, Qt::AspectRatioMode::KeepAspectRatio)));
    };

    switch (mode)
    {
        case IStatus::DisplayMode::OnlyText:
            button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            textMode();
            break;

        case IStatus::DisplayMode::OnlyIcon:
            button->setToolButtonStyle(Qt::ToolButtonIconOnly);
            imageMode();
            break;
        case IStatus::DisplayMode::TextIcon:
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            imageMode();
            textMode();
            break;
    }

    const auto foregroundBrush = status->brush(IStatus::DisplayRole::Foreground);
    const auto backgroundBrush = status->brush(IStatus::DisplayRole::Background);

    QPalette palette = button->palette();
    if (foregroundBrush != Qt::NoBrush)
    {
        palette.setBrush(QPalette::ColorRole::ButtonText, status->brush(IStatus::DisplayRole::Foreground));
    }
    if (backgroundBrush != Qt::NoBrush)
    {
        button->setStyleSheet("");
        button->setAutoFillBackground(true);
        palette.setBrush(QPalette::ColorRole::Button, status->brush(IStatus::DisplayRole::Background));
    }
    else
    {
        button->setStyleSheet(stylesheet);
        button->setAutoFillBackground(false);
    }
    button->setPalette(palette);

    // clang-format off
        connect(status->getPluginObject(), SIGNAL(displayChange(plugin::IStatus::DisplayRole)), this, SLOT(onStatusDisplayChanged(plugin::IStatus::DisplayRole)));
    // clang-format on

    if (status->action())
    {
        connect(button, &QPushButton::released, this, [status, button]() {
            // Update the button top left coordinates
            auto point = button->mapToGlobal(button->geometry().topLeft());
            status->action()->setData(point);

            emit status->action()->triggered(true);
        });
    }

    mapStatusPlugins(status->getPluginUUID(), status, button, mode);
}

/*
#include <QMetaMethod>
bool cen::CentaurApp::initCandleViewPlugin(cen::plugin::ICandleView *candleView) noexcept
{
    auto obj = candleView->getPluginObject();

    if (candleView->realtimePlotAllowed())
    {
        auto meta = obj->metaObject();

        if (meta->indexOfSignal(QMetaObject::normalizedSignature("snRealTimeCandleUpdate(const cen::uuid &, quint64, cen::plugin::ICandleView::Timestamp, const cen::plugin::ICandleView::CandleData &)")) == -1)
        {
            logError("plugins", QString(tr("The plugin with name ##F2FEFF#%1# does not have the update candle signature. Plugin will be unloaded")).arg(candleView->getPluginName()));
            return false;
        }

        // clang-format off
        connect(obj,
            SIGNAL(snRealTimeCandleUpdate(cen::uuid,quint64,cen::plugin::ICandleView::Timestamp,cen::plugin::ICandleView::CandleData)),
            this,
            SLOT(onRealTimeCandleUpdate(cen::uuid,quint64,cen::plugin::ICandleView::Timestamp,cen::plugin::ICandleView::CandleData)));
        // clang-format on
    }

    mapCandleViewSupport(candleView, { candleView->supportedExchanges(), candleView->supportedTimeFrames() });

    return true;
}
*/

CENTAUR_PLUGIN_NAMESPACE::PluginInformation cen::CentaurApp::pluginInformationFromBase(cen::plugin::IBase *base)
{
    return {
        base->getPluginUUID(),
        base->getPluginName(),
        base->getPluginVersionString()
    };
}
