/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// General aspects of plugins

#include "CentaurApp.hpp"
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QPluginLoader>
#include <QUrl>

// clang-format off
CENTAUR_WARN_PUSH
CENTAUR_WARN_OFF(padded)
CENTAUR_WARN_OFF(undefined-func-template)
CENTAUR_WARN_OFF(signed-enum-bitfield)
CENTAUR_WARN_OFF(documentation-unknown-command)
CENTAUR_WARN_OFF(global-constructors)
CENTAUR_WARN_OFF(missing-noreturn)
CENTAUR_WARN_OFF(shadow-field-in-constructor)
CENTAUR_WARN_OFF(reserved-id-macro)
#include <fmt/core.h>
CENTAUR_WARN_POP
// clang-format on

void cent::CentaurApp::loadPlugins() noexcept
{
    logTrace("plugins", "CentaurApp::loadPlugins()");

    auto errorDelay = [](const int &ms) {
        QTime dieTime = QTime::currentTime().addMSecs(ms);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    };

    QDomDocument doc("plugins");
    QFile file(cent::g_globals->pluginsPath + "/local/plugins.xml");
    if (!file.open(QIODevice::ReadOnly))
    {
        logFatal("plugins", "Failed to locate plugins.xml");
        QMessageBox::critical(this,
            tr("Plugins Error"),
            tr("Failed to locate the local plugins information file"));
        errorDelay(500); // Wait for all logging messages be processed and store in the DB
        QApplication::quit();
        return;
    }

    if (!doc.setContent(&file))
    {
        logFatal("plugins", "plugins.xml is not a valid XML document");
        QMessageBox::critical(this,
            tr("Plugins Error"),
            tr("The local plugins is not valid"));
        file.close();
        errorDelay(500); // Wait for all logging messages be processed and store in the DB
        QApplication::quit();
        return;
    }
    file.close();
    logInfo("plugins", "Plugins local data loaded and parsed.");

    QString plugin = g_globals->pluginsPath;
    QDir pluginsDir(plugin);
    for (const auto &file : pluginsDir.entryList(QDir::Files))
    {
        QString realFile = pluginsDir.absoluteFilePath(file);
        // Detect if the file is a symbolic link
        QFileInfo info(realFile);
        if (info.isSymLink() || info.isSymbolicLink()
#ifdef Q_OS_WIN
            || info.isShortcut()
#endif /* Q_OS_WIN */
        )
            realFile = info.symLinkTarget();
        QPluginLoader loader(realFile);
        QObject *plugin = loader.instance();
        if (plugin)
        {
            // Add to the list
            auto baseInterface = qobject_cast<cent::plugin::IBase *>(plugin);

            if (baseInterface == nullptr)
                logError("plugin", "The plugin does not implement the IBase interface");
            else
            {
                m_pluginsData.push_back(baseInterface);
                // Init the plugin
                baseInterface->setPluginInterfaces(g_logger, nullptr);

                logInfo("plugin", QString("Plugin found in file: ##F2FEFF#%1#").arg(file));
                if (auto interface = qobject_cast<cent::plugin::IExchange *>(plugin); interface)
                {
                    logInfo("plugin", QString("IExchange plugin found in file: ##F2FEFF#%1#").arg(file));
                    if (!initExchangePlugin(interface))
                    {
                        loader.unload();
                        logWarn("plugin", QString("Plugin IExchange in file: ##F2FEFF#%1#, was unloaded").arg(file));
                    }
                    updatePluginsMenu(interface->getPluginUUID(), doc, baseInterface);
                }
                else if (auto interface = qobject_cast<cent::plugin::IStatus *>(plugin); interface)
                {
                    logInfo("plugin", QString("IStatus plugin found in file: ##F2FEFF#%1#").arg(file));
                    // Init the plugin
                    interface->initialization(m_ui->m_statusBar);
                    updatePluginsMenu(interface->getPluginUUID(), doc, baseInterface);
                }
            }
        }
        else
            loader.unload();
    }
}

