////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_USERLINEEDIT_HPP
#define CENTAUR_USERLINEEDIT_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"
#include "LinkedLineEdit.hpp"

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY UserLineEdit : public LinkedLineEdit
{
    Q_OBJECT
public:
    explicit UserLineEdit(QWidget *parent = nullptr);
    ~UserLineEdit() override = default;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_USERLINEEDIT_HPP
