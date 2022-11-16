////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_ADDUSERDIALOG_HPP
#define CENTAUR_ADDUSERDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class AddUserDialog;
}
class AddUserDialog : public QDialog
{
    enum RoleIndex
    {
        dbIndex = Qt::UserRole + 1
    };

public:
    struct UserInformation
    {
        QString user;
        QString name;
        QString psw;
        QString prevPsw;
        QString mail;
        QImage photograph;

        bool photoUpdate;
        bool pswUpdate;
    };

public:
    AddUserDialog(QWidget *parent = nullptr);
    ~AddUserDialog() override;

public:
    C_NODISCARD UserInformation getUserInformation() const noexcept;

public:
    void enableEditOnly(const QString &usr, const QString &display, const QString &email, const QImage &image) noexcept;

public slots:
    void onAccept() noexcept;
    void searchImage() noexcept;

protected:
    void restoreInterface() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::AddUserDialog *ui();
    Ui::AddUserDialog *ui() const;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_ADDUSERDIALOG_HPP
