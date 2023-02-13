/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "../ui/ui_SettingsDialog.h"
#include "AddUserDialog.hpp"
#include "Globals.hpp"
#include "Logger.hpp"
#include "LoginDialog.hpp"
#include "QRCodeDialog.hpp"
#include "SettingsDialog.hpp"
#include "TOTP.hpp"

#include <QCheckBox>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

namespace
{
    using updatePtr = void (*)(const QString &, bool);
    using statusPtr = bool (*)(const QString &);

    std::string toBase32(const std::string &data)
    {
        const auto size       = data.size();
        const auto mod        = size % 5;
        const auto iterations = [&]() {
            auto iter = size / 5;
            return mod ? iter + 1 : iter;
        }();

        if (size == 0)
            return {};

        static constexpr std::array<char, 33> base32Table = {
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
        };
        static constexpr std::array<uint64_t, 5> index   = { 0, 2, 4, 5, 7 };
        static constexpr std::array<uint64_t, 5> padding = { 0, 6, 4, 3, 1 };

        std::string out;
        out.reserve(iterations * 8);
        for (auto i = 0ull; i < iterations; ++i)
        {
            const auto copy = (i == iterations - 1 && mod) ? mod : 5ull;

            uint64_t block { 0 };
            memmove(&block, data.substr(i * 5, copy).c_str(), copy);
            uint64_t block_be = 0xffffffffff & ((block << 32) | (block << 16 & 0xFF000000) | (block << 0 & 0x00FF0000) | (block >> 16 & 0x0000FF00) | (block >> 32 & 0x000000FF));

            for (int k = 7; k >= 0; --k)
            {
                const uint64_t idx = (block_be >> 5 * k) & 0x1F;
                out.push_back(base32Table[idx]);
            }
        }
        if (mod)
        {
            for (auto k = (iterations - 1) * 8 + index[mod], i = 0ull; i < padding[mod]; ++i, ++k)
                out[k] = '=';
        }

        return out;
    }
} // namespace

BEGIN_CENTAUR_NAMESPACE

void SettingsDialog::initSessionWidget() noexcept
{
    connect(ui()->addUserBtn, &QPushButton::released, this, &SettingsDialog::onAddUser);
    connect(_sesImpl->_checkBox, &QCheckBox::clicked, this, &SettingsDialog::onAllow2FA);

    // Info user table data
    ui()->userTable->setColumnCount(2);
    ui()->userTable->setRowCount(6);
    ui()->userTable->setHorizontalHeaderLabels({ tr("Option"), tr("Value") });
    ui()->userTable->setItem(0, 0, _sesImpl->_name);
    ui()->userTable->setItem(1, 0, _sesImpl->_user);
    ui()->userTable->setItem(2, 0, _sesImpl->_psw);
    ui()->userTable->setItem(3, 0, _sesImpl->_email);
    ui()->userTable->setItem(4, 0, _sesImpl->_picture);
    ui()->userTable->setItem(5, 0, _sesImpl->_2fa);
    ui()->userTable->setItem(0, 1, _sesImpl->_nameValue);
    ui()->userTable->setItem(1, 1, _sesImpl->_userValue);
    ui()->userTable->setItem(2, 1, _sesImpl->_pswValue);
    ui()->userTable->setItem(3, 1, _sesImpl->_emailValue);
    ui()->userTable->setItem(4, 1, _sesImpl->_pictureValue);
    ui()->userTable->setCellWidget(5, 1, _sesImpl->_checkBoxWidget);

    // First time started
    if (isFirstTimeStarted())
    {

        QIcon addIcon;
        addIcon.addFile(QString::fromUtf8(":/buttons/add"), QSize(), QIcon::Normal, QIcon::Off);
        ui()->addUserBtn->setIcon(addIcon);
        ui()->addUserBtn->setText(tr("Add"));
    }
    else
    {
        QIcon editIcon;
        editIcon.addFile(QString::fromUtf8(":/options/edit"), QSize(), QIcon::Normal, QIcon::Off);
        ui()->addUserBtn->setIcon(editIcon);
        ui()->addUserBtn->setText(tr("Edit"));

        // Open the database to fill the data
        openUserInformationData();
    }
}

