////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "LoginDialog.hpp"
#include "../ui/ui_LoginDialog.h"
#include "Globals.hpp"
#include "TOTP.hpp"
#include <QCryptographicHash>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QUuid>

BEGIN_CENTAUR_NAMESPACE

struct LoginDialog::Impl
{
    inline Impl() :
        ui { new Ui::LoginDialog } { }
    inline ~Impl() = default;

    bool loginTFA { false };
    bool pswMode { false };
    bool tfaMode { false };

    std::unique_ptr<Ui::LoginDialog> ui;
};

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);

    connect(ui()->acceptButton, &QPushButton::released, this, &LoginDialog::onAccept);

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    restoreInterface();
}

LoginDialog::~LoginDialog() = default;

Ui::LoginDialog *LoginDialog::ui()
{
    return _impl->ui.get();
}

void LoginDialog::onAccept() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("LoginDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("__Session__data");
    const QString registeredPassword = settings.value("__sec__").toString();
    const QString registeredUser     = settings.value("__user__").toString();
    const bool loginTFA              = settings.value("__2fa__").toBool();
    settings.endGroup();


    const QString hashedPassword = QString::fromUtf8(QCryptographicHash::hash(QByteArrayView(ui()->passwordEdit->text().toUtf8()), QCryptographicHash::RealSha3_512).toBase64());

    if (!_impl->tfaMode && _impl->pswMode)
    {
        if (registeredPassword != hashedPassword)
        {
            ui()->errLabel->setText(tr("Wrong password"));
            ui()->errLabel->show();
            return;
        }

        if (_impl->pswMode)
        {
            userPassword = ui()->passwordEdit->text();
        }

        accept();
    }

    if (!_impl->pswMode && !_impl->tfaMode)
    {
        // Normal mode

#ifndef TEST_LOGIN_MODE // Avoid do all the testing
        if (ui()->userEdit->text() != registeredUser)
        {
            ui()->errLabel->setText(tr("Wrong user"));
            ui()->errLabel->show();
            return;
        }

        if (registeredPassword != hashedPassword)
        {
            ui()->errLabel->setText(tr("Wrong password"));
            ui()->errLabel->show();
            return;
        }

        if (loginTFA)
        {
            const QString tfaFile = []() -> QString {
                QString data = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
                return QString("%1/f6110cb58f3b").arg(data);
            }();

            QFile file(tfaFile);
            if (!file.open(QIODeviceBase::ReadOnly))
            {
                // Code: 0x00000001 is encryption failed
                // Code: 0x00000002 is a file failure
                QMessageBox::critical(this, tr("Error"), QString(tr("There is an error in the user identification. 2FA is not available.")));
                exit(EXIT_FAILURE);
            }

            QTextStream stream(&file);
            QString fileData = stream.readAll();
            file.close();

            g_globals->session.userTFA = AESSym::decrypt(fileData, ui()->passwordEdit->text().toLocal8Bit());

            if (ui()->tfaEdit->text().toInt() != getTOTPCode(g_globals->session.userTFA.toStdString()))
            {
                ui()->errLabel->setText(tr("Wrong 2FA input"));
                ui()->errLabel->show();
                return;
            }
        }
#endif /*TEST_LOGIN_MODE*/

        settings.beginGroup("__Session__data");
        g_globals->session.user    = settings.value("__user__").toString();
        g_globals->session.display = settings.value("__display__").toString();
        g_globals->session.email   = settings.value("__email__").toString();
        g_globals->session.tfa     = settings.value("__2fa__").toBool();
        settings.endGroup();
    }

    if (_impl->tfaMode)
    {
        if (g_globals->session.userTFA.isEmpty())
        {
            if (registeredPassword != registeredUser)
            {
                ui()->errLabel->setText(tr("Wrong password"));
                ui()->errLabel->show();
                return;
            }

            if (_impl->pswMode)
            {
                userPassword = ui()->passwordEdit->text();
            }

            const QString tfaFile = []() -> QString {
                QString data = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
                return QString("%1/f6110cb58f3b").arg(data);
            }();

            QFile file(tfaFile);
            if (!file.open(QIODeviceBase::ReadOnly))
            {
                // Code: 0x00000001 is encryption failed
                // Code: 0x00000002 is a file failure
                QMessageBox::critical(this, tr("Error"), QString(tr("There is an error in the user identification. 2FA is not available.")));
                exit(EXIT_FAILURE);
            }

            QTextStream stream(&file);
            QString fileData = stream.readAll();
            file.close();

            g_globals->session.userTFA = AESSym::decrypt(fileData, ui()->passwordEdit->text().toLocal8Bit());
        }

        if (ui()->tfaEdit->text().toInt() != getTOTPCode(g_globals->session.userTFA.toStdString()))
        {
            ui()->errLabel->setText(tr("Wrong 2FA input"));
            ui()->errLabel->show();
            return;
        }

        accept();
    }

#ifndef TEST_LOGIN_MODE // avoid closing because it's not going to work
    accept();
#endif /*TEST_LOGIN_MODE*/
}

void LoginDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("LoginDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void LoginDialog::setPasswordMode() noexcept
{
    _impl->pswMode = true;
    ui()->userEdit->setReadOnly(true);
    ui()->userEdit->setText(g_globals->session.user);
    ui()->tfaEdit->hide();
    ui()->iconLabel->setPixmap(
        QPixmap::fromImage(
            g_globals->session.image.scaled(ui()->iconLabel->size(), Qt::KeepAspectRatio)));
}

void LoginDialog::setNormalMode() noexcept
{
    // Select the image
    loadImage();

    // Determinate if the 2FA is activated
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("__Session__data");
    g_globals->session.tfa = settings.value("__2fa__").toBool();
    settings.endGroup();

    // Index 0 holds the 2FA activated and Index 1 holds the login 2fa
    if (g_globals->session.tfa)
    {
        _impl->loginTFA = true;
        ui()->tfaEdit->show();
    }
    else
        ui()->tfaEdit->hide();
}

void LoginDialog::setTFAMode() noexcept
{
    _impl->tfaMode = true;
    ui()->userEdit->setReadOnly(true);
    ui()->userEdit->setText(g_globals->session.user);
    ui()->iconLabel->setPixmap(
        QPixmap::fromImage(
            g_globals->session.image.scaled(ui()->iconLabel->size(), Qt::KeepAspectRatio)));

    ui()->userEdit->hide();
    if (!g_globals->session.userTFA.isEmpty())
        ui()->passwordEdit->hide();
}

void LoginDialog::loadImage() noexcept
{
    const QString dataPath      = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    const QString imageFilePath = QString("%1/e30dfd91071c.image.data").arg(dataPath);

    if(QFile::exists(imageFilePath))
    {
        g_globals->session.image.load(imageFilePath);

        ui()->iconLabel->setPixmap(
            QPixmap::fromImage(
                g_globals->session.image.scaled(ui()->iconLabel->size(), Qt::KeepAspectRatio)));
    }
}

END_CENTAUR_NAMESPACE
