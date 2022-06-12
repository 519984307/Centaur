/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 29/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "ExchangeRate.hpp"
#include "CentaurInterface.hpp"
#include "CentaurPlugin.hpp"
#include "protocol.hpp"

#include <QObject>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wdeprecated-volatile"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wdocumentation-deprecated-sync"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-dtor"
#pragma clang diagnostic ignored "-Wshadow-uncaptured-local"
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
#pragma clang diagnostic ignored "-Wsuggest-override"
#endif /*__clang__*/

// C++ CURL Wrapper C++Requests (CPR)
#include <cpr/cpr.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wambiguous-reversed-operator"
#pragma clang diagnostic ignored "-Wsuggest-override"
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif /*__clang__*/

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/schema.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

/// UUID v5 Hashed String: CentaurProject-ExchangeRate-0.2.0

namespace
{
    constexpr char g_ExchangeRateName[]          = "ExchangeRate";
    constexpr char g_ExchangeRateVersionString[] = "0.2.0";
    constexpr char g_uuidString[]                = "{f77ecf55-8162-5570-a9dc-3a79c6757c72}";

    constexpr char g_schema[]                    = R"({
"$schema": "http://json-schema.org/draft-07/schema",
"$id": "http://richardqpc.com/binapi/currency_exchange.schema.json",
"type": "object",
"title": "The root schema",
"required": [
  "Realtime Currency Exchange Rate"
],
"properties": {
  "Realtime Currency Exchange Rate": {
    "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate",
    "type": "object",
    "title": "The Realtime Currency Exchange Rate schema",
    "required": [
      "5. Exchange Rate"
    ],
    "properties": {
      "1. From_Currency Code": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/1.%20From_Currency%20Code",
        "type": "string",
        "title": "The 1. From_Currency Code schema"
      },
      "2. From_Currency Name": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/2.%20From_Currency%20Name",
        "type": "string",
        "title": "The 2. From_Currency Name schema"
      },
      "3. To_Currency Code": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/3.%20To_Currency%20Code",
        "type": "string",
        "title": "The 3. To_Currency Code schema"
      },
      "4. To_Currency Name": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/4.%20To_Currency%20Name",
        "type": "string",
        "title": "The 4. To_Currency Name schema"
      },
      "5. Exchange Rate": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/5.%20Exchange%20Rate",
        "type": "string",
        "title": "The 5. Exchange Rate schema"
      },
      "6. Last Refreshed": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/6.%20Last%20Refreshed",
        "type": "string",
        "title": "The 6. Last Refreshed schema"
      },
      "7. Time Zone": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/7.%20Time%20Zone",
        "type": "string",
        "title": "The 7. Time Zone schema"
      },
      "8. Bid Price": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/8.%20Bid%20Price",
        "type": "string",
        "title": "The 8. Bid Price schema"
      },
      "9. Ask Price": {
        "$id": "#/properties/Realtime%20Currency%20Exchange%20Rate/properties/9.%20Ask%20Price",
        "type": "string",
        "title": "The 9. Ask Price schema"
      }
    },
    "additionalProperties": true
  }
},
"additionalProperties": true
})";
} // namespace

CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::ExchangeRatePlugin(QObject *parent) :
    QObject(parent),
    m_thisUUID { g_uuidString }
{
}

QObject *CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginObject() noexcept
{
    return qobject_cast<QObject *>(this);
}
QString CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginName() noexcept
{
    return g_ExchangeRateName;
}
QString CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginVersionString() noexcept
{
    return g_ExchangeRateVersionString;
}
void CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, [[maybe_unused]] CENTAUR_INTERFACE_NAMESPACE::ILongOperation *lOper) noexcept
{
    m_logger = logger;
    m_config = config;
    logTrace("ExchangeRatePlugin", "ExchangeRatePlugin::setPluginInterfaces");
}
CENTAUR_NAMESPACE::uuid CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginUUID() noexcept
{
    return m_thisUUID;
}

void CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::initialization(QStatusBar *bar) noexcept
{
    logTrace("ExchangeRatePlugin", "ExchangeRatePlugin::initialization");

    m_exchangeRate = new QLabel("                ", bar);
    m_exchangeRate->setStyleSheet(R"(QLabel{color: rgb(158, 231, 255); min-width: 85px;})");
    bar->addPermanentWidget(m_exchangeRate, 0);

    updateExchangeRate();
}

