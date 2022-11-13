/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_NETWORKADDRESSDIALOG_HPP
#define CENTAUR_NETWORKADDRESSDIALOG_HPP

#include "Centaur.hpp"
#include "../ui/ui_NetworkAddress.h"
#include "BinanceAPI.hpp"
#include "CentaurInterface.hpp"
#include "OptionsTableWidget.hpp"
#include <QDialog>

namespace CENTAUR_NAMESPACE
{
    class NetworkAddressDialog : public QDialog
    {
    public:
        NetworkAddressDialog(BINAPI_NAMESPACE::BinanceAPISpot *api, BINAPI_NAMESPACE::AllCoinsInformation *data, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, QString coin, QWidget *parent = nullptr);
        ~NetworkAddressDialog() override;

    protected:
        void saveInterfaceState() noexcept;
        void restoreInterfaceState() noexcept;

    protected slots:
        void onNetworkChanged(int index) noexcept;

    private:
        QString m_coin;
        BINAPI_NAMESPACE::BinanceAPISpot *m_api;
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config;
        std::unique_ptr<Ui::NetworkAddress> m_ui;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_NETWORKADDRESSDIALOG_HPP
