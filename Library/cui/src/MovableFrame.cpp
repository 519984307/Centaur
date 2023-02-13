////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "MovableFrame.hpp"
#include <QMouseEvent>

BEGIN_CENTAUR_NAMESPACE

MovableFrame::MovableFrame(QWidget *parent) :
    QFrame { parent }
{
    setMouseTracking(true);
    m_topLevelWidget = parent;
    while (m_topLevelWidget->parentWidget() != nullptr)
        m_topLevelWidget = m_topLevelWidget->parentWidget();
}

void MovableFrame::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        m_startPoint = event->globalPosition().toPoint();

    QFrame::mousePressEvent(event);
}

void MovableFrame::mouseMoveEvent(QMouseEvent *event)
{
     if (event->buttons() & Qt::LeftButton)
     {
         if ((event->globalPosition().toPoint() - m_startPoint) != QPoint { 0, 0 })
         {
             QRect thisRect = m_topLevelWidget->geometry();
             thisRect.moveTopLeft(thisRect.topLeft() + (event->globalPosition().toPoint() - m_startPoint));
             m_topLevelWidget->setGeometry(thisRect);
         }

         m_startPoint = event->globalPosition().toPoint();
     }

    QFrame::mouseMoveEvent(event);
}

void MovableFrame::mouseDoubleClickEvent(C_UNUSED QMouseEvent *event)
{
    if (m_topLevelWidget->isMaximized())
    {
        m_topLevelWidget->showNormal();
        if (!m_normalGeometry.isEmpty())
            m_topLevelWidget->setGeometry(m_normalGeometry);
    }
    else
    {
        m_normalGeometry = m_topLevelWidget->geometry();
        m_topLevelWidget->showMaximized();
    }
}

void MovableFrame::overrideParent(QWidget *newParent) noexcept
{
    m_topLevelWidget = newParent;
}

END_CENTAUR_NAMESPACE
