/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_STATUSDIALOG_HPP
#define CENTAUR_STATUSDIALOG_HPP

#include "BinanceAPI.hpp"
#include "../../Centaur.hpp"
#include "../ui/ui_StatusDialog.h"

namespace CENTAUR_NAMESPACE
{
    class StatusDialog : public QDialog
    {
        Q_OBJECT
    public:
        StatusDialog( BINAPI_NAMESPACE::SPOT::APIKeyPermissions *apiKeyPermissions, BINAPI_NAMESPACE::SPOT::AccountAPITradingStatus *apiTradingStatus, const QString &status, QWidget *parent);
        ~StatusDialog() override;

    protected:
        void saveInterfaceState() noexcept;
        void restoreInterfaceState() noexcept;

    private:
        std::unique_ptr<Ui::StatusDialog> m_ui;
    };
}

#endif // CENTAUR_STATUSDIALOG_HPP
