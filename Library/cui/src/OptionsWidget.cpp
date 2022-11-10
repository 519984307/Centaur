////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "OptionsWidget.hpp"
#include "../ui/ui_OptionsWidget.h"

BEGIN_CENTAUR_NAMESPACE

struct OptionsWidget::Impl
{
    Impl() :
        ui { new Ui::OptionsWidget } { }
    ~Impl()= default;
    std::unique_ptr<Ui::OptionsWidget> ui;
};


OptionsWidget::OptionsWidget(int buttons, QWidget *parent) :
    QWidget { parent },
    _impl { new Impl }
{
    _impl->ui->setupUi(this);

    if (!(buttons & Buttons::view))
        _impl->ui->viewBtn->hide();
    if (!(buttons & Buttons::edit))
        _impl->ui->editBtn->hide();
    if (!(buttons & Buttons::del))
        _impl->ui->delBtn->hide();
}

OptionsWidget::~OptionsWidget() = default;

QPushButton *OptionsWidget::editButton() noexcept
{
    return _impl->ui->editBtn;
}

QPushButton *OptionsWidget::deleteButton() noexcept
{
    return _impl->ui->delBtn;
}

QPushButton *OptionsWidget::viewButton() noexcept
{
    return _impl->ui->viewBtn;
}

END_CENTAUR_NAMESPACE
