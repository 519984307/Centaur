////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "SearchLineEdit.hpp"

BEGIN_CENTAUR_NAMESPACE
SearchLineEdit::SearchLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    addAction(QIcon(":/search/glass"), ActionPosition::LeadingPosition);
}

void SearchLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    emit searchFocus();
}

void SearchLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit searchLostFocus();
}

END_CENTAUR_NAMESPACE
