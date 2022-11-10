////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ResizableFrame.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>

BEGIN_CENTAUR_NAMESPACE

ResizableFrame::ResizableFrame(QWidget *parent) :
    QFrame(parent)
{
    setMouseTracking(true);


    m_topLevelWidget = parent;
    while (m_topLevelWidget->parentWidget() != nullptr)
        m_topLevelWidget = m_topLevelWidget->parentWidget();
}

void ResizableFrame::mousePressEvent(QMouseEvent *event)
{
    const QPoint localPoint = event->pos();

    for (int i = 0; i < 8; ++i)
    {
        if (m_activeFrames[i].contains(localPoint))
        {
            grabMouse();
            m_mouseAt = i;
            break;
        }
    }

    m_startPoint = event->globalPosition().toPoint();

    m_bottomDiff = m_topLevelWidget->geometry().bottom() - event->globalPosition().toPoint().y();
    m_rightDiff  = m_topLevelWidget->geometry().right() - event->globalPosition().toPoint().x();
    m_leftDiff   = event->globalPosition().toPoint().x() - m_topLevelWidget->geometry().left();
    m_topDiff    = event->globalPosition().toPoint().y() - m_topLevelWidget->geometry().top();

    QFrame::mousePressEvent(event);
}

void ResizableFrame::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint localPoint = event->pos();
    QRect geometry          = m_topLevelWidget->geometry();

    auto cursor             = Qt::ArrowCursor;
    int mouseAt             = m_mouseAt;
    if (mouseAt == -1)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (m_activeFrames[i].contains(localPoint))
            {
                mouseAt = i;
                break;
            }
        }
    }

    switch (mouseAt)
    {
        case HotSpots::topLeftCorner:
            C_FALLTHROUGH;
        case HotSpots::bottomRightCorner:
            cursor = Qt::SizeFDiagCursor;
            break;
        case HotSpots::topRightCorner:
            C_FALLTHROUGH;
        case HotSpots::bottomLeftCorner:
            cursor = Qt::SizeBDiagCursor;
            break;
        case HotSpots::topBorder:
            C_FALLTHROUGH;
        case HotSpots::bottomBorder:
            cursor = Qt::SizeVerCursor;
            break;
        case HotSpots::rightBorder:
            C_FALLTHROUGH;
        case HotSpots::leftBorder:
            cursor = Qt::SizeHorCursor;
            break;
        default:
            cursor = Qt::ArrowCursor;
    }

    setCursor(cursor);

    if (event->buttons() & Qt::LeftButton)
    {
        const QPoint globalPoint = event->globalPosition().toPoint();

        if (const QPoint diff = m_startPoint - globalPoint; diff != QPoint(0, 0))
        {
            switch (m_mouseAt)
            {
                case HotSpots::topLeftCorner:
                    geometry.setTopLeft(globalPoint - QPoint { m_leftDiff, m_topDiff });
                    break;
                case HotSpots::topRightCorner:
                    geometry.setTopRight(globalPoint + QPoint { m_rightDiff, -m_topDiff });

                    break;
                case HotSpots::bottomLeftCorner:
                    geometry.setBottomLeft(globalPoint + QPoint { -m_leftDiff, m_bottomDiff });

                    break;
                case HotSpots::bottomRightCorner:
                    geometry.setBottomRight(globalPoint + QPoint { m_rightDiff, m_bottomDiff });

                    break;
                case HotSpots::topBorder:
                    geometry.setTop(globalPoint.y() - m_topDiff);

                    break;
                case HotSpots::bottomBorder:
                    geometry.setBottom(globalPoint.y() + m_bottomDiff);

                    break;
                case HotSpots::rightBorder:
                    geometry.setRight(globalPoint.x() + m_rightDiff);

                    break;
                case HotSpots::leftBorder:
                    geometry.setLeft(globalPoint.x() - m_leftDiff);
                    break;
            }
        }
        m_startPoint = globalPoint;
    }

    if (event->buttons() & Qt::LeftButton)
        m_topLevelWidget->setGeometry(geometry);

    QFrame::mouseMoveEvent(event);
}

void ResizableFrame::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseAt = -1;
    releaseMouse();

    QFrame::mouseReleaseEvent(event);
}

void ResizableFrame::resizeEvent(QResizeEvent *event)
{
    m_activeFrames[topLeftCorner] = {
        0, 0, 5, 5
    };

    m_activeFrames[topRightCorner] = {
        event->size().width() - 5, 0, 5, 5
    };

    m_activeFrames[bottomLeftCorner] = {
        0, event->size().height() - 5, 5, 5
    };

    m_activeFrames[bottomRightCorner] = {
        event->size().width() - 5, event->size().height() - 5, 5, 5
    };

    m_activeFrames[topBorder] = {
        5, 0, event->size().width() - 10, 3
    };

    m_activeFrames[rightBorder] = {
        event->size().width() - 3, 5, 3, event->size().height() - 10
    };

    m_activeFrames[leftBorder] = {
        0, 5, 3, event->size().height() - 10
    };

    m_activeFrames[bottomBorder] = {
        5, event->size().height() - 3, event->size().width() - 10, 3
    };

    QFrame::resizeEvent(event);
}

/* FOR DEBUGGING PORPOISES: This draws the resizing rectangles
void ResizableFrame::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(0, 255, 255)));
    painter.drawRects(m_activeFrames, 4);
    painter.setBrush(QBrush(QColor(255, 0, 255)));
    painter.drawRects(m_activeFrames + 4, 4);

}
*/

END_CENTAUR_NAMESPACE
