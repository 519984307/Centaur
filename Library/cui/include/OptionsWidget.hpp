////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_OPTIONSWIDGET_HPP
#define CENTAUR_OPTIONSWIDGET_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"
#include <QPushButton>

BEGIN_CENTAUR_NAMESPACE
class CENTFIN_LIBRARY OptionsWidget : public QWidget
{
    Q_OBJECT
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
    ~OptionsWidget() override;

public:
    QPushButton *editButton() noexcept;
    QPushButton *deleteButton() noexcept;
    QPushButton *viewButton() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_OPTIONSWIDGET_HPP
