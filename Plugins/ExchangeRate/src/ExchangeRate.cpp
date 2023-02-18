/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 29/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "ExchangeRate.hpp"
#include "CentaurInterface.hpp"
#include "CentaurPlugin.hpp"
#include "Protocol.hpp"
#include <QApplication>
#include <QDate>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QTextStream>
#include <QTimer>

#ifdef __clang__
#pragma clang diagnostic push #pragma clang diagnostic ignored "-Wold-style-cast"
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

/// UUID v5 Hashed String: CentaurProject-ExchangeRate-0.2.0

namespace
{
    constexpr char g_ExchangeRateName[]          = "ExchangeRate";
    constexpr char g_ExchangeRateVersionString[] = "0.2.0";
    constexpr char g_uuidString[]                = "f77ecf55-8162-5570-a9dc-3a79c6757c72";

} // namespace

CENTAUR_PLUGIN_NAMESPACE::ValueThread::ValueThread(const QString &qte, const QString &bse, qreal qteQty, QObject *parent) noexcept :
    QThread(parent),
    quote { qte },
    base { bse },
    quoteQuantity { qteQty }
{
}

void CENTAUR_PLUGIN_NAMESPACE::ValueThread::run()
{
    QString parameters = QString("from=%1&to=%2&amount=%3").arg(quote, base).arg(quoteQuantity, 0, 'f');

    cpr::Url url { QString("https://api.exchangerate.host/convert?%1").arg(parameters).toStdString() };

    cpr::Session session;
    session.SetUrl(url);
    session.SetUserAgent("CPP_interface_api/curl-openssl/7.70.0/Richard");

    cpr::Response apiCall;
    apiCall = session.Get();

#ifndef NDEBUG
    qDebug() << "ValueThread::acquireFromInternet Elapsed time: " << apiCall.elapsed;
#endif // NDEBUG

    // CURL Checking
    if (apiCall.error.code != cpr::ErrorCode::OK)
    {
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEX. Data was not acquire\n%1")).arg(QString::fromStdString(apiCall.error.message)));
        return;
    }

    // HTTP Error checking
    if (apiCall.status_code != 200)
    {
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEX. Data was not acquire\n%1")).arg(QString::fromStdString(apiCall.reason)));
        return;
    }

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(apiCall.text.c_str());
    if (jsonDoc.HasParseError())
    {
        QString errorFormat = QString("%1. At offset: %2").arg(rapidjson::GetParseError_En(jsonDoc.GetParseError())).arg(jsonDoc.GetErrorOffset());
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEx. The data had errors\n%1")).arg(errorFormat));
        return;
    }

    auto itResult = jsonDoc.FindMember("result");
    if (itResult != jsonDoc.MemberEnd())
        emit newValue(itResult->value.GetDouble());
}

CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::ExchangeRatePlugin(QObject *parent) :
    QObject(parent),
    m_thisUUID { g_uuidString }
{
}

QObject *CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginObject() noexcept
{
    return qobject_cast<QObject *>(this);
}

QString CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginName() const noexcept
{
    return g_ExchangeRateName;
}

QString CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginVersionString() const noexcept
{
    return g_ExchangeRateVersionString;
}

void CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config) noexcept
{
    m_logger = logger;
    m_config = config;
    logTrace("ExchangeRatePlugin", "ExchangeRatePlugin::setPluginInterfaces");
}

CENTAUR_NAMESPACE::uuid CENTAUR_PLUGIN_NAMESPACE::ExchangeRatePlugin::getPluginUUID() const noexcept
{
    return m_thisUUID;
}

