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
                    baseInterface->setPluginInterfaces(g_logger, nullptr);
                    logInfo("plugin", QString(LS("info-plugin-found")).arg(plFile));

                    if (auto stInterface = qobject_cast<CENTAUR_PLUGIN_NAMESPACE::IStatus *>(plugin); stInterface)
                    {
                        logInfo("plugin", QString(LS("info-plugin-istatus")).arg(plFile));
                        // Init the plugin
                        stInterface->initialization(m_ui->m_statusBar);
                        updatePluginsMenu(stInterface->getPluginUUID(), doc, baseInterface);
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

void CENTAUR_NAMESPACE::CentaurApp::updatePluginsMenu(const uuid &uuid, xercesc::DOMDocument *doc, CENTAUR_PLUGIN_NAMESPACE::IBase *base) noexcept
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
                //// auto id   = dataElem.getAttribute("id");
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
                    else
                    {
                        //   auto function = base->connectMenu(id);
                        //   if (function != nullptr)
                        //   {
                        auto menuAction = new QAction(QString { StrXML { name->getNodeValue() } });
                        parent->addAction(menuAction);
                        //       connect(menuAction, &QAction::triggered, base->getPluginObject(), function);
                        //   }
                        //   else
                        //       logWarn("plugins", QString(tr("%1 the plugin interface does not implement the menu name %2")).arg(id, name));
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
