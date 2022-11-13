/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 02/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_FAVORITESDIALOG_HPP
#define CENTAUR_FAVORITESDIALOG_HPP

#include "Centaur.hpp"
#include "../ui/ui_FavoritesDialog.h"
#include "CentaurGlobal.hpp"
#include "CentaurUIState.hpp"
#include <QDialog>

namespace CENTAUR_NAMESPACE
{
    class FavoritesDialog : public QDialog
    {
        Q_OBJECT
    public:
        explicit FavoritesDialog(QWidget *parent = nullptr) noexcept;
        ~FavoritesDialog() override;

    protected:
        void saveInterfaceState() noexcept;
        void loadInterfaceState() noexcept;

    public slots:
        void onDelete() noexcept;

    private:
        FavoritesDialogInterfaceState m_state;
        std::unique_ptr<Ui::FavoritesDialog> m_ui;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_FAVORITESDIALOG_HPP
