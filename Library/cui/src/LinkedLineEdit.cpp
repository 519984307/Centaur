////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//


#include "LinkedLineEdit.hpp"

BEGIN_CENTAUR_NAMESPACE

LinkedLineEdit::LinkedLineEdit(QWidget *parent) :
    QLineEdit(parent)
{

}

void LinkedLineEdit::linkLabel(QLabel *label)
{
    m_linkedLabel    = label;
    m_defaultPalette = m_linkedLabel->palette();
    m_defaultFont = m_linkedLabel->font();
}

void LinkedLineEdit::focusInEvent(QFocusEvent *event)
{
    if (m_linkedLabel)
    {
        QPalette pal { m_defaultPalette };
        pal.setColor(QPalette::WindowText, QColor(33,150,243));
        m_linkedLabel->setPalette(pal);

        QFont font{m_defaultFont};
        font.setWeight(QFont::Weight::DemiBold);
        m_linkedLabel->setFont(font);
    }
    QLineEdit::focusInEvent(event);
}

void LinkedLineEdit::focusOutEvent(QFocusEvent *event)
{
    if (m_linkedLabel)
    {
        m_linkedLabel->setFont(m_defaultFont);
        m_linkedLabel->setPalette(m_defaultPalette);
    }
    QLineEdit::focusOutEvent(event);
}

void LinkedLineEdit::paintEvent(QPaintEvent *event)
{

    QLineEdit::paintEvent(event);
}

END_CENTAUR_NAMESPACE
