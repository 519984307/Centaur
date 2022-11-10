////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_SEARCHLINEEDIT_HPP
#define CENTAUR_SEARCHLINEEDIT_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"
#include <QLineEdit>

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    ~SearchLineEdit() override = default;

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

signals:
    void searchFocus();
    void searchLostFocus();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_SEARCHLINEEDIT_HPP
