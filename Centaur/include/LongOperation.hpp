/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 01/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_LONGOPERATION_HPP
#define CENTAUR_LONGOPERATION_HPP

#include "Centaur.hpp"
#include "../ui/ui_LenghtyDialog.h"
#include "CentaurGlobal.hpp"
#include "CentaurInterface.hpp"
#include <QDialog>

namespace CENTAUR_NAMESPACE
{
    class LongOperationDialog;
    class LongOperation : public CENTAUR_INTERFACE_NAMESPACE::ILongOperation
    {
    public:
        LongOperation();
        ~LongOperation() override;

    public:
        void show(const QString &message, const QIcon &icon, bool modal) override;

        /// \brief Hide the dialog
        void hide() override;

    private:
        LongOperationDialog *m_operation;
    };

    class LongOperationDialog : public QDialog
    {
        Q_OBJECT
    public:
        explicit LongOperationDialog(QWidget *parent = nullptr);
        ~LongOperationDialog() override;

    public:
        void setParameters(const QString &message, const QIcon &icon);

    private:
        std::unique_ptr<Ui::LongOperation> m_ui;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_LONGOPERATION_HPP