void cent::CentaurApp::updatePluginsMenu(const cent::plugin::PluginUUID &uuid, const QDomDocument &doc, plugin::IBase *base) noexcept
{
    auto uuidStr        = fmt::format("{{{:08x}-{:04x}-{:04x}-{:04x}-{:04x}{:08x}}}",
               uuid.dev0.dev0_u,
               uuid.dev1.dev1_s,
               uuid.sp0.sp0_s,
               uuid.sp1.sp1_s,
               uuid.plg.plg0.plg0_s,
               uuid.plg.plg1.plg1_u);

    QDomElement docElem = doc.firstChildElement();

    auto nodeList       = docElem.elementsByTagName("plugin");

    // This is a recursive function intended to parse the XML File and add all the sub menus
    auto insertToMenu = [this, &base](const auto &recursive, QMenu *parent, QDomNode &dataNode) -> void { // NOLINT(misc-no-recursion)
        while (!dataNode.isNull())
        {
            if (dataNode.isElement())
            {
                auto dataElem = dataNode.toElement();
                if (dataElem.nodeName() != "data")
                {
                    dataNode = dataNode.nextSibling();
                    logWarn("plugins", QString(tr("Invalid node name %1. Was expecting 'data'")).arg(dataElem.nodeName()));
                    continue;
                }
                auto type = dataElem.attribute("type");
                auto name = dataElem.attribute("name");
                auto id   = dataElem.attribute("id");
                if (!type.isEmpty())
                {
                    if (type == "sep")
                        parent->addSeparator();
                    else if (type == "sub")
                    {
                        auto newMenu = new QMenu(name, this);
                        auto fc      = dataElem.firstChild();
                        recursive(recursive, newMenu, fc);
                        parent->addMenu(newMenu);
                    }
                }
                else
                {
                    if (name.isEmpty())
                    {
                        logWarn("plugins", QString(tr("%1 does not implement menu options correctly. The menu name is empty")).arg(id));
                        return;
                    }
                    else
                    {

                        auto function = base->connectMenu(id);
                        if (function != nullptr)
                        {
                            auto menuAction = new QAction(name);
                            parent->addAction(menuAction);
                            connect(menuAction, &QAction::triggered, base->getPluginObject(), function);
                        }
                        else
                            logWarn("plugins", QString(tr("%1 the plugin interface does not implement the menu name %2")).arg(id, name));
                    }
                }
            }
            dataNode = dataNode.nextSibling();
        }
    };

    for (int i = 0; i < nodeList.count(); ++i)
    {
        auto node = nodeList.at(i);
        if (node.isElement())
        {
            auto elem = node.toElement();
            auto uuid = elem.attribute("uuid");
            if (uuid == uuidStr.c_str())
            {
                // Parse the menu items
                auto menuNodeList = elem.elementsByTagName("menu");
                if (menuNodeList.isEmpty())
                {
                    logInfo("plugins", QString(tr("%1 does not implement menu options")).arg(uuid));
                    return;
                }

                auto menuNode = menuNodeList.at(0); // Must be one. Everything else will be ignored
                if (!menuNode.isElement())
                {
                    logWarn("plugins", QString(tr("%1 does not implement menu options correctly")).arg(uuid));
                    return;
                }

                auto menuElem     = menuNode.toElement();
                auto mainMenuName = menuElem.attribute("name");
                if (mainMenuName.isEmpty())
                {
                    logWarn("plugins", QString(tr("%1 does not implement menu options correctly. The main menu name is empty")).arg(uuid));
                    return;
                }

                auto mainMenu = new QMenu(mainMenuName, this);
                auto fc       = menuElem.firstChild();
                insertToMenu(insertToMenu, mainMenu, fc);

                m_ui->m_menuBar->insertMenu(m_ui->m_menuView->menuAction(), mainMenu);
            }
        }
    }
}

