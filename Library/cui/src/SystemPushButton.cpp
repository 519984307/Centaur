////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "SystemPushButton.hpp"

#include <QApplication>

BEGIN_CENTAUR_NAMESPACE

SystemPushButton::SystemPushButton(QWidget *parent) :
    QPushButton(parent)
{
    m_topLevelWidget = parent;
    while (m_topLevelWidget->parentWidget() != nullptr)
        m_topLevelWidget = m_topLevelWidget->parentWidget();

    connect(this, &QPushButton::released, this, &SystemPushButton::activate);
}

void SystemPushButton::setButtonClass(CENTAUR_NAMESPACE::SystemPushButton::ButtonClass btnClass) noexcept
{
    m_class = btnClass;
}

#ifdef Q_OS_MAC
void SystemPushButton::linkClose(SystemPushButton *min, SystemPushButton *fullscreen)
{
    m_linkMin     = min;
    m_linkMaxFull = fullscreen;
}

void SystemPushButton::linkMinimize(SystemPushButton *close, SystemPushButton *fullscreen)
{
    m_linkClose   = close;
    m_linkMaxFull = fullscreen;
}

void SystemPushButton::linkFullScreen(SystemPushButton *close, SystemPushButton *min)
{
    m_linkClose = close;
    m_linkMin   = min;
}

#else
void SystemPushButton::linkClose(SystemPushButton *min, SystemPushButton *maximize)
{
    m_linkMin     = min;
    m_linkMaxFull = maximize
}
void SystemPushButton::linkMinimize(SystemPushButton *close, SystemPushButton *maximize)
{
    m_linkClose   = close;
    m_linkMaxFull = maximize;
}
void SystemPushButton::linkMaximize(SystemPushButton *close, SystemPushButton *min)
{
    m_linkClose = close;
    m_linkMin   = min;
}
#endif

void SystemPushButton::activate() noexcept
{
    switch (m_class)
    {
        case ButtonClass::close:
            m_topLevelWidget->close();
            break;
        case ButtonClass::minimize:
            m_topLevelWidget->showMinimized();
            break;
#ifdef Q_OS_MAC
        case ButtonClass::fullscreen:
            if (m_topLevelWidget->isFullScreen())
            {
                m_topLevelWidget->showNormal();
                if (m_linkClose)
                    m_linkClose->show();
                if (m_linkMin)
                    m_linkMin->show();
            }
            else
            {
                m_topLevelWidget->showFullScreen();
                if (m_linkClose)
                    m_linkClose->hide();
                if (m_linkMin)
                    m_linkMin->hide();
            }
#else
        case ButtonClass::maximize:
            if (!m_topLevelWidget->isMaximized())
                m_topLevelWidget->showNormal();
            else
                m_topLevelWidget->showMaximized();
#endif
            break;
        case ButtonClass::override:
            emit systemPressed();
            break;
    }
}

END_CENTAUR_NAMESPACE
