////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "UserLineEdit.hpp"

BEGIN_CENTAUR_NAMESPACE

UserLineEdit::UserLineEdit(QWidget *parent) :
    LinkedLineEdit(parent)
{
    addAction(QIcon(":/user/edit"), QLineEdit::LeadingPosition);
}

END_CENTAUR_NAMESPACE
