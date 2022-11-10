////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

//
// Created by Ricardo Romero on 03/11/22.
//

#ifndef FINANCEABLE_ANIMATEDBUTTON_HPP
#define FINANCEABLE_ANIMATEDBUTTON_HPP

#include "CentaurLibrary.hpp"
#include "Centaur.hpp"
#include <QPushButton>

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY AnimatedButton : public QPushButton
{
public:
    enum class Animation
    {
        Top,
        Left,
        Bottom,
        Right
    };
public:
    explicit AnimatedButton(QWidget *parent = nullptr);
    ~AnimatedButton() override;

public:
    void setExpandedText(const QString &text);
    void setAnimationPosition(Animation pos);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;



private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE
#endif // FINANCEABLE_ANIMATEDBUTTON_HPP