cen::plugin::IStatus::DisplayMode cen::plugin::ExchangeRatePlugin::initialize() noexcept
{
    std::ifstream input(m_config->getConfigurationFileName());
    rapidjson::IStreamWrapper isw(input);

    if (pluginSettings.ParseStream(isw).HasParseError())
    {
        QString str = QString(tr("%1 at %2")).arg(rapidjson::GetParseError_En(pluginSettings.GetParseError())).arg(pluginSettings.GetErrorOffset());
        logError("ExchangeRatePlugin", QString("Response contain JSON errors. %1").arg(str));
        return DisplayMode::OnlyIcon;
    }

    m_updateMilliseconds = [&]() -> int64_t {
        if (auto iter = pluginSettings.FindMember("reload-timer"); iter != pluginSettings.MemberEnd())
        {
            const auto time = iter->value.GetInt64();
            logTrace("ExchangeRatePlugin", QString("Timer will be set to: %1").arg(time));
            return time;
        }
        logWarn("ExchangeRatePlugin", "Timer will be set to default: 1 hour");
        return 3'600'000ll;
    }();

    auto *reloadDataTimer = new QTimer(this);
    connect(reloadDataTimer, &QTimer::timeout, this, [&]() {
        onReloadData(true);
    });
    reloadDataTimer->start(static_cast<int>(m_updateMilliseconds));
    m_configurationLoaded = true;
    loadData();

    return DisplayMode::TextIcon;
}

QString cen::plugin::ExchangeRatePlugin::text() noexcept
{
    return QString("%1/%2: $ %3").arg(m_defaultBase, m_defaultQuote, QLocale(QLocale::English).toString(m_defaultValue, 'f', 5));
}

QPixmap cen::plugin::ExchangeRatePlugin::image() noexcept
{
    if (!m_configurationLoaded)
        return QPixmap(":/icon/red");
    else
        return QPixmap(":/icon/blue");
}

QFont cen::plugin::ExchangeRatePlugin::font() noexcept
{
    // return the default font
    return QApplication::font();
}

QBrush cen::plugin::ExchangeRatePlugin::brush(cen::plugin::IStatus::DisplayRole role) noexcept
{
    switch (role)
    {
        case DisplayRole::Icon: C_FALLTHROUGH;
        case DisplayRole::Text: C_FALLTHROUGH;
        case DisplayRole::Font: C_FALLTHROUGH;
        case DisplayRole::Background:
            return Qt::NoBrush;
        case DisplayRole::Foreground:
            return { QColor(158, 231, 255) };
    }
}

QAction *cen::plugin::ExchangeRatePlugin::action() noexcept
{
    return nullptr;
}

QList<QString> cen::plugin::ExchangeRatePlugin::listSupported() noexcept
{
    return QList<QString>();
}

qreal cen::plugin::ExchangeRatePlugin::value(const QString &quote, const QString &base) noexcept
{
    return convert(quote, base, 1.0, nullptr);
}

qreal cen::plugin::ExchangeRatePlugin::convert(const QString &quote, const QString &base, qreal quoteQuantity) noexcept
{
    return convert(quote, base, quoteQuantity, nullptr);
}

qreal cen::plugin::ExchangeRatePlugin::convert(const QString &quote, const QString &base, qreal quoteQuantity, QDate *date) noexcept
{
    QString parameters = QString("from=%1&to=%2&amount=%3").arg(quote, base).arg(quoteQuantity, 0, 'f');
    QString dateString;
    if (date != nullptr)
    {
        dateString = date->toString("yyyy-MM-dd");
        parameters += QString("&date=%1").arg(dateString);
    }

    cpr::Url url { QString("https://api.exchangerate.host/convert?%1").arg(parameters).toStdString() };

    cpr::Session session;
    session.SetUrl(url);
    session.SetUserAgent("CPP_interface_api/curl-openssl/7.70.0/Richard");

    cpr::Response apiCall;
    apiCall = session.Get();

#ifndef NDEBUG
    qDebug() << "CurrencySettingsWidget::acquireFromInternet Elapsed time: " << apiCall.elapsed;
#endif // NDEBUG

    // CURL Checking
    if (apiCall.error.code != cpr::ErrorCode::OK)
    {
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEX. Data was not acquire\n%1")).arg(QString::fromStdString(apiCall.error.message)));
        return -1;
    }

    // HTTP Error checking
    if (apiCall.status_code != 200)
    {
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEX. Data was not acquire\n%1")).arg(QString::fromStdString(apiCall.reason)));
        return -1;
    }

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(apiCall.text.c_str());
    if (jsonDoc.HasParseError())
    {
        QString errorFormat = QString("%1. At offset: %2").arg(rapidjson::GetParseError_En(jsonDoc.GetParseError())).arg(jsonDoc.GetErrorOffset());
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEx. The data had errors\n%1")).arg(errorFormat));
        return -1;
    }

    if (date != nullptr)
    {
        auto itHistorical = jsonDoc.FindMember("historical");
        if (itHistorical != jsonDoc.MemberEnd() && itHistorical->value.GetBool())
        {
            auto itHistoricalDate = jsonDoc.FindMember("date");
            if (itHistoricalDate == jsonDoc.MemberEnd())
                return std::numeric_limits<qreal>::infinity();
            if (QString(itHistoricalDate->value.GetString()) != dateString)
                return std::numeric_limits<qreal>::infinity();
        }
        else
            return std::numeric_limits<qreal>::infinity();
    }

    auto itResult = jsonDoc.FindMember("result");
    if (itResult != jsonDoc.MemberEnd())
        return itResult->value.GetDouble();
    return -1;
}

