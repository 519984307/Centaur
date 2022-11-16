/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_BINANCESTATUS_HPP
#define CENTAUR_BINANCESTATUS_HPP

#include "BinanceAPI.hpp"
#include <CentaurInterface.hpp>
#include <CentaurPlugin.hpp>
#include <QLabel>
#include <QMouseEvent>

namespace CENTAUR_PLUGIN_NAMESPACE
{

    class ClickableLabel : public QLabel
    {
        Q_OBJECT
    public:
        explicit ClickableLabel(QWidget *parent);
        ~ClickableLabel() override;

    public:
        void mouseReleaseEvent(QMouseEvent *ev) override;

    signals:
        void snClick();
    };

    class BinanceSPOTStatus : public QObject,
                              public CENTAUR_PLUGIN_NAMESPACE::IStatus
    {

        Q_OBJECT

        Q_PLUGIN_METADATA(IID "com.centaur-project.plugin.BinanceSPOTStatus/1.0")
        Q_INTERFACES(CENTAUR_PLUGIN_NAMESPACE::IBase CENTAUR_PLUGIN_NAMESPACE::IStatus)

    public:
        explicit BinanceSPOTStatus(QObject *parent = nullptr);
        ~BinanceSPOTStatus() override = default;

    public:
        QObject *getPluginObject() noexcept override;
        QString getPluginName() noexcept override;
        QString getPluginVersionString() noexcept override;
        void setPluginInterfaces(CENTAUR_INTERFACE_NAMESPACE::ILogger *logger, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config) noexcept override;
        uuid getPluginUUID() noexcept override;

    public:
        DisplayMode initialize() noexcept override;
        QAction *action(const QPoint &pt) noexcept override;
        QString text() noexcept override;
        QPixmap image() noexcept override;
        QFont font() noexcept override;
        QBrush brush(DisplayRole role) noexcept override;

    signals:
        void displayChange(plugin::IStatus::DisplayRole role);

    protected:
        void onCheckAccountStatus() noexcept;
        void onViewData() noexcept;

    private:
        QPixmap m_image;
        ClickableLabel *m_status { nullptr };

    private:
        std::unique_ptr<BINAPI_NAMESPACE::BinanceAPISpot> m_api { nullptr };
        BINAPI_NAMESPACE::SPOT::ExchangeInformation m_exchInfo;
        BINAPI_NAMESPACE::BinanceLimits m_limits;
        BINAPI_NAMESPACE::BinanceKeys m_keys;

    private:
        CENTAUR_INTERFACE_NAMESPACE::ILogger *m_logger { nullptr };
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config { nullptr };
        uuid m_thisUUID;

    private:
        QString m_accountStatus;
        BINAPI_NAMESPACE::SPOT::APIKeyPermissions m_apiPermissions;
        BINAPI_NAMESPACE::SPOT::AccountAPITradingStatus m_apiTradingStatus;
    };
} // namespace CENTAUR_PLUGIN_NAMESPACE

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

#endif // CENTAUR_BINANCESTATUS_HPP
