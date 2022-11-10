////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "AnimatedButton.hpp"
#include <QPropertyAnimation>

BEGIN_CENTAUR_NAMESPACE

struct AnimatedButton::Impl
{
    explicit inline Impl(QObject *parent) :
        anim { new QPropertyAnimation(parent, "geometry") },
        displayPixels { 0 },
        position { Animation::Right },
        expandedState { false } { }
    inline ~Impl() = default;

    QPropertyAnimation *anim;
    QString expandedText;
    QIcon icon;

    int displayPixels;

    QRect initialGeometry;

    Animation position;

    bool expandedState;
};

AnimatedButton::AnimatedButton(QWidget *parent) :
    QPushButton { parent },
    _impl { new Impl(this) }
{
    setAnimationPosition(Animation::Right);

    _impl->anim->setEasingCurve(QEasingCurve::OutQuart);
    _impl->anim->setDuration(250);


    connect(_impl->anim, &QPropertyAnimation::finished, this,
        [&]() {
            if (_impl->expandedState)
                setText(_impl->expandedText);
            else
                setText("");

            setIcon(_impl->icon);
        });
}

void AnimatedButton::enterEvent(QEnterEvent *event)
{
    if (_impl->anim->state() == QPropertyAnimation::State::Running)
    {
        // _impl->initialGeometry can not be updated here because
        // frameGeometry() will have the geometry of whatever state the animation is in.
        // This will later cause problems since leaveEvent() uses _impl->initialGeometry to get the initial state.
        // So this initial state will be at any point in time the animation is in, which is not a determinate state
        _impl->anim->stop();
    }
    else
    {
        // This condition will be triggered in the first animation and whenever the animation is stopped (a.k.a. finished)
        // And the intention of this, is to update the initial geometry every time the animation starts, because the parent window might have been
        // resized, moved, maximized, which will essentially modify the left and right values of this widget.
        _impl->initialGeometry = frameGeometry();

        if(!icon().isNull())
            _impl->icon = icon();
    }



    auto thisRect = frameGeometry();
    thisRect.setX(_impl->initialGeometry.x() - _impl->displayPixels - 16 );

    // frameGeometry() is not guaranteed tha it is the same as  _impl->initialGeometry
    // because the user can leave the widget and returned in less than 250ms which will imply that
    // _impl->initialGeometry and frameGeometry() are in completely different states
    _impl->anim->setStartValue(frameGeometry());
    _impl->anim->setEndValue(thisRect);

   setIcon(QIcon());

    _impl->expandedState = true;
    _impl->anim->start();



    QWidget::enterEvent(event);
}

void AnimatedButton::leaveEvent(QEvent *event)
{
    if (_impl->anim->state() == QPropertyAnimation::State::Running)
        _impl->anim->stop();

    _impl->anim->setStartValue(frameGeometry());
    _impl->anim->setEndValue(_impl->initialGeometry);

    _impl->expandedState = false;

    _impl->anim->start();



    QWidget::leaveEvent(event);
}

void AnimatedButton::setAnimationPosition(AnimatedButton::Animation pos)
{
    _impl->position = pos;
}

void AnimatedButton::setExpandedText(const QString &text)
{
    auto thisFont = font();
    QFontMetrics metrics(thisFont);
    _impl->expandedText  = text;
    _impl->displayPixels = metrics.horizontalAdvance(text);
}

AnimatedButton::~AnimatedButton() = default;

END_CENTAUR_NAMESPACE
