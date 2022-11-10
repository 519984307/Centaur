////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_LINKEDLINEEDIT_HPP
#define CENTAUR_LINKEDLINEEDIT_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"

#include <QLabel>
#include <QLineEdit>

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY LinkedLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LinkedLineEdit(QWidget *parent = nullptr);
    ~LinkedLineEdit() override = default;

public:
    void linkLabel(QLabel *label);

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QFont m_defaultFont;
    QPalette m_defaultPalette;
    QLabel *m_linkedLabel { nullptr };
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_LINKEDLINEEDIT_HPP
