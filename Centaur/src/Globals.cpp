/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "Globals.hpp"
#include "../ui/ui_CentaurApp.h"
#include "Logger.hpp"
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <concepts>
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

auto cen::Globals::VisualsUI::loadVisualsUI(Ui::CentaurApp *ui) noexcept -> cen::Globals::VisualsUI::ErrorDetail
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

            auto doc = parser->getDocument();

            parseVisuals(doc, ui);

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

static bool operator==(const CENTAUR_NAMESPACE::XMLStr &p1, const CENTAUR_NAMESPACE::XMLStr &p2)
{
    return xercesc::XMLString::equals(p1, p2);
}

static bool operator==(const XMLCh *p1, const CENTAUR_NAMESPACE::XMLStr &p2)
{
    return xercesc::XMLString::equals(p1, p2);
}

auto cen::Globals::VisualsUI::parseVisuals(xercesc::DOMDocument *doc, Ui::CentaurApp *ui) -> void
{
    assert(doc != nullptr);

    const XMLStr headerStr { "header" };
    const XMLStr tableStr { "table" };
    const XMLStr searchStr { "search" };
    const XMLStr titleStr { "title" };
    const XMLStr qssStr { "qss" };
    const XMLStr priceStr { "price" };
    const XMLStr bidsStr { "bids" };
    const XMLStr asksStr { "asks" };
    const XMLStr fontStr { "font" };
    const XMLStr familyStr { "family" };
    const XMLStr sizeStr { "size" };
    const XMLStr weightStr { "weight" };
    const XMLStr italicStr { "italic" };
    const XMLStr spacingStr { "spacing" };
    const XMLStr latencyStr { "latency" };
    const XMLStr lowStr { "low" };
    const XMLStr mediumStr { "medium" };
    const XMLStr highStr { "high" };
    const XMLStr upColorStr { "up_color" };
    const XMLStr downColorStr { "down_color" };
    const XMLStr neutralColorStr { "neutral_color" };
    const XMLStr rangeMinStr { "range_min" };
    const XMLStr rangeMaxStr { "range_max" };
    const XMLStr colorStr { "color" };
    const XMLStr latFontStr { "latency_font" };

    auto retrieveAttributeString = [](xercesc::DOMAttr *attr) -> QString {
        if (attr != nullptr)
            return QString { StrXML { attr->getNodeValue() } };
        return "";
    };

    auto retrieveAttributeApplyOperation = [&retrieveAttributeString](xercesc::DOMAttr *attr, std::invocable<QString> auto fn) {
        QString s = retrieveAttributeString(attr);
        if (!s.isEmpty())
            fn(s);
    };

    auto stringToBool = [](const QString &str) -> bool {
        return str == "true";
    };

    auto parseFont = [&](QFont &font, xercesc::DOMElement *node) {
        // <font family="Roboto" size="10" weight="400" italic="false" spacing="0"/>>
        retrieveAttributeApplyOperation(node->getAttributeNode(familyStr), [&font](const QString &s) { font.setFamily(s); });
        retrieveAttributeApplyOperation(node->getAttributeNode(sizeStr), [&font](const QString &s) { font.setPointSize(s.toInt()); });
        retrieveAttributeApplyOperation(node->getAttributeNode(weightStr), [&font](const QString &s) {
            int weight = s.toInt();
            if (weight % 100 || weight < 0 || weight > 900)
                weight = 400;
            font.setWeight(static_cast<QFont::Weight>(weight)); });
        retrieveAttributeApplyOperation(node->getAttributeNode(italicStr), [&font, &stringToBool](const QString &s) { font.setItalic(stringToBool(s)); });
        retrieveAttributeApplyOperation(node->getAttributeNode(spacingStr), [&font](const QString &s) { font.setLetterSpacing(QFont::SpacingType::PercentageSpacing, s.toDouble()); });
    };

    auto parseQSS = [](xercesc::DOMElement *node) -> QString {
        auto nodes = node->getChildNodes();
        for (auto idx = 0ull; idx < nodes->getLength(); ++idx)
        {
            auto node = nodes->item(idx);
            if (node->getNodeType() == xercesc::DOMNode::NodeType::TEXT_NODE)
            {
                auto textNode = dynamic_cast<xercesc::DOMText *>(node);

                QString file  = QString { StrXML { textNode->getData() } };
                QString path  = g_globals->paths.resPath + "/StyleSheet/" + file;
                QFile fl(path);
                if (fl.open(QIODeviceBase::ReadOnly))
                {
                    QTextStream textStream(&fl);
                    return textStream.readAll();
                }
                else
                {
                    logError("app", QString(LS("error-qss-not-found")).arg(file));
                    return "";
                }
            }
        }
        return "";
    };

    auto parseHeader = [&qssStr, &fontStr, &parseQSS, &parseFont](xercesc::DOMElement *node, QString &qss, QFont &font) {
        // Parses the font and qss style
        auto nodes = node->getChildNodes();
        for (auto idx = 0ull; idx < nodes->getLength(); ++idx)
        {
            auto node = nodes->item(idx);
            if (node->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
            {
                auto elementNode = dynamic_cast<xercesc::DOMElement *>(node);
                if (elementNode->getNodeName() == qssStr)
                    qss = parseQSS(elementNode);
                else if (elementNode->getNodeName() == fontStr)
                    parseFont(font, elementNode);
            }
        }
    };

    auto parseSearchEdit = [&parseHeader](xercesc::DOMElement *node, QLineEdit *edit) {
        QFont font;
        QString qss;
        parseHeader(node, qss, font);
        if (!qss.isEmpty())
            edit->setStyleSheet(qss);
        edit->setFont(font);
    };

    auto parseLabel = [&parseHeader](xercesc::DOMElement *node, QLabel *label) {
        QString qss;
        QFont font;
        parseHeader(node, qss, font);
        if (!qss.isEmpty())
            label->setStyleSheet(qss);
        label->setFont(font);
    };

    auto parseSimpleTable = [&parseHeader](xercesc::DOMElement *node, QString &qss, QFont &font) {
        parseHeader(node, qss, font);
    };

    auto parseLatency = [&lowStr, &mediumStr, &highStr, &rangeMinStr, &rangeMaxStr, &colorStr, &retrieveAttributeApplyOperation](
                            xercesc::DOMElement *node,
                            uint64_t &lowMin, uint64_t &lowMax,
                            uint64_t &medMin, uint64_t &medMax,
                            uint64_t &highMin, uint64_t &highMax,
                            QColor &lowColor, QColor &medColor, QColor &highColor) {
        auto nodes = node->getChildNodes();
        for (auto idx = 0ull; idx < nodes->getLength(); ++idx)
        {
            auto node = nodes->item(idx);
            if (node->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
            {
                auto elementNode = dynamic_cast<xercesc::DOMElement *>(node);
                if (elementNode->getNodeName() == lowStr)
                {
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(rangeMinStr), [&lowMin](const QString &s) { lowMin = s.toULongLong(); });
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(rangeMaxStr), [&lowMax](const QString &s) { lowMax = s.toULongLong(); });
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(colorStr), [&lowColor](const QString &s) { lowColor.setNamedColor(s); });
                }
                else if (elementNode->getNodeName() == mediumStr)
                {
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(rangeMinStr), [&medMin](const QString &s) { medMin = s.toULongLong(); });
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(rangeMaxStr), [&medMax](const QString &s) { medMax = s.toULongLong(); });
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(colorStr), [&medColor](const QString &s) { medColor.setNamedColor(s); });
                }
                else if (elementNode->getNodeName() == highStr)
                {
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(rangeMinStr), [&highMin](const QString &s) { highMin = s.toULongLong(); });
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(rangeMaxStr), [&highMax](const QString &s) { highMax = s.toULongLong(); });
                    retrieveAttributeApplyOperation(elementNode->getAttributeNode(colorStr), [&highColor](const QString &s) { highColor.setNamedColor(s); });
                }
            }
        }
    };

    XMLStr dockSymbolsu16 { "docks-symbols" };
    auto symbolsData = doc->getElementById(dockSymbolsu16);
    if (symbolsData != nullptr)
    {
        auto nodes = symbolsData->getChildNodes();
        for (auto idx = 0ull; idx < nodes->getLength(); ++idx)
        {
            auto node = nodes->item(idx);
            if (node->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
            {
                auto elementNode = dynamic_cast<xercesc::DOMElement *>(node);
                //// qDebug() << StrXML { elementNode->getNodeName() };
                if (elementNode->getNodeName() == headerStr)
                {
                    QString qssHeader;
                    parseHeader(elementNode, qssHeader, g_globals->fonts.symbolsDock.headerFont);
                    if (!qssHeader.isEmpty())
                        ui->listWatchList->horizontalHeader()->setStyleSheet(qssHeader);
                }
                else if (elementNode->getNodeName() == searchStr)
                {
                    parseSearchEdit(elementNode, ui->editWatchListFilter);
                }
                else if (elementNode->getNodeName() == tableStr)
                {
                    auto tableNodes = elementNode->getChildNodes();
                    for (auto idxTable = 0ull; idxTable < tableNodes->getLength(); ++idxTable)
                    {
                        auto tableNode = tableNodes->item(idxTable);
                        if (tableNode->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
                        {
                            auto tableElementNode = dynamic_cast<xercesc::DOMElement *>(tableNode);
                            if (tableElementNode->getNodeName() == qssStr)
                            {
                                const QString tableStyleSheet = parseQSS(tableElementNode);
                                if (!tableStyleSheet.isEmpty())
                                    ui->listWatchList->setStyleSheet(tableStyleSheet);
                            }
                            else if (tableElementNode->getNodeName() == fontStr)
                            {
                                parseFont(g_globals->fonts.symbolsDock.tableFont, tableElementNode);
                            }
                            else if (tableElementNode->getNodeName() == priceStr)
                            {
                                g_globals->colors.symbolsDockColors.priceUp.setNamedColor(retrieveAttributeString(tableElementNode->getAttributeNode(upColorStr)));
                                g_globals->colors.symbolsDockColors.priceDown.setNamedColor(retrieveAttributeString(tableElementNode->getAttributeNode(downColorStr)));
                                g_globals->colors.symbolsDockColors.priceNeutral.setNamedColor(retrieveAttributeString(tableElementNode->getAttributeNode(neutralColorStr)));
                            }
                            else if (tableElementNode->getNodeName() == latencyStr)
                            {
                                parseLatency(tableElementNode,
                                    g_globals->params.symbolsDockParameters.latencyLowMin,
                                    g_globals->params.symbolsDockParameters.latencyLowMax,
                                    g_globals->params.symbolsDockParameters.latencyMediumMin,
                                    g_globals->params.symbolsDockParameters.latencyMediumMax,
                                    g_globals->params.symbolsDockParameters.latencyHighMin,
                                    g_globals->params.symbolsDockParameters.latencyHighMax,
                                    g_globals->colors.symbolsDockColors.latencyLow,
                                    g_globals->colors.symbolsDockColors.latencyMedium,
                                    g_globals->colors.symbolsDockColors.latencyHigh);
                            }
                        } // if (tableNode)
                    }     // for
                }         // if( nodes )
            }             // for
        }
    }

    XMLStr dockOrderBooksu16 { "docks-order-book" };
    auto orderBookData = doc->getElementById(dockOrderBooksu16);

    if (orderBookData != nullptr)
    {

        auto parseOrderBookSide = [&](xercesc::DOMElement *node,
                                      QTableView *table,
                                      QLabel *symbolTitle,
                                      QLabel *latencyTitle,
                                      QFont &headerFont,
                                      QFont &tableFont,
                                      uint64_t &lowMin, uint64_t &lowMax,
                                      uint64_t &medMin, uint64_t &medMax,
                                      uint64_t &highMin, uint64_t &highMax,
                                      QColor &lowColor, QColor &medColor, QColor &highColor) {
            auto sideNodes = node->getChildNodes();
            for (auto idx = 0ull; idx < sideNodes->getLength(); ++idx)
            {
                if (sideNodes->item(idx)->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
                {
                    auto sideNode = dynamic_cast<xercesc::DOMElement *>(sideNodes->item(idx));
                    if (sideNode->getNodeName() == headerStr)
                    {
                        QString qssHeader;
                        parseHeader(sideNode, qssHeader, headerFont);
                        if (!qssHeader.isEmpty())
                            table->horizontalHeader()->setStyleSheet(qssHeader);
                    }
                    else if (sideNode->getNodeName() == tableStr)
                    {
                        QString qssTable;
                        parseSimpleTable(sideNode, qssTable, tableFont);
                        if (!qssTable.isEmpty())
                            table->setStyleSheet(qssTable);
                    }
                    else if (sideNode->getNodeName() == titleStr)
                    {
                        parseLabel(sideNode, symbolTitle);
                    }
                    else if (sideNode->getNodeName() == latFontStr)
                    {
                        parseLabel(sideNode, latencyTitle);
                    }
                    else if (sideNode->getNodeName() == latencyStr)
                    {
                        parseLatency(sideNode, lowMin, lowMax, medMin, medMax, highMin, highMax, lowColor, medColor, highColor);
                    }
                }
            }
        };

        auto nodes = orderBookData->getChildNodes();
        for (auto idx = 0ull; idx < nodes->getLength(); ++idx)
        {
            auto node = nodes->item(idx);
            if (node->getNodeType() == xercesc::DOMNode::NodeType::ELEMENT_NODE)
            {
                auto elementNode = dynamic_cast<xercesc::DOMElement *>(node);
                if (elementNode->getNodeName() == asksStr)
                {
                    parseOrderBookSide(elementNode,
                        ui->asksTable,
                        ui->asksSymbol,
                        ui->asksLatency,
                        g_globals->fonts.orderBookDock.asksSide.headerFont,
                        g_globals->fonts.orderBookDock.asksSide.tableFont,
                        g_globals->params.orderBookParameters.asksSide.latencyLowMin,
                        g_globals->params.orderBookParameters.asksSide.latencyLowMax,
                        g_globals->params.orderBookParameters.asksSide.latencyMediumMin,
                        g_globals->params.orderBookParameters.asksSide.latencyMediumMax,
                        g_globals->params.orderBookParameters.asksSide.latencyHighMin,
                        g_globals->params.orderBookParameters.asksSide.latencyHighMax,
                        g_globals->colors.orderBookDockColors.asksSide.latencyLow,
                        g_globals->colors.orderBookDockColors.asksSide.latencyMedium,
                        g_globals->colors.orderBookDockColors.asksSide.latencyHigh);
                }
                if (elementNode->getNodeName() == bidsStr)
                {
                    parseOrderBookSide(elementNode,
                        ui->bidsTable,
                        ui->bidsSymbol,
                        ui->bidsLatency,
                        g_globals->fonts.orderBookDock.bidsSide.headerFont,
                        g_globals->fonts.orderBookDock.bidsSide.tableFont,
                        g_globals->params.orderBookParameters.bidsSide.latencyLowMin,
                        g_globals->params.orderBookParameters.bidsSide.latencyLowMax,
                        g_globals->params.orderBookParameters.bidsSide.latencyMediumMin,
                        g_globals->params.orderBookParameters.bidsSide.latencyMediumMax,
                        g_globals->params.orderBookParameters.bidsSide.latencyHighMin,
                        g_globals->params.orderBookParameters.bidsSide.latencyHighMax,
                        g_globals->colors.orderBookDockColors.bidsSide.latencyLow,
                        g_globals->colors.orderBookDockColors.bidsSide.latencyMedium,
                        g_globals->colors.orderBookDockColors.bidsSide.latencyHigh);
                }
            }
        } // for
    }     // if (orderBookData != nullptr)
}
