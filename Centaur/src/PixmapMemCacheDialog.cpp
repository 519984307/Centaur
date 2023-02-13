/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 08/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "PixmapMemCacheDialog.hpp"
#include "../ui/ui_PixmapMemCacheDialog.h"
#include <QMessageBox>
#include <QSettings>

BEGIN_CENTAUR_NAMESPACE

struct PixmapMemCacheDialog::Impl
{
    inline Impl() :
        ui { new Ui::PixmapMemCacheDialog } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::PixmapMemCacheDialog> ui;
};

PixmapMemCacheDialog::PixmapMemCacheDialog(QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    connect(ui()->acceptButton, &QPushButton::released, this, &PixmapMemCacheDialog::onAccept);
    connect(ui()->cancelButton, &QPushButton::released, this, &PixmapMemCacheDialog::onCancel);

    connect(ui()->resetButton, &QPushButton::released, this, [&]() {
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("advancedSettings-PixmapCache");
        auto value = settings.value("default", -1).toInt();
        if (value == -1)
        {
            // Ideally, this situation must not arise because CentaurApp::loadInterfaceState() writes this value at startup
            value = 10240; // Qt documentation value
        }
        ui()->memUnits->setCurrentIndex(Kilo);
        ui()->memEdit->setText(QString("%1").arg(value));
        settings.endGroup();
    });

    ui()->memEdit->setValidator(new QDoubleValidator(0, 1000000000, 5, ui()->memEdit));

    restoreInterface();
}

PixmapMemCacheDialog::~PixmapMemCacheDialog() = default;

Ui::PixmapMemCacheDialog *PixmapMemCacheDialog::ui()
{
    return _impl->ui.get();
}

void PixmapMemCacheDialog::onAccept() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("PixmapMemCacheDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    double multiplier;
    auto index = ui()->memUnits->currentIndex();
    switch (index)
    {
        case Kilo:
            multiplier = 1'000;
            break;
        case Mega:
            multiplier = 1'000'000;
            break;
        case Giga:
            multiplier = 1'000'000'000;
            break;
        case bytes:
        default:
            multiplier = 1;
            break;
    }

    auto mem = ui()->memEdit->text().toDouble() * multiplier;

    if (mem > 1'073'741'824 || mem < 10'240'000) // 0x40000000
    {
        QMessageBox::information(this,
            tr("Memory size invalid"),
            tr("Memory size must be between 1 GB and 10 MB (1,073,741,824 bytes and 10'240'000 bytes)"),
            QMessageBox::Ok);
        return;
    }

    memory = static_cast<int>(mem);

    accept();
}

void PixmapMemCacheDialog::onCancel() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("PixmapMemCacheDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    reject();
}

void PixmapMemCacheDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("PixmapMemCacheDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void PixmapMemCacheDialog::setData(const QString &m, int unit) noexcept
{
    ui()->memUnits->setCurrentIndex(unit);
    ui()->memEdit->setText(m);
}

END_CENTAUR_NAMESPACE
