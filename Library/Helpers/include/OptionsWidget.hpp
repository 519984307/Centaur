/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_OPTIONSWIDGET_HPP
#define CENTAUR_OPTIONSWIDGET_HPP

#include "../../Centaur.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>

namespace CENTAUR_NAMESPACE
{
    class OptionsWidget : public QWidget
    {
        Q_OBJECT

        struct Ui
        {
            QHBoxLayout *horizontalLayout { nullptr };
            QSpacerItem *horizontalSpacerLeft { nullptr };
            QPushButton *viewBtn { nullptr };
            QPushButton *editBtn { nullptr };
            QPushButton *delBtn { nullptr };
            QSpacerItem *horizontalSpacerRight { nullptr };
        } m_ui;

    public:
        enum Buttons : int
        {
            none = 0,
            view = 0x00000001,
            edit = 0x00000002,
            del  = 0x00000004
        };

    public:
        OptionsWidget(int buttons, QWidget *parent = nullptr);
        ~OptionsWidget() override = default;

    public:
        C_NODISCARD QPushButton *editButton() const noexcept;
        C_NODISCARD QPushButton *deleteButton() const noexcept;
        C_NODISCARD QPushButton *viewButton() const noexcept;

    private:
        void initWidget();
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_OPTIONSWIDGET_HPP
