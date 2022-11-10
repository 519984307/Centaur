////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "PasswordLineEdit.hpp"

BEGIN_CENTAUR_NAMESPACE

PasswordLineEdit::PasswordLineEdit(QWidget *parent) :
    LinkedLineEdit(parent),
    m_view { new QAction { this } }
{

    connect(this, &QLineEdit::textChanged, this, [&]() {
        if (text().isEmpty())
        {
            m_view->setIcon(QIcon { "" });
        }
        else
        {
            if (!m_visibility)
            {
                setEchoMode(QLineEdit::EchoMode::Password);
                m_view->setIcon(QIcon { ":/password/hide" });
            }
            else
            {
                setEchoMode(QLineEdit::EchoMode::Normal);
                m_view->setIcon(QIcon { ":/password/show" });
            }
        }
    });

    connect(m_view, &QAction::triggered, this, [&]([[maybe_unused]] bool trigger) {
        if (m_visibility)
        {
            setEchoMode(QLineEdit::EchoMode::Password);
            m_view->setIcon(QIcon { ":/password/hide" });
        }
        else
        {
            setEchoMode(QLineEdit::EchoMode::Normal);
            m_view->setIcon(QIcon { ":/password/show" });
        }

        m_visibility = !m_visibility;
    });

    addAction(QIcon(":/password/edit"), QLineEdit::LeadingPosition);
    addAction(m_view, QLineEdit::TrailingPosition);
}

END_CENTAUR_NAMESPACE
