/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "Globals.hpp"
#include "Logger.hpp"
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
/*
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
*/

#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>

CENTAUR_NAMESPACE::Globals *CENTAUR_NAMESPACE::g_globals { nullptr };

auto CENTAUR_NAMESPACE::Globals::Locale::loadLocale(const QString &language) noexcept -> ErrorDetail
{
    assert(g_globals != nullptr);
    const QString localeFile = QString("%1/Local/%2.xml").arg(g_globals->paths.installPath, language);
    {
        // make the unique_ptr's being release before the call to terminate
        std::unique_ptr<xercesc::XercesDOMParser> parser = std::make_unique<xercesc::XercesDOMParser>();
        parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
        parser->setDoSchema(true);
        parser->setDoNamespaces(true);

        //  std::unique_ptr<xercesc::ErrorHandler> errHandler { static_cast<xercesc::ErrorHandler *>(new xercesc::HandlerBase()) };
        //   parser->setErrorHandler(errHandler.get());

        if (!QFile::exists(localeFile))
            return ErrorDetail::localeFileError;
        try
        {

            char *xmlFile = localeFile.toLatin1().data();
            parser->parse(xmlFile);
            auto doc        = parser->getDocument();

            auto docElement = doc->getDocumentElement();

            if (docElement == nullptr)
                return ErrorDetail::emptyDocument;

            auto langAttribute = docElement->getAttribute(XMLStr { "lang" });

            if (langAttribute == nullptr || QString { StrXML { langAttribute } } != language)
                return ErrorDetail::langError;

            XMLStr localeName { "locale" };
            XMLStr idName { "id" };

            auto nodes     = docElement->getElementsByTagName(localeName);
            const auto len = nodes->getLength();

            for (auto idx = 0ull; idx < len; ++idx)
            {
                auto node        = nodes->item(idx);

                auto attributes  = node->getAttributes();
                auto idAttribute = attributes->getNamedItem(idName);
                if (idAttribute != nullptr)
                {
                    StrXML idValue { idAttribute->getNodeValue() };

                    auto textNode = node->getFirstChild();
                    if (textNode != nullptr && textNode->getNodeType() == xercesc::DOMNode::NodeType::TEXT_NODE)
                    {
                        auto valueNode = dynamic_cast<xercesc::DOMText *>(textNode);
                        StrXML localeValue { valueNode->getNodeValue() };

                        const uint64_t crc64 = CENTAUR_NAMESPACE::crc64::compute(idValue, strlen(idValue));
                        if (m_locale.contains(crc64))
                        {
                            qDebug() << "CRC64 for " << localeValue << "already exists";
                        }
                        else
                            m_locale[crc64] = localeValue;
                    }
                }
                else
                {
                    logWarn("app", QString("Locale tag %1 does not have the id attribute").arg(QString { StrXML { node->getNodeName() } }));
                }
            }
        } catch (const xercesc::XMLException &ex)
        {
            char *message = xercesc::XMLString::transcode(ex.getMessage());
            qDebug() << "Exception message is: \n"
                     << message << "\n";
            xercesc::XMLString::release(&message);
            return ErrorDetail::xmlDocumentInvalid;
        }
    }

    return ErrorDetail::noError;
}

namespace
{
// clang-format off
    #if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_PUSH()
    CENTAUR_WARN_OFF(exit-time-destructors)
    CENTAUR_WARN_OFF(global-constructors)
    #endif
    // clang-format on
    const QString emptyString {};
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_POP()
#endif

} // namespace

auto cen::Globals::Locale::get(uint64_t crc) const -> const QString &
{
    auto key = m_locale.find(crc);
    if (key == m_locale.end())
        return emptyString;
    return key->second;
}

auto cen::Globals::VisualsUI::loadVisualsUI() noexcept -> cen::Globals::VisualsUI::ErrorDetail
{
    { // make the unique_ptr's being release before the call to terminate
        std::unique_ptr<xercesc::XercesDOMParser> parser = std::make_unique<xercesc::XercesDOMParser>();
        parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
        parser->setDoSchema(true);
        parser->setDoNamespaces(true);
        parser->setValidationSchemaFullChecking(true);

        //  std::unique_ptr<xercesc::ErrorHandler> errHandler { static_cast<xercesc::ErrorHandler *>(new xercesc::HandlerBase()) };
        //   parser->setErrorHandler(errHandler.get());

        QString file       = g_globals->paths.resPath + "/ui.xml";
        QString fileSchema = g_globals->paths.resPath + "/ui-schema.xsd";
        if (!QFile::exists(file) || !QFile::exists(fileSchema))
            return cen::Globals::VisualsUI::ErrorDetail::visualsFileError;
        try
        {
            parser->setExternalNoNamespaceSchemaLocation(fileSchema.toLatin1().data());
            char *xmlFile = file.toLatin1().data();

            parser->parse(xmlFile);

            cen::Globals::VisualsUI::allocateVisuals();

            auto doc = parser->getDocument();

            parseVisuals(doc);

        } catch (const xercesc::XMLException &ex)
        {
            char *message = xercesc::XMLString::transcode(ex.getMessage());
            qDebug() << "Exception message is: \n"
                     << message << "\n";
            xercesc::XMLString::release(&message);
            return cen::Globals::VisualsUI::ErrorDetail::xmlDocumentInvalid;
        }
    }

    return cen::Globals::VisualsUI::ErrorDetail::noError;
}

