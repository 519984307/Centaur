/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// General aspects of plugins

#include "CentaurApp.hpp"
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPluginLoader>
#include <QString>
#include <QUrl>

#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>

void CENTAUR_NAMESPACE::CentaurApp::loadPlugins() noexcept
{
    logTrace("plugins", "CentaurApp::loadPlugins()");

    try
    {
        xercesc::XMLPlatformUtils::Initialize();
    } catch (const xercesc::XMLException &ex)
    {
        char *message = xercesc::XMLString::transcode(ex.getMessage());
        qDebug() << "Error during initialization!" << message << "\n";
        xercesc::XMLString::release(&message);
    }

    auto errorDelay = [](const int &ms) {
        QTime dieTime = QTime::currentTime().addMSecs(ms);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    };

    m_longOperation       = std::make_unique<LongOperation>();

    const QString xmlFile = g_globals->paths.pluginsPath + "/local/plugins.xml";
    xercesc::DOMDocument *doc { nullptr };
    {
        // make the unique_ptr's being release before the call to terminate
        std::unique_ptr<xercesc::XercesDOMParser> parser = std::make_unique<xercesc::XercesDOMParser>();
        parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
        parser->setDoSchema(true);
        parser->setDoNamespaces(true);

        //  std::unique_ptr<xercesc::ErrorHandler> errHandler { static_cast<xercesc::ErrorHandler *>(new xercesc::HandlerBase()) };
        //   parser->setErrorHandler(errHandler.get());

        if (!QFile::exists(xmlFile))
        {
            logFatal("plugins", LS("fatal-plugins-file"));
            QMessageBox::critical(this,
                LS("fatal-plugins-file-title-ui"),
                LS("fatal-plugins-file-ui"));
            errorDelay(500); // Wait for all logging messages be processed and store in the DB
            QApplication::quit();
            return;
        }
        try
        {
            parser->parse(xmlFile.toLatin1().data());
            doc = parser->getDocument();

        } catch (const xercesc::XMLException &ex)
        {
            logFatal("plugins", LS("fatal-plugins-file-invalid"));
            QMessageBox::critical(this,
                LS("fatal-plugins-file-title-ui"),
                LS("fatal-plugins-file-invalid-ui"));
            errorDelay(500); // Wait for all logging messages be processed and store in the DB
            QApplication::quit();
            return;
        }

        logInfo("plugins", LS("info-plugins-file-loaded"));

        QString pluginPath = g_globals->paths.pluginsPath;
        QDir pluginsDir(pluginPath);
        for (const auto &plFile : pluginsDir.entryList(QDir::Files))
        {
            QString realFile = pluginsDir.absoluteFilePath(plFile);
            // Detect if the file is a symbolic link
            QFileInfo info(realFile);
            if (info.isSymLink() || info.isSymbolicLink()
#ifdef Q_OS_WIN
                || info.isShortcut()
#endif /* Q_OS_WIN */
            )
            {
                realFile = info.symLinkTarget();
            }

            QPluginLoader loader(realFile);
            QObject *plugin = loader.instance();

            if (plugin)
            {
                // Add to the list
                auto baseInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IBase *>(plugin);

                if (baseInterface == nullptr)
                    logError("plugin", LS("error-file-not-plugin"));
                else
                {
                    m_pluginsData.push_back(baseInterface);
                    // Init the plugin

                    auto pluginConfig = new PluginConfiguration(baseInterface->getPluginUUID().to_string().c_str());

                    loadPluginLocalData(baseInterface->getPluginUUID(), doc, pluginConfig);
                    baseInterface->setPluginInterfaces(g_logger,
                        static_cast<CENTAUR_INTERFACE_NAMESPACE::IConfiguration *>(pluginConfig),
                        static_cast<CENTAUR_INTERFACE_NAMESPACE::ILongOperation *>(m_longOperation.get()));

                    logInfo("plugin", QString(LS("info-plugin-found")).arg(plFile));
                    // Generate the plugin data
                    m_configurationInterface[baseInterface->getPluginUUID().to_string().c_str()] = pluginConfig;

                    if (auto exInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IExchange *>(plugin); exInterface)
                    {
                        logInfo("plugin", QString(LS("info-plugin-iexchange")).arg(plFile));
                        if (!initExchangePlugin(exInterface))
                        {
                            loader.unload();
                            logWarn("plugin", QString(LS("warning-iexchange-plugin-unloaded")).arg(plFile));
                        }
                        else
                            updatePluginsMenus(exInterface->getPluginUUID(), doc, baseInterface);
                    }
                    else if (auto stInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IStatus *>(plugin); stInterface)
                    {
                        logInfo("plugin", QString(LS("info-plugin-istatus")).arg(plFile));
                        // Init the plugin
                        stInterface->initialization(m_ui->m_statusBar);
                        updatePluginsMenus(stInterface->getPluginUUID(), doc, baseInterface);
                    }
                }
            }
            else
                loader.unload();
        }
    }

    try
    {
        xercesc::XMLPlatformUtils::Terminate();
    } catch (const xercesc::XMLException &ex)
    {
        char *message = xercesc::XMLString::transcode(ex.getMessage());
        qDebug() << "Error during termination!" << message << "\n";
        xercesc::XMLString::release(&message);
    }
}

