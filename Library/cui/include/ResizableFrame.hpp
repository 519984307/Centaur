////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_RESIZABLEFRAME_HPP
#define CENTAUR_RESIZABLEFRAME_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"
// Qt Specifics

#include <QFrame>
BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY ResizableFrame : public QFrame
{
    enum HotSpots : int
    {
        topLeftCorner = 0,
        topRightCorner,
        bottomLeftCorner,
        bottomRightCorner,
        topBorder,
        bottomBorder,
        rightBorder,
        leftBorder
    };

public:
    explicit ResizableFrame(QWidget *parent = nullptr);
    ~ResizableFrame() override = default;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    // void paintEvent(QPaintEvent *event) override;

private:
    int m_mouseAt { -1 };
    int m_bottomDiff { 0 };
    int m_rightDiff { 0 };
    int m_leftDiff { 0 };
    int m_topDiff { 0 };
    QRect m_activeFrames[8];
    QPoint m_startPoint;
    QWidget *m_topLevelWidget;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_RESIZABLEFRAME_HPP