void SettingsDialog::onAddUser() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("__Session__data");

    bool editingMode = false;
    AddUserDialog dlg(this);
    if (!g_globals->session.user.isEmpty())
    {
        editingMode = true;
        dlg.enableEditOnly(
            _sesImpl->_userValue->text(),
            _sesImpl->_nameValue->text(),
            _sesImpl->_emailValue->text(),
            _sesImpl->_pictureValue->data(Qt::UserRole).value<QImage>());
    }

    if (dlg.exec() == QDialog::Accepted)
    {
        // The database will be created
        AddUserDialog::UserInformation ifo = dlg.getUserInformation();

        // Initialize the data in the table
        _sesImpl->_userValue->setText(ifo.user);
        _sesImpl->_nameValue->setText(ifo.name);
        _sesImpl->_emailValue->setText(ifo.mail);

        g_globals->session.user    = ifo.user;
        g_globals->session.display = ifo.name;
        g_globals->session.email   = ifo.mail;

        if (!_sesImpl->_pswValue->text().isEmpty())
        {
            _sesImpl->_pswValue->setText(QString("*").repeated(15));
            _sesImpl->_pswValue->setData(Qt::UserRole, ifo.psw);

            const QString tfaFile = []() -> QString {
                QString data = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
                return QString("%1/f6110cb58f3b").arg(data);
            }();

            QFile file(tfaFile);
            if (!file.open(QIODeviceBase::ReadOnly) && g_globals->session.tfa)
            {
                // Code: 0x00000001 is encryption failed
                // Code: 0x00000002 is a file failure
                QMessageBox::critical(this, tr("Error"), QString(tr("2FA can not be updated.\n2FA will be disabled.\nError: 0x0000001")),
                    QMessageBox::Ok);
                _sesImpl->_checkBox->setChecked(false);
            }
            else
            {
                QTextStream stream(&file);
                QString fileData = stream.readAll();
                file.close();

                g_globals->session.userTFA = AESSym::decrypt(fileData, ifo.prevPsw.toLocal8Bit());

                if (!QFile::remove(tfaFile))
                {
                    QMessageBox::critical(this, tr("Error"), QString(tr("2FA can not be updated.\n2FA will be disabled.\nError: 0x0000002")),
                        QMessageBox::Ok);
                    _sesImpl->_checkBox->setChecked(false);
                }
                else
                {
                    auto result = AESSym::encrypt(g_globals->session.userTFA.toLocal8Bit(), ifo.psw.toLocal8Bit());
                    QFile file(tfaFile);
                    if (!file.open(QIODeviceBase::WriteOnly) || result.isEmpty())
                    {
                        // Code: 0x00000001 is: encryption failed
                        // Code: 0x00000002 is: a file failure
                        QMessageBox::critical(this, tr("Error"),
                            QString(tr("2FA can not be updated.\n2FA will be disabled.\nError: 0x0000003")),
                            QMessageBox::Ok);
                        _sesImpl->_checkBox->setChecked(false);
                    }
                    else
                    {
                        // Store the data
                        QTextStream stream(&file);
                        stream << result;
                        file.close();
                    }
                }
            }
        }

        if (ifo.photoUpdate)
        {
            _sesImpl->_pictureValue->setData(Qt::DecorationRole, QPixmap::fromImage(ifo.photograph).scaled(100, 100, Qt::KeepAspectRatio));
            _sesImpl->_pictureValue->setData(Qt::UserRole, ifo.photograph);
            g_globals->session.image = ifo.photograph;
            ui()->userTable->setRowHeight(4, 100);
        }

        QIcon editIcon;
        editIcon.addFile(QString::fromUtf8(":/options/edit"), QSize(), QIcon::Normal, QIcon::Off);
        ui()->addUserBtn->setIcon(editIcon);
        ui()->addUserBtn->setText(tr("Edit"));

        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__Session__data");
        settings.setValue("__user__", g_globals->session.user);
        settings.setValue("__display__", g_globals->session.display);
        settings.setValue("__email__", g_globals->session.email);
        if (!editingMode || ifo.pswUpdate)
            settings.setValue("__sec__", QString::fromUtf8(QCryptographicHash::hash(QByteArrayView(ifo.psw.toUtf8()), QCryptographicHash::RealSha3_512).toBase64()));
        settings.endGroup();

        settings.beginGroup("__Session__");
        settings.setValue("__time__", true);
        settings.endGroup();
    }
}

void SettingsDialog::resetTablesValues() noexcept
{
    _sesImpl->_nameValue->setText("");
    _sesImpl->_userValue->setText("");
    _sesImpl->_pswValue->setText("");
    _sesImpl->_emailValue->setText("");
    _sesImpl->_pictureValue->setText("");
    _sesImpl->_checkBox->setChecked(false);

    QIcon addIcon;
    addIcon.addFile(QString::fromUtf8(":/buttons/add"), QSize(), QIcon::Normal, QIcon::Off);
    ui()->addUserBtn->setIcon(addIcon);
    ui()->addUserBtn->setText(tr("Add"));
}

