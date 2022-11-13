////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "QRCodeDialog.hpp"
#include "../ui/ui_QRCodeDialog.h"
#include <QSettings>
#include <qrencode.h>

BEGIN_CENTAUR_NAMESPACE

struct QRCodeDialog::Impl
{
    inline Impl() :
        ui { new Ui::QRCodeDialog } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::QRCodeDialog> ui;
};

QRCodeDialog::QRCodeDialog(const QString &mail, const QString &key, QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);
    connect(ui()->closeLabel, &QLabel::linkActivated, this, &QRCodeDialog::dlgLinkActivated);

    ui()->codeLabel->setText(key);

    const auto url = QString("otpauth://totp/%1?secret=%2&issuer=Centaur").arg(mail, key);

    auto code = QRcode_encodeString(url.toUtf8().constData(), 9, QR_ECLEVEL_H, QR_MODE_8, 1);
    if (code != nullptr)
    {
        const int height = code->width;
        QImage im(code->width, height, QImage::Format::Format_RGB32);
        for (int x = 0; x < code->width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                const int index = (y * height) + x;
                auto color      = code->data[index] & 0x1 ? QColor(Qt::black) : QColor(Qt::white);
                im.setPixelColor(x, y, color);
            }
        }

        ui()->qrCodeLabel->setPixmap(QPixmap::fromImage(
            im.scaled(156, 156, Qt::KeepAspectRatio), Qt::MonoOnly));

        QRcode_free(code);
    }

    restoreInterface();
}

QRCodeDialog::~QRCodeDialog() = default;

Ui::QRCodeDialog *QRCodeDialog::ui()
{
    return _impl->ui.get();
}

void QRCodeDialog::dlgLinkActivated(C_UNUSED const QString &link)
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("QRCodeDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    accept();
}

void QRCodeDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("QRCodeDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

END_CENTAUR_NAMESPACE