void CENTAUR_NAMESPACE::CentaurApp::updatePluginsMenus(const uuid &uuid, xercesc::DOMDocument *doc, CENTAUR_PLUGIN_NAMESPACE::IBase *base) noexcept
{

    auto pluginUuidStr = XMLStr { uuid.to_string().c_str() };

    auto docElem       = doc->getDocumentElement();

    if (docElem == nullptr)
    {
        logWarn("plugins", LS("warning-plugins-file-empty"));
        return;
    }
    auto nodeList = docElem->getElementsByTagName(XMLStr { "plugin" });

    // This is a recursive function intended to parse the XML File and add all the sub menus
    auto insertToMenu = [this, &base](const auto &recursive, QMenu *parent, xercesc::DOMNode *dataNode) -> void { // NOLINT(misc-no-recursion)
        auto typeStr = XMLStr { "type" };
        auto dataStr = XMLStr { "data" };
        auto nameStr = XMLStr { "name" };
        auto sepStr  = XMLStr { "sep" };
        auto subStr  = XMLStr { "sub" };
        auto idStr   = XMLStr { "id" };

        while (dataNode)
        {
            if (dataNode->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
            {
                if (!xercesc::XMLString::equals(dataNode->getNodeName(), dataStr))
                {
                    logWarn("plugins", QString(LS("warning-plugin-data-expected")).arg(QString { StrXML { dataNode->getNodeName() } }));
                    dataNode = dataNode->getNextSibling();
                    continue;
                }

                auto dataElem = dynamic_cast<xercesc::DOMElement *>(dataNode);

                auto type     = dataElem->getAttributeNode(typeStr);
                auto name     = dataElem->getAttributeNode(nameStr);
                auto id       = dataElem->getAttributeNode(idStr);

                if (type != nullptr)
                {
                    if (xercesc::XMLString::equals(type->getNodeValue(), sepStr))
                        parent->addSeparator();
                    else if (xercesc::XMLString::equals(type->getNodeValue(), subStr))
                    {
                        if (name == nullptr || xercesc::XMLString::stringLen(name->getNodeValue()) == 0)
                        {
                            logWarn("plugins", QString(LS("warning-plugin-name-expected")).arg(base->getPluginName()));
                            dataNode = dataNode->getNextSibling();
                            continue;
                        }

                        auto newMenu = new QMenu(QString { StrXML { name->getNodeValue() } }, this);
                        auto fc      = dataElem->getFirstChild();
                        recursive(recursive, newMenu, fc);
                        parent->addMenu(newMenu);
                    }
                }
                else
                {

                    if (name == nullptr || xercesc::XMLString::stringLen(name->getNodeValue()) == 0)
                    {
                        logWarn("plugins", QString(LS("warning-plugin-name-expected")).arg(base->getPluginName()));
                        dataNode = dataNode->getNextSibling();
                        continue;
                    }

                    if (id == nullptr || xercesc::XMLString::stringLen(id->getNodeValue()) == 0)
                    {
                        logWarn("plugins", QString(LS("warning-plugin-id-expected")).arg(base->getPluginName()));
                        dataNode = dataNode->getNextSibling();
                        continue;
                    }

                    else
                    {


                        auto menuAction = new QAction(QString { StrXML { name->getNodeValue() } });
                        parent->addAction(menuAction);
                        const StrXML idValue { id->getNodeValue() };
                        auto res = base->addMenuAction(menuAction, CENTAUR_NAMESPACE::uuid { std::string { idValue } });
                        if (!res)
                            menuAction->setDisabled(true);
                    }
                }
            }

            dataNode = dataNode->getNextSibling();
        }
    };

    auto uuidStr = XMLStr { "uuid" };
    auto menuStr = XMLStr { "menu" };
    auto nameStr = XMLStr { "name" };

    for (auto i = 0ull; i < nodeList->getLength(); ++i)
    {
        auto node = nodeList->item(i);

        if (node->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
        {
            auto attributes = node->getAttributes();
            if (attributes == nullptr || attributes->getLength() == 0ull)
                continue;

            auto attribute = attributes->getNamedItem(uuidStr);
            if (attribute == nullptr)
                continue;

            if (!xercesc::XMLString::equals(attribute->getNodeValue(), pluginUuidStr))
                continue;

            if (!node->hasChildNodes())
                continue;

            auto nodeChildren = (dynamic_cast<xercesc::DOMElement *>(node))->getElementsByTagName(menuStr);
            if (nodeChildren->getLength() == 0)
            {
                logInfo("plugins", QString(LS("info-plugin-no-menus")).arg(base->getPluginName()));
                continue;
            }

            // This will only use the first menu item, everything else will be ignored
            auto menuItem       = nodeChildren->item(0);

            auto menuAttributes = menuItem->getAttributes();
            if (menuAttributes == nullptr || menuAttributes->getLength() == 0ull)
                continue;

            auto menuNameAttribute = menuAttributes->getNamedItem(nameStr);

            if (menuNameAttribute == nullptr || xercesc::XMLString::stringLen(menuNameAttribute->getNodeValue()) == 0)
            {
                logWarn("plugins", QString(LS("warning-plugin-no-menu-name")).arg(base->getPluginName()));
                continue;
            }

            auto mainMenu = new QMenu(QString { StrXML { menuNameAttribute->getNodeValue() } }, this);

            insertToMenu(insertToMenu, mainMenu, menuItem->getFirstChild());

            m_ui->m_menuBar->insertMenu(m_ui->m_menuView->menuAction(), mainMenu);
        }
    }
}

void CENTAUR_NAMESPACE::CentaurApp::loadPluginLocalData(const CENTAUR_NAMESPACE::uuid &uuid, xercesc::DOMDocument *doc, PluginConfiguration *config) noexcept
{
    auto pluginUuidStr = XMLStr { uuid.to_string().c_str() };

    auto docElem       = doc->getDocumentElement();

    if (docElem == nullptr)
    {
        logWarn("plugins", LS("warning-plugins-file-empty"));
        return;
    }

    auto nodeList = docElem->getElementsByTagName(XMLStr { "plugin" });

    auto uuidStr  = XMLStr { "uuid" };
    auto localStr = XMLStr { "local" };

    for (auto i = 0ull; i < nodeList->getLength(); ++i)
    {
        auto node = nodeList->item(i);

        if (node->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
        {
            auto attributes = node->getAttributes();
            if (attributes == nullptr || attributes->getLength() == 0ull)
                continue;

            auto attribute = attributes->getNamedItem(uuidStr);
            if (attribute == nullptr)
                continue;

            if (!xercesc::XMLString::equals(attribute->getNodeValue(), pluginUuidStr))
                continue;

            if (!node->hasChildNodes())
                continue;

            auto nodeChildrenLocals = (dynamic_cast<xercesc::DOMElement *>(node))->getElementsByTagName(localStr);

            if (nodeChildrenLocals->getLength() == 0)
                continue;

            auto nodeChildren = nodeChildrenLocals->item(0)->getChildNodes(); // Only one element will be parsed
            for (auto idx = 0ull; idx < nodeChildren->getLength(); ++idx)
            {
                auto localChildren = nodeChildren->item(idx);
                if (localChildren->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
                {
                    StrXML localNodeName { localChildren->getNodeName() };
                    auto localNodeChildren = localChildren->getChildNodes();
                    for (auto jdx = 0ull; jdx < localNodeChildren->getLength(); ++jdx)
                    {
                        auto localNodeChild = localNodeChildren->item(jdx);
                        if (localNodeChild->getNodeType() == xercesc::DOMNode::NodeType::TEXT_NODE)
                        {
                            auto value = const_cast<XMLCh *>(localNodeChild->getNodeValue());
                            xercesc::XMLString::trim(value);
                            config->addValue(std::string { localNodeName }, std::string { StrXML { value } });
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool CENTAUR_NAMESPACE::CentaurApp::initExchangePlugin(CENTAUR_NAMESPACE::plugin::IExchange *exchange) noexcept
{
    logTrace("plugins", "CentaurApp::initExchangePlugin");

    const auto uuid = exchange->getPluginUUID();

    if (!exchange->initialization())
    {
        logError("plugins", LS("error-iexchange-plugin-init"));
        return false;
    }

    auto list                                            = populateExchangeSymbolList(exchange, uuid.to_string().c_str());

    m_exchangeList[QString { uuid.to_string().c_str() }] = ExchangeInformation { uuid, exchange, list, exchange->getSymbolListName().first };

    // clang-format off
    connect(exchange->getPluginObject(), SIGNAL(snTickerUpdate(QString,int,quint64,double)), this, SLOT(onTickerUpdate(QString,int,quint64,double)));
    connect(exchange->getPluginObject(), SIGNAL(snOrderbookUpdate(QString,QString,quint64,QMap<qreal,QPair<qreal,qreal> >,QMap<qreal,QPair<qreal,qreal> >)), this, SLOT(onOrderbookUpdate(QString, QString, quint64, QMap<qreal,QPair<qreal,qreal> >,QMap<qreal,QPair<qreal,qreal> >)));
    // connect(exchange->getPluginObject(), SIGNAL(emitAcceptAsset(QString,int,QList<QPair<QString,QIcon*> >)), this, SLOT(onBalanceAcceptAsset(QString,int,QList<QPair<QString,QIcon*> >)));
    // connect(exchange->getPluginObject(), SIGNAL(emitBalanceUpdate(QList<QString>,int)), this, SLOT(onBalanceUpdate(QList<QString>,int)));
    // clang-format on

    return true;
}

CENTAUR_NAMESPACE::CenListCtrl *CENTAUR_NAMESPACE::CentaurApp::populateExchangeSymbolList(CENTAUR_NAMESPACE::plugin::IExchange *exchange, const QString &uuidString) noexcept
{
    logTrace("plugins", "CentaurApp::populateExchangeSymbolList");

    auto widget             = new QWidget();
    const auto [name, icon] = exchange->getSymbolListName();

    if (icon != nullptr)
        m_ui->tabSymbols->addTab(widget, *icon, name);
    else
        m_ui->tabSymbols->addTab(widget, name);

    auto verticalLayout = new QVBoxLayout(widget);
    verticalLayout->setSpacing(2);
    verticalLayout->setContentsMargins(2, 2, 2, 2);

    auto editCtrl = new QLineEdit(widget);
    editCtrl->setFont(m_ui->editWatchListFilter->font());
    editCtrl->setPlaceholderText(LS("ui-docks-search"));
    editCtrl->setClearButtonEnabled(true);
    editCtrl->addAction(g_globals->icons.searchIcon, QLineEdit::LeadingPosition);
    editCtrl->setStyleSheet(m_ui->editWatchListFilter->styleSheet());
    verticalLayout->addWidget(editCtrl);

    auto symbolsList    = new CenListCtrl(widget);
    auto itemModel      = new QStandardItemModel(0, 1, widget);
    auto sortProxyModel = new QSortFilterProxyModel(widget);
    symbolsList->setStyleSheet(m_ui->listWatchList->horizontalHeader()->styleSheet());

    sortProxyModel->setSourceModel(itemModel);
    symbolsList->setModel(sortProxyModel);
    symbolsList->setObjectName(uuidString);

    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(editCtrl, &QLineEdit::textChanged, sortProxyModel, &QSortFilterProxyModel::setFilterFixedString);
    connect(symbolsList, &CenListCtrl::snAddToWatchList, this, &CentaurApp::onAddToWatchList);

    symbolsList->verticalHeader()->setFont(g_globals->fonts.symbolsDock.headerFont);
    symbolsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    symbolsList->setGridStyle(Qt::NoPen);
    symbolsList->setSortingEnabled(true);
    symbolsList->sortByColumn(0, Qt::AscendingOrder);
    symbolsList->verticalHeader()->setVisible(false);

    itemModel->setHorizontalHeaderLabels({ LS("ui-docks-symbol") });
    itemModel->horizontalHeaderItem(0)->setFont(g_globals->fonts.symbolsDock.headerFont);
    itemModel->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    verticalLayout->addWidget(symbolsList);

    auto symbols = exchange->getSymbolList();

    for (const auto &[sym, icon] : symbols)
    {
        auto item  = new QStandardItem(sym);
        int curRow = itemModel->rowCount();
        item->setFont(g_globals->fonts.symbolsDock.tableFont);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setIcon(*icon);

        itemModel->insertRow(curRow, item);
    }

    logInfo("plugins", "Exchange list populated");

    return symbolsList;
}