bool CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::addMenuAction(C_UNUSED QAction *action, C_UNUSED const uuid &menuId) noexcept
{
    return false;
}

void CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::updateExchangeRate() noexcept
{
    logTrace("ExchangeRatePlugin", "ExchangeRatePlugin::updateExchangeRate");

    std::string base, quote, key;

    bool baseError, quoteError, keyError;
    base  = m_config->getValue("base", &baseError);
    quote = m_config->getValue("quote", &quoteError);
    key   = m_config->getValue("apiKey", &keyError);

    if (!baseError || !quoteError || !keyError)
    {
        logError("ExchangeRatePlugin", QString("Configuration data is incomplete."));
        return;
    }

    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;
    try
    {
        ec.loadPublicKey(m_config->getPluginPublicKeyPath());
    } catch (const std::exception &ex)
    {
        logError("ExchangeRatePlugin", QString { "RSA Key could not be opened. %1" }.arg(ex.what()));
        return;
    }

    auto decryptedKey = ec.decryptPublic(key, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64);

    // API_KEY: KJX64L888HD5U5T9
    if (decryptedKey.empty())
    {
        logError("ExchangeRatePlugin", "Error deciphering the https://www.alphavantage.co/query API Key ");
        return;
    }

    cpr::Response exchange = cpr::Get(cpr::Url { "https://www.alphavantage.co/query" },
        cpr::Parameters { { "function", "CURRENCY_EXCHANGE_RATE" },
            { "apikey", decryptedKey },
            { "from_currency", base },
            { "to_currency", quote } });

    if (exchange.error.code != cpr::ErrorCode::OK)
    {
        logError("ExchangeRatePlugin", exchange.error.message.c_str());
    }

    rapidjson::Document jsonDoc;

    if (jsonDoc.Parse(exchange.text.c_str()).HasParseError())
    {
        QString str = QString(tr("%1 at %2")).arg(rapidjson::GetParseError_En(jsonDoc.GetParseError())).arg(jsonDoc.GetErrorOffset());
        logError("ExchangeRatePlugin", QString("Response contain JSON errors. %1").arg(str));
        return;
    }

    rapidjson::Document schemaDoc;
    if (schemaDoc.Parse(g_schema).HasParseError())
    {
        QString str = QString(tr("%1 at %2")).arg(rapidjson::GetParseError_En(jsonDoc.GetParseError())).arg(jsonDoc.GetErrorOffset());
        logError("ExchangeRatePlugin", QString("The internal JSON schema is ill-formed. %1").arg(str));
        return;
    }

    rapidjson::SchemaDocument sd(schemaDoc);
    rapidjson::SchemaValidator sv { sd };

    if (!jsonDoc.Accept(sv))
    {
        rapidjson::StringBuffer sb;
        sv.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        QString str = QString("Invalid schema %1.\nInvalid keyword: %2\n").arg(sb.GetString(), sv.GetInvalidSchemaKeyword());
        sb.Clear();
        sv.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        str += QString("Invalid document: %1\n").arg(sb.GetString());
        logError("PluginsDialog", QString("File plid.json does not comply the schema. %1").arg(str));
        return;
    }

    auto exchRate = std::stod(jsonDoc["Realtime Currency Exchange Rate"]["5. Exchange Rate"].GetString());

    logInfo("ExchangeRatePlugin", QString("Exchange rate is -> %1/%2: %3").arg(base.c_str(), quote.c_str()).arg(exchRate, 8, 'f', 5));

    if (exchRate > 0)
    {
        m_exchangeRate->setText(QString("%1/%2: $ %3  ").arg(base.c_str(), quote.c_str()).arg(exchRate, 8, 'f', 5));
    }
    else
    {
        m_exchangeRate->setText(QString("%1/%2: N/A").arg(base.c_str(), quote.c_str()));
    }
}

/*
QString ExchangeRatePlugin::menuSlotName([[maybe_unused]] const QString &identifier) noexcept
{
    return QString();
}
 */
