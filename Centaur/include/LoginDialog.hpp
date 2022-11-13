////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef CENTAUR_LOGINDIALOG_HPP
#define CENTAUR_LOGINDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class LoginDialog;
}
class LoginDialog : public QDialog
{
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override;

#ifndef TEST_LOGIN_MODE
protected slots:
#else
public slots:
#endif /*TEST_LOGIN_MODE*/
    void onAccept() noexcept;

public:
    /// \brief Only asks for the password
    void setPasswordMode() noexcept;
    void setNormalMode() noexcept;
    void setTFAMode() noexcept;

public:
    /// \brief Only valid when password mode is set, and the caller must clear the data
    QString userPassword;

protected:
    void restoreInterface() noexcept;
    void loadImage() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::LoginDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_LOGINDIALOG_HPP