void cen::plugin::ExchangeRatePlugin::acquireFromInternet() noexcept
{
    logDebug("ExchangeRatePlugin", "ExchangeRatePlugin::acquireFromInternet()");

    cpr::Url url { "https://api.exchangerate.host/symbols" };

    cpr::Session session;
    session.SetUrl(url);
    session.SetUserAgent("CPP_interface_api/curl-openssl/7.70.0/RichardCentaur");

    cpr::Response apiCall;
    apiCall = session.Get();

    logDebug("ExchangeRatePlugin", QString("CurrencySettingsWidget::acquireFromInternet Elapsed time: %1").arg(apiCall.elapsed));

    // CURL Checking
    if (apiCall.error.code != cpr::ErrorCode::OK)
    {
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEX. Data was not acquire\n%1")).arg(QString::fromStdString(apiCall.error.message)));
        return;
    }

    // HTTP Error checking
    if (apiCall.status_code != 200)
    {
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEX. Data was not acquire\n%1")).arg(QString::fromStdString(apiCall.reason)));
        return;
    }

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(apiCall.text.c_str());
    if (jsonDoc.HasParseError())
    {
        QString errorFormat = QString("%1. At offset: %2").arg(rapidjson::GetParseError_En(jsonDoc.GetParseError())).arg(jsonDoc.GetErrorOffset());
        QMessageBox::critical(nullptr, tr("Failed to acquire data"), QString(tr("CurEX. The data had errors\n%1")).arg(errorFormat));
        return;
    }

    auto cacheMember = pluginSettings.FindMember("cache");
    if (cacheMember != pluginSettings.MemberEnd())
        pluginSettings.RemoveMember(cacheMember);

    rapidjson::Value cache(rapidjson::kArrayType);
    rapidjson::Document::AllocatorType &allocator = pluginSettings.GetAllocator();

    if (jsonDoc.HasMember("success") && jsonDoc["success"].GetBool())
    {
        auto symbolsIter = jsonDoc.FindMember("symbols");
        if (symbolsIter != jsonDoc.MemberEnd())
        {
            for (const auto &symbol : symbolsIter->value.GetObject())
            {
                QString description;
                QString name { symbol.name.GetString() };
                auto descIter = symbol.value.FindMember("description");
                if (descIter != symbol.value.MemberEnd())
                {
                    description = descIter->value.GetString();
                }
                m_currencyData[name] = description;

                rapidjson::Value jsonName;
                rapidjson::Value jsonDescription;
                jsonName.SetString(name.toStdString().c_str(), static_cast<rapidjson::SizeType>(name.toStdString().size()), allocator);
                jsonDescription.SetString(description.toStdString().c_str(), static_cast<rapidjson::SizeType>(description.toStdString().size()), allocator);

                rapidjson::Value cacheItem(rapidjson::kObjectType);
                cacheItem.AddMember("currency", jsonName, allocator);
                cacheItem.AddMember("description", jsonDescription, allocator);

                cache.PushBack(cacheItem, allocator);
            }
        }
    }

    pluginSettings.AddMember("cache", cache, allocator);

    rapidjson::Value jsonDate;
    auto dateString = QDate::currentDate().toString().toStdString();
    jsonDate.SetString(dateString.c_str(), static_cast<rapidjson::SizeType>(dateString.size()), allocator);
    pluginSettings["timestamp"] = jsonDate;
}

