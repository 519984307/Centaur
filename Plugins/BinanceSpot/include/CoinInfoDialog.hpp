/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_COININFODIALOG_HPP
#define CENTAUR_COININFODIALOG_HPP

#include "../ui/ui_CoinInfoDialog.h"
#include "../ui/ui_NetworksDialog.h"
#include "BinanceAPI.hpp"
#include "CentaurInterface.hpp"
#include "OptionsTableWidget.hpp"
#include <QDialog>

namespace CENTAUR_NAMESPACE
{
    class CoinInfoDialog : public QDialog
    {
        Q_OBJECT
    public:
        CoinInfoDialog(BINAPI_NAMESPACE::AllCoinsInformation *data, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, const QString &filter = "", QWidget *parent = nullptr);
        ~CoinInfoDialog() override = default;

    protected slots:
        void onViewItem(QStandardItem *item) noexcept;

    private:
        QAction *m_viewAction { nullptr };
        std::unique_ptr<Ui::CoinInfoDialog> m_ui;
        BINAPI_NAMESPACE::AllCoinsInformation *m_info;
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config;
    };

    class NetworkListDialog : public QDialog
    {
        Q_OBJECT
    public:
        NetworkListDialog(const QString &name, const QString &coin, std::vector<BINAPI_NAMESPACE::SPOT::CoinInformation::NetworkList> *data, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, QWidget *parent = nullptr);
        ~NetworkListDialog() override = default;

    protected slots:
        void onCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) noexcept;

    private:
        std::unique_ptr<Ui::NetworkListDialog> m_ui;
        std::vector<BINAPI_NAMESPACE::SPOT::CoinInformation::NetworkList> *m_list;
        CENTAUR_INTERFACE_NAMESPACE::IConfiguration *m_config;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_COININFODIALOG_HPP
