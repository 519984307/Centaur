////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_SYSTEMPUSHBUTTON_HPP
#define CENTAUR_SYSTEMPUSHBUTTON_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"
#include <QPushButton>

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY SystemPushButton : public QPushButton
{
    Q_OBJECT

public:
    enum class ButtonClass
    {
        close = 0,
        minimize,
#ifdef Q_OS_MAC
        fullscreen,
#else
        maximize,
#endif /* Q_OS_MAC */
        override
    };
public:
    explicit SystemPushButton(QWidget *parent = nullptr);
    ~SystemPushButton() override = default;

    void setButtonClass(ButtonClass btnClass) noexcept;

#ifdef Q_OS_MAC
    void linkClose(SystemPushButton *min, SystemPushButton *fullscreen);
    void linkMinimize(SystemPushButton *close, SystemPushButton *fullscreen);
    void linkFullScreen(SystemPushButton *close, SystemPushButton *min);
#else
    void linkClose(SystemPushButton *min, SystemPushButton *maximize);
    void linkMinimize(SystemPushButton *close, SystemPushButton *maximize);
    void linkMaximize(SystemPushButton *close, SystemPushButton *min);
#endif

public slots:
    void activate() noexcept;

signals:
    void systemPressed();

private:
    SystemPushButton *m_linkMin { nullptr };
    SystemPushButton *m_linkMaxFull { nullptr };
    SystemPushButton *m_linkClose { nullptr };

    ButtonClass m_class { ButtonClass::close };

    QWidget *m_topLevelWidget;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_SYSTEMPUSHBUTTON_HPP
