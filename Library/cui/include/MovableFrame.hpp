////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_MOVABLEFRAME_HPP
#define CENTAUR_MOVABLEFRAME_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"

#include <QFrame>

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY MovableFrame : public QFrame
{
public:
    explicit MovableFrame(QWidget *parent = nullptr);
    ~MovableFrame() override = default;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

public:
    void overrideParent(QWidget *newParent) noexcept;

private:
    QRect m_normalGeometry;
    QWidget *m_topLevelWidget;
    QPoint m_startPoint;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_MOVABLEFRAME_HPP
