////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "AddUserDialog.hpp"
#include "../ui/ui_AddUserDialog.h"
#include "CentaurGlobal.hpp"
#include "Logger.hpp"
#include <QCryptographicHash>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QSettings>
#include <QStandardPaths>

BEGIN_CENTAUR_NAMESPACE

struct AddUserDialog::Impl
{
    inline Impl() :
        ui { new Ui::AddUserDialog } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::AddUserDialog> ui;

    QImage photo;

    bool editingMode { false };
    bool photoUpdate { false };
    bool pswUpdate { false };
};

AddUserDialog::AddUserDialog(QWidget *parent) :
    QDialog(parent),
    _impl { new AddUserDialog::Impl }
{
    ui()->setupUi(this);

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    ui()->titleFrame->overrideParent(this);

    ui()->closeButton->setButtonClass(SystemPushButton::ButtonClass::override);
    connect(ui()->closeButton, &SystemPushButton::systemPressed, this, [&]() {
        reject();
    });

    ui()->newUsrEdit->linkLabel(ui()->label2);
    ui()->newPswEdit->linkLabel(ui()->passwordLabel);
    ui()->confirmEdit->linkLabel(ui()->label4);
    ui()->nameEdit->linkLabel(ui()->label5);
    ui()->mailEdit->linkLabel(ui()->label8);
    ui()->oldPasswordEdit->linkLabel(ui()->oldPasswordLabel);

    ui()->pswConfirmErrorLabel->hide();
    ui()->pswErrorLabel->hide();
    ui()->userErrorLabel->hide();

    ui()->nameErrorLabel->hide();
    ui()->mailErrorLabel->hide();

    connect(ui()->addButton, &QPushButton::released, this, &AddUserDialog::onAccept);
    connect(ui()->cancelButton, &QPushButton::released, this, &QDialog::reject);
    connect(ui()->searchPhoto, &QPushButton::released, this, &AddUserDialog::searchImage);

    ui()->oldPasswordLabel->hide();
    ui()->oldPasswordEdit->hide();
    ui()->oldPswErrorLabel->hide();

    restoreInterface();
}

AddUserDialog::~AddUserDialog() = default;

Ui::AddUserDialog *AddUserDialog::ui()
{
    return _impl->ui.get();
}

Ui::AddUserDialog *AddUserDialog::ui() const
{
    return _impl->ui.get();
}

void AddUserDialog::enableEditOnly(const QString &usr, const QString &display, const QString &email, const QImage &image) noexcept
{
    _impl->editingMode = true;
    ui()->newUsrEdit->setText(usr);
    ui()->nameEdit->setText(display);
    ui()->mailEdit->setText(email);
    ui()->passwordLabel->setText(tr("New Password"));
    ui()->oldPasswordLabel->show();
    ui()->oldPasswordEdit->show();
    // Look for the user data
    _impl->photo = image;
    ui()->photoLabel->setPixmap(QPixmap::fromImage(_impl->photo.scaled(ui()->photoLabel->size())));
}

AddUserDialog::UserInformation AddUserDialog::getUserInformation() const noexcept
{
    UserInformation aud;

    aud.user       = ui()->newUsrEdit->text();
    aud.name       = ui()->nameEdit->text();
    aud.psw        = ui()->newPswEdit->text();
    aud.mail       = ui()->mailEdit->text();
    aud.photograph = _impl->photo;

    aud.photoUpdate = _impl->photoUpdate;
    aud.pswUpdate   = _impl->pswUpdate;

    return aud;
}

void AddUserDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("AddUserDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void AddUserDialog::onAccept() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("AddUserDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    ui()->pswConfirmErrorLabel->hide();
    ui()->pswErrorLabel->hide();
    ui()->userErrorLabel->hide();
    ui()->nameErrorLabel->hide();
    ui()->mailErrorLabel->hide();
    ui()->oldPswErrorLabel->hide();

    // Perhaps, the user erase all fields of the password, so we are no longer updating this field
    _impl->pswUpdate = false;

    if (ui()->newUsrEdit->text().isEmpty())
    {
        ui()->userErrorLabel->setText(tr("Must type a username"));
        ui()->userErrorLabel->show();
        return;
    }

    if (_impl->editingMode)
    {

        if (!ui()->oldPasswordEdit->text().isEmpty() || !ui()->newPswEdit->text().isEmpty() || !ui()->confirmEdit->text().isEmpty())
        {

            QSettings settings("CentaurProject", "Centaur");
            settings.beginGroup("__Session__data");
            QString oldPassword = settings.value("__sec__").toString();
            settings.endGroup();

            if (oldPassword != QString::fromUtf8(QCryptographicHash::hash(QByteArrayView(ui()->oldPasswordEdit->text().toUtf8()), QCryptographicHash::RealSha3_512).toBase64()))
            {
                ui()->oldPswErrorLabel->setText(tr("not valid"));
                ui()->oldPswErrorLabel->show();
                return;
            }

            if (ui()->newPswEdit->text().isEmpty())
            {
                ui()->pswErrorLabel->setText(tr("Must type a password"));
                ui()->pswErrorLabel->show();
                return;
            }

            if ((ui()->newPswEdit->text() != ui()->confirmEdit->text()))
            {
                ui()->pswConfirmErrorLabel->setText(tr("Passwords don't match"));
                ui()->pswConfirmErrorLabel->show();
                return;
            }

            _impl->pswUpdate = true;
        }
    }

    if (ui()->newPswEdit->text().isEmpty() && !_impl->editingMode)
    {
        ui()->pswErrorLabel->setText(tr("Must type a password"));
        ui()->pswErrorLabel->show();
        return;
    }

    if ((ui()->newPswEdit->text() != ui()->confirmEdit->text()) && !_impl->editingMode)
    {
        ui()->pswConfirmErrorLabel->setText(tr("Passwords don't match"));
        ui()->pswConfirmErrorLabel->show();
        return;
    }

    if (ui()->nameEdit->text().isEmpty())
    {
        ui()->nameErrorLabel->setText(tr("Type a name"));
        ui()->nameErrorLabel->show();
        return;
    }

    if (ui()->mailEdit->text().isEmpty())
    {
        ui()->mailErrorLabel->setText("Type an e-mail");
        ui()->mailErrorLabel->show();
        return;
    }
    else
    {

        QRegularExpression mailRegex(R"(\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b)", QRegularExpression::PatternOption::CaseInsensitiveOption);

        QRegularExpressionMatch match = mailRegex.match(ui()->mailEdit->text());

        if (!match.hasMatch())
        {
            ui()->mailErrorLabel->setText(tr("Invalid e-mail"));
            ui()->mailErrorLabel->show();
            return;
        }
    }

    accept();
}

void AddUserDialog::searchImage() noexcept
{
    QFileDialog dlg(this, tr("Open File"));

    static bool firstDialog = true;

    if (firstDialog)
    {
        firstDialog                         = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dlg.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = QImageReader::supportedMimeTypes();
    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dlg.setMimeTypeFilters(mimeTypeFilters);
    dlg.selectMimeTypeFilter("image/jpeg");
    dlg.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);

    if (dlg.exec() == QDialog::Accepted)
    {
        const QString dataPath      = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        const QString imageFilePath = QString("%1/e30dfd91071c.image.data").arg(dataPath);

        if (QFile::exists(imageFilePath))
        {
            logInfo("addUser", "Internal image file removed.");
            QFile::remove(imageFilePath);
        }

        if (QFile::copy(dlg.selectedFiles().first(), imageFilePath))
        {
            logInfo("addUser", "Internal image file copied.");
            _impl->photoUpdate = true;
            if (_impl->photo.load(imageFilePath))
            {
                logInfo("addUser", "Internal image file valid and loaded.");
                ui()->photoLabel->setPixmap(QPixmap::fromImage(_impl->photo.scaled(ui()->photoLabel->size())));
            }
        }
    }
}

END_CENTAUR_NAMESPACE