auto cen::Globals::VisualsUI::allocateVisuals() noexcept -> void
{
    assert(g_globals != nullptr);
    g_globals->visuals.dockSymbols = new DockSymbols;
}

auto cen::Globals::VisualsUI::releaseVisuals() noexcept -> void
{
    assert(g_globals != nullptr);
    if (g_globals->visuals.dockSymbols)
    {
        delete g_globals->visuals.dockSymbols;
        g_globals->visuals.dockSymbols = nullptr;
    }

    logTrace("app", LS("trace-visuals-released"));
}

auto cen::Globals::VisualsUI::parseVisuals(xercesc::DOMDocument *doc) -> void
{
    assert(doc != nullptr);

    auto parseAttributesAsFont = [](xercesc::DOMNamedNodeMap *atts) -> cen::Globals::VisualsUI::VisualsFont {
        cen::Globals::VisualsUI::VisualsFont vf;

        auto font = atts->getNamedItem(XMLStr { "font" });
        if (font != nullptr)
            vf.name = StrXML { font->getNodeValue() };
        auto size = atts->getNamedItem(XMLStr { "size" });
        if (size != nullptr)
            vf.size = std::stoi({ StrXML { size->getNodeValue() } });
        auto weight = atts->getNamedItem(XMLStr { "weight" });
        if (weight != nullptr)
            vf.weight = std::stoi({ StrXML { weight->getNodeValue() } });
        auto italic = atts->getNamedItem(XMLStr { "italic" });
        if (italic != nullptr)
        {
            vf.italic = [&italic]() -> bool {
                if (xercesc::XMLString::equals(italic->getNodeValue(), XMLStr { "true" }))
                    return true;
                return false;
            }();
        }

        auto spacing = atts->getNamedItem(XMLStr { "spacing" });
        if (spacing != nullptr)
            vf.spacing = std::stod({ StrXML { spacing->getNodeValue() } });
        return vf;
    };

    auto parseCDATAFromChildren = [](xercesc::DOMNode *node) -> QString {
        auto nodes = node->getChildNodes();

        if (nodes == nullptr)
            return "";

        const auto len = nodes->getLength();

        for (auto i = 0ull; i < len; ++i)
        {
            auto child = nodes->item(i);
            auto type  = child->getNodeType();

            if (type == xercesc::DOMNode::NodeType::CDATA_SECTION_NODE)
            {
                auto cdataNode = dynamic_cast<xercesc::DOMCDATASection *>(child);
                return QString { StrXML { cdataNode->getNodeValue() } };
            }
        }

        return "";
    };

    XMLStr dockSymbolsu16 { "docks-symbols" };
    auto symbolsData = doc->getElementById(dockSymbolsu16);

    if (symbolsData != nullptr)
    {
        auto nodeList   = symbolsData->getChildNodes();
        const auto size = nodeList->getLength();

        XMLStr headerStr { "header" };
        XMLStr tableStr { "table" };
        XMLStr searchStr { "search" };

        for (auto i = 0ull; i < size; ++i)
        {
            auto node = nodeList->item(i);

            if (xercesc::XMLString::equals(node->getNodeName(), headerStr))
            {
                g_globals->visuals.dockSymbols->headerFont = parseAttributesAsFont(node->getAttributes());
                g_globals->visuals.dockSymbols->headerCSS  = parseCDATAFromChildren(node);
            }
            else if (xercesc::XMLString::equals(node->getNodeName(), tableStr))
            {
                g_globals->visuals.dockSymbols->tableFont = parseAttributesAsFont(node->getAttributes());
                g_globals->visuals.dockSymbols->tableCSS  = parseCDATAFromChildren(node);
            }
            else if (xercesc::XMLString::equals(node->getNodeName(), searchStr))
            {
                g_globals->visuals.dockSymbols->searchFont = parseAttributesAsFont(node->getAttributes());
                g_globals->visuals.dockSymbols->searchCSS  = parseCDATAFromChildren(node);
            }
        }
    }
}
