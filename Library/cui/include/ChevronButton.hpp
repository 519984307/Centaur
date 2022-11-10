////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#ifndef FINANCEABLE_CHEVRONBUTTON_HPP
#define FINANCEABLE_CHEVRONBUTTON_HPP

#include "CentaurLibrary.hpp"
#include "Centaur.hpp"
#include <QPushButton>

class QPropertyAnimation;
BEGIN_CENTAUR_NAMESPACE

class ChevronButton final : public QPushButton
{
    Q_OBJECT

public:
    ChevronButton(QWidget *parent);
    ~ChevronButton() override;

    Q_PROPERTY(qreal rotate READ getAngle WRITE setAngle)

public:
    qreal getAngle() const noexcept;
    void setAngle(qreal angle) noexcept;



public:
    void allowAnimations(bool status);

public:
    void addAnimationToButton(QPropertyAnimation *newAnimation, bool internalTime = true);

protected slots:
    void checkStateChange(bool newState) noexcept;

signals:
    void animationFinish();
    // IMPORTANT: Connect view Qt::DirectConnection
    void animationAboutToStart();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE

#endif // FINANCEABLE_CHEVRONBUTTON_HPP