void SettingsDialog::openUserInformationData() noexcept
{
    QSettings settings("CentaurProject", "Centaur");

    const QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    QDir pathDir;
    pathDir.setPath(dataPath);

    settings.beginGroup("__Session__data");
    g_globals->session.user    = settings.value("__user__").toString();
    g_globals->session.display = settings.value("__display__").toString();
    g_globals->session.email   = settings.value("__email__").toString();
    g_globals->session.tfa     = settings.value("__2fa__").toBool();
    settings.endGroup();

    const QString imageFilePath = QString("%1/e30dfd91071c.image.data").arg(dataPath);

    _sesImpl->_userValue->setText(g_globals->session.user);
    _sesImpl->_nameValue->setText(g_globals->session.display);
    _sesImpl->_emailValue->setText(g_globals->session.email);

    g_globals->session.image.load(imageFilePath);
    if (g_globals->session.image.isNull())
        logWarn("session", "user image does not exists");
    else
    {
        _sesImpl->_pictureValue->setData(Qt::DecorationRole, QPixmap::fromImage(g_globals->session.image).scaled(100, 100, Qt::KeepAspectRatio));
        _sesImpl->_pictureValue->setData(Qt::UserRole, g_globals->session.image);
    }

    ui()->userTable->setRowHeight(4, 100);

    _sesImpl->_checkBox->setChecked(g_globals->session.tfa);
}

void SettingsDialog::onAllow2FA(bool checked) noexcept
{
    const QString tfaFile = []() -> QString {
        QString data = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        return QString("%1/f6110cb58f3b").arg(data);
    }();

    if (checked)
    {
        if (_sesImpl->_emailValue->text().isEmpty())
        {
            QMessageBox::warning(this, tr("Warning"), tr("e-mail user field could not be empty"), QMessageBox::Ok);
            _sesImpl->_checkBox->setChecked(false);
            return;
        }

        if (tfaFile.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("There is an error in the user identification. 2FA is not available"), QMessageBox::Ok);
            _sesImpl->_checkBox->setChecked(false);
            return;
        }
        else
        {
            if (QFile::exists(tfaFile))
            {
                int res = QMessageBox::warning(this, tr("Warning"),
                    tr("Currently, there is 2FA registered and the data can not be retrieved for security.\nWould you like to repeat the process?.\nSelecting no the 2FA will be disabled"),
                    QMessageBox::Yes | QMessageBox::No);
                if (res == QMessageBox::Yes)
                {
                    // Remove and continue
                    QFile::remove(tfaFile);
                }
                else
                {
                    // Do not continue and set the checked flag
                    _sesImpl->_checkBox->setChecked(false);
                    return;
                }
            }
        }

        // Create a 120-bits random key, shuffled 1000 times
        auto generatedKey = AESSym::createUniqueId(1'000, 15);
        auto base32       = QString::fromStdString(toBase32(generatedKey));

        // Store in the file the key encrypted with the password
        // Since this operation can fail and return, write the file first and if it not fails, just then, show the QR code dialog
        auto userPsw = _sesImpl->_pswValue->data(Qt::UserRole).toString();
        if (userPsw.isEmpty())
        {
            LoginDialog dlg(this);
            dlg.setPasswordMode();
            if (dlg.exec() == QDialog::Accepted)
            {
                userPsw          = dlg.userPassword;
                dlg.userPassword = QString::fromStdString(AESSym::createUniqueId(10, 8)); // clear the data with random digits
            }
            else
            {
                QMessageBox::critical(this, "Critical", QString(tr("In order to provide 2FA you must set your password")));
                _sesImpl->_checkBox->setChecked(false);
                return;
            }
        }

        auto result = AESSym::encrypt(QString::fromStdString(generatedKey).toLocal8Bit(), userPsw.toLocal8Bit());
        QFile file(tfaFile);
        if (!file.open(QIODeviceBase::WriteOnly) || result.isEmpty())
        {
            // Code: 0x00000001 is: encryption failed
            // Code: 0x00000002 is: a file failure
            QMessageBox::critical(this, tr("Error"),
                QString(tr("There is an error in the user identification. 2FA is not available.\nCode: %1")).arg(result.isEmpty() ? "0x00000001" : "0x00000002"),
                QMessageBox::Ok);
            _sesImpl->_checkBox->setChecked(false);
            return;
        }

        // Store the data
        QTextStream stream(&file);
        stream << result;
        file.close();

        // Show the code
        QRCodeDialog qrCode(_sesImpl->_emailValue->text(), base32, this);
        qrCode.exec();

        g_globals->session.tfa = true;
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__Session__data");
        settings.setValue("__2fa__", g_globals->session.tfa);
        settings.endGroup();
    }
    else
    {
        g_globals->session.tfa = false;
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__Session__data");
        settings.setValue("__2fa__", g_globals->session.tfa);
        settings.endGroup();
    }
}

END_CENTAUR_NAMESPACE
