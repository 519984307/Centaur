////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PASWORDLINEEDIT_HPP
#define CENTAUR_PASWORDLINEEDIT_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"
#include "LinkedLineEdit.hpp"
#include <QAction>
#include <QLineEdit>

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY PasswordLineEdit : public LinkedLineEdit
{
    Q_OBJECT
public:
    explicit PasswordLineEdit(QWidget *parent = nullptr);
    ~PasswordLineEdit() override = default;

private:
    QAction *m_view;
    bool m_visibility { false };
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_PASWORDLINEEDIT_HPP
