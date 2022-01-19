/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_EXCHANGERATE_HPP
#define CENTAUR_EXCHANGERATE_HPP

#include <CentaurInterface.hpp>
#include <CentaurPlugin.hpp>
#include <QLabel>

class ExchangeRatePlugin : public QObject,
                           public cent::plugin::IStatus
{

    Q_OBJECT

    Q_PLUGIN_METADATA(IID "com.centaur-project.plugin.ExchangeRatePlugin/1.0")
    Q_INTERFACES(cent::plugin::IBase cent::plugin::IStatus)

public:
    explicit ExchangeRatePlugin(QObject *parent = nullptr);
    ~ExchangeRatePlugin() override = default;

public:
    QObject *getPluginObject() noexcept override;
    QString getPluginName() noexcept override;
    QString getPluginVersionString() noexcept override;
    void setPluginInterfaces(cent::interface::ILogger *logger, cent::interface::IConfiguration *config) noexcept override;
    cent::plugin::PluginUUID getPluginUUID() noexcept override;
    QString menuSlotName(const QString &identifier) noexcept override;

public:
    void initialization(QStatusBar *bar) noexcept override;

protected:
    void updateExchangeRate() noexcept;

private:
    QLabel *m_exchangeRate;

private:
    cent::interface::ILogger *m_logger { nullptr };
    cent::interface::IConfiguration *m_config { nullptr };
};

// Helper macros
#define logInfo(x, y) \
    m_logger->info(x, y)

#define logWarn(x, y) \
    m_logger->warning(x, y)

#if !defined(NDEBUG)
#define logTrace(x, y) \
    m_logger->trace(x, y)
#define logDebug(x, y) \
    m_logger->debug(x, y)
#else
#define logTrace(x, y) \
    ((void *)0)
#define logDebug(x, y) \
    ((void *)0)
#endif /**/

#define logError(x, y) \
    m_logger->error(x, y)

#define logFatal(x, y) \
    m_logger->fatal(x, y)

#endif // CENTAUR_EXCHANGERATE_HPP