bool cent::CentaurApp::initExchangePlugin(cent::plugin::IExchange *exchange) noexcept
{
    logTrace("plugins", "CentaurApp::initExchangePlugin");

    auto uuid = exchange->getPluginUUID();

    /// Personal thought. I think {fmt} is good for this kind of stuff
    auto uuidStr = fmt::format("{{{:08x}-{:04x}-{:04x}-{:04x}-{:04x}{:08x}}}",
        uuid.dev0.dev0_u,
        uuid.dev1.dev1_s,
        uuid.sp0.sp0_s,
        uuid.sp1.sp1_s,
        uuid.plg.plg0.plg0_s,
        uuid.plg.plg1.plg1_u);

    logInfo("plugins", QString("Plugin identified itself with the plid: ##F2FEFF#%1#").arg(uuidStr.c_str()));

    if (!exchange->initialization())
    {
        logError("plugins", "Failed to initialize plugin");
        return false;
    }

    cent::plugin::GlobalPluginUUID global { uuid, uuidStr.c_str() };

    auto list                 = populateExchangeSymbolList(exchange, global.id);

    m_exchangeList[global.id] = { global, exchange, list, exchange->getSymbolListName().first };

    // clang-format off
    connect(exchange->getPluginObject(), SIGNAL(sgTickerUpdate(QString,int,quint64,double)), this, SLOT(onTickerUpdate(QString,int,quint64,double)));
    connect(exchange->getPluginObject(), SIGNAL(sgOrderbookUpdate(QString,QString,quint64,QMap<QString,QPair<QString,QString> >,QMap<QString,QPair<QString,QString> >)), this, SLOT(onOrderbookUpdate(QString,QString,quint64,QMap<QString,QPair<QString,QString> >,QMap<QString,QPair<QString,QString> >)));

    // connect(exchange->getPluginObject(), SIGNAL(emitAcceptAsset(QString,int,QList<QPair<QString,QIcon*> >)), this, SLOT(onBalanceAcceptAsset(QString,int,QList<QPair<QString,QIcon*> >)));
    // connect(exchange->getPluginObject(), SIGNAL(emitBalanceUpdate(QList<QString>,int)), this, SLOT(onBalanceUpdate(QList<QString>,int)));
    //  clang-format on
    return true;
}

cent::CenListCtrl *cent::CentaurApp::populateExchangeSymbolList(cent::plugin::IExchange *exchange, const QString &uuidString) noexcept
{
    logTrace("plugins", "CentaurApp::populateExchangeSymbolList");

    auto widget         = new QWidget();
    const auto [name, icon] = exchange->getSymbolListName();

    if (icon != nullptr)
        m_ui->m_ctrlSymbols->addTab(widget, *icon, name);
    else
        m_ui->m_ctrlSymbols->addTab(widget, name);

    auto verticalLayout = new QVBoxLayout(widget);
    verticalLayout->setSpacing(2);
    verticalLayout->setContentsMargins(2, 2, 2, 2);

    auto editCtrl = new QLineEdit(widget);
    editCtrl->setFont(QFont("Arial", 13));
    editCtrl->setPlaceholderText(tr("Search..."));
    editCtrl->setClearButtonEnabled(true);
    editCtrl->addAction(m_icnSearch, QLineEdit::LeadingPosition);
    verticalLayout->addWidget(editCtrl);

    auto symbolsList    = new CenListCtrl(widget);
    auto itemModel      = new QStandardItemModel(0, 1, widget);
    auto sortProxyModel = new QSortFilterProxyModel(widget);
    sortProxyModel->setSourceModel(itemModel);
    symbolsList->setModel(sortProxyModel);
    symbolsList->setObjectName(uuidString);

    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(editCtrl, &QLineEdit::textChanged, sortProxyModel, &QSortFilterProxyModel::setFilterFixedString);
    connect(symbolsList, &CenListCtrl::sgAddToWatchList, this, &CentaurApp::onAddToWatchList);

    symbolsList->verticalHeader()->setFont(QFont("Arial", 10));
    symbolsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    symbolsList->setGridStyle(Qt::NoPen);
    symbolsList->setSortingEnabled(true);
    symbolsList->sortByColumn(0, Qt::AscendingOrder);
    symbolsList->verticalHeader()->setVisible(false);

    itemModel->setHorizontalHeaderLabels({ tr("Symbol") });
    itemModel->horizontalHeaderItem(0)->setFont(QFont("Arial", 10));
    itemModel->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);

    verticalLayout->addWidget(symbolsList);

    auto symbols = exchange->getSymbolList();

    for (const auto &[sym, icon] : symbols)
    {
        auto item  = new QStandardItem(sym);
        int curRow = itemModel->rowCount();
        item->setFont(QFont("Arial", 10));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setIcon(*icon);

        itemModel->insertRow(curRow, item);
    }

    logInfo("plugins", "Exchange list populated");

    return symbolsList;
}

