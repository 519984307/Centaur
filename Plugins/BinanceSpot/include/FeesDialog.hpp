/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_FEESDIALOG_HPP
#define CENTAUR_FEESDIALOG_HPP

#include "Centaur.hpp"
#include "../ui/ui_FeesDialog.h"
#include "BinanceAPI.hpp"
#include "CentaurInterface.hpp"
#include "OptionsTableWidget.hpp"
#include <QDialog>

namespace CENTAUR_NAMESPACE
{
    class TradeFeesDialog : public QDialog
    {
        Q_OBJECT
    public:
        TradeFeesDialog(const BINAPI_NAMESPACE::SpotTradingFees &fees, const QString &symbolFilter, QDialog *parent = nullptr);
        ~TradeFeesDialog();

    protected:
        void saveInterfaceState() noexcept;
        void restoreInterfaceState() noexcept;

    private:
        QStandardItemModel *m_searchModel { nullptr };

    private:
        std::unique_ptr<Ui::FeesDialog> m_ui;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_FEESDIALOG_HPP
