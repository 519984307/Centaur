////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "ChevronButton.hpp"
#include <QPaintEvent>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

BEGIN_CENTAUR_NAMESPACE

struct ChevronButton::Impl
{
    Impl() :
        chevron(":/button/left_chevron") { }

    QIcon chevron;
    qreal angle { 0.0 };
    bool animations;

    QParallelAnimationGroup *animationGroup { nullptr };
    QPropertyAnimation *rotationAnimation { nullptr };
};

ChevronButton::ChevronButton(QWidget *parent) :
    QPushButton(parent),
    _impl { new ChevronButton::Impl }
{
    connect(this, &QPushButton::toggled, this, &ChevronButton::checkStateChange);
    allowAnimations(true);
}

ChevronButton::~ChevronButton() = default;

void ChevronButton::paintEvent(QPaintEvent *event)
{

    QPushButton::paintEvent(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    auto thisRect = rect();
    thisRect.setLeft(thisRect.right() - 35);
    thisRect.setTop(5);
    thisRect.setBottom(thisRect.bottom() - 6);
    thisRect.setRight(thisRect.left() + 24);

    if (thisRect.left() > rect().width() / 2)
    {
        QTransform t;
        t.rotate(getAngle());
        painter.drawPixmap(thisRect, _impl->chevron.pixmap(24, 24).transformed(t, Qt::TransformationMode::SmoothTransformation));
    }
}

qreal ChevronButton::getAngle() const noexcept
{
    return _impl->angle;
}

void ChevronButton::setAngle(qreal angle) noexcept
{
    _impl->angle = angle;
    repaint();
}

void ChevronButton::allowAnimations(bool status)
{
    _impl->animations = status;
    if (status)
    {
        _impl->animationGroup    = new QParallelAnimationGroup(this);
        _impl->rotationAnimation = new QPropertyAnimation(this, "rotate", _impl->animationGroup);

        connect(_impl->animationGroup, &QParallelAnimationGroup::finished, this, [&]() { emit animationFinish(); });

        _impl->animationGroup->addAnimation(_impl->rotationAnimation);
    }
    else
    {
        delete _impl->animationGroup;
    }
}

void ChevronButton::checkStateChange(bool state) noexcept
{
    if (_impl->animations && isVisible())
    {
        const auto startAngleValue = getAngle();
        const auto endAngleValue   = state ? -90.0 : 0.0;
        _impl->rotationAnimation->setDuration(750);
        _impl->rotationAnimation->setEasingCurve(QEasingCurve::OutQuart);
        _impl->rotationAnimation->setStartValue(startAngleValue);
        _impl->rotationAnimation->setEndValue(endAngleValue);

        emit animationAboutToStart();

        _impl->animationGroup->start();
    }
    else
    {
        if (state)
        {
            setAngle(-90.0);
        }
        else
            setAngle(0);
    }
}

void ChevronButton::addAnimationToButton(QPropertyAnimation *newAnimation, bool internalTime)
{
    if (newAnimation->parent() != _impl->animationGroup)
    {
        if (internalTime)
            newAnimation->setDuration(750);

        newAnimation->setParent(_impl->animationGroup);
        _impl->animationGroup->addAnimation(newAnimation);
    }
}

END_CENTAUR_NAMESPACE