void cen::plugin::ExchangeRatePlugin::acquireFromCache() noexcept
{
    logDebug("ExchangeRatePlugin", "ExchangeRatePlugin::acquireFromCache()");

    for (auto &item : pluginSettings["cache"].GetArray())
    {
        QString name { item["currency"].GetString() };
        QString desc { item["description"].GetString() };
        m_currencyData[name] = desc;
    }
}

void cen::plugin::ExchangeRatePlugin::loadData() noexcept
{
    if (!m_configurationLoaded)
        return;

    auto tsIter = pluginSettings.FindMember("timestamp");
    if (tsIter == pluginSettings.MemberEnd())
        acquireFromInternet();

    auto timestampCache = QDate::fromString(QString { tsIter->value.GetString() });

    const auto currentDate = QDate::currentDate();

    if (timestampCache.day() != currentDate.day())
    {
        // We have a change of date
        acquireFromInternet();
    }
    else
    {
        // Same date
        acquireFromCache();
    }

    onReloadData(false);

    storeData();
}

void cen::plugin::ExchangeRatePlugin::storeData() noexcept
{
    rapidjson::StringBuffer jsonBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(jsonBuffer);
    pluginSettings.Accept(writer);

    QFile file(QString::fromStdString(m_config->getConfigurationFileName()));
    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream stream(&file);
        stream << jsonBuffer.GetString();
        file.close();
    }
}

void cen::plugin::ExchangeRatePlugin::onReloadData(bool threading) noexcept
{
    const auto reloadFromInternet = [&]() -> bool {
        const auto currentTime = QDateTime::currentMSecsSinceEpoch();
        if (auto iter = pluginSettings.FindMember("value-timestamp"); iter != pluginSettings.MemberEnd())
        {
            int64_t data { iter->value.GetInt64() };
            if (data)
            {
                const auto difference = currentTime - data;
                if (!(difference >= m_updateMilliseconds))
                    return false;
            }
            iter->value.SetInt64(currentTime);
        }
        else
        {
            pluginSettings.AddMember("value-timestamp", currentTime, pluginSettings.GetAllocator());
        }

        return true;
    }();

    m_defaultQuote = pluginSettings["default"]["quote"].GetString();
    m_defaultBase  = pluginSettings["default"]["base"].GetString();

    m_defaultValue = [&]() -> qreal {
        auto iter = pluginSettings.FindMember("value-cache");

        if (reloadFromInternet || iter == pluginSettings.MemberEnd())
        {
            if (!threading)
            {
                auto defValue = value(m_defaultQuote, m_defaultBase);

                if (iter != pluginSettings.MemberEnd())
                {
                    iter->value.SetDouble(defValue);
                }
                else
                {
                    pluginSettings.AddMember("value-cache", defValue, pluginSettings.GetAllocator());
                }

                return defValue;
            }
            else
            {
                auto *valueThread = new ValueThread(m_defaultQuote, m_defaultBase, 1.0, this);
                connect(valueThread, &ValueThread::newValue, this, &ExchangeRatePlugin::onValueUpdate);
                connect(valueThread, &ValueThread::finished, valueThread, &QObject::deleteLater);
                valueThread->start();

                return -1;
            }
        }
        else
        {
            return iter->value.GetDouble();
        }
    }();

    storeData();

    for (auto &available : pluginSettings["available"].GetArray())
    {
        m_currencySupported.emplace_back(available["b"].GetString(), available["q"].GetString());
    }

    emit displayChange(plugin::IStatus::DisplayRole::Text);
}

void cen::plugin::ExchangeRatePlugin::onValueUpdate(qreal val) noexcept
{
    m_defaultValue = val;

    auto iter = pluginSettings.FindMember("value-cache");

    if (iter != pluginSettings.MemberEnd())
    {
        iter->value.SetDouble(val);
    }
    else
    {
        pluginSettings.AddMember("value-cache", val, pluginSettings.GetAllocator());
    }

    storeData();

    emit displayChange(plugin::IStatus::DisplayRole::Text);
}
