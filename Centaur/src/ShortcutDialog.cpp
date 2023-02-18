/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "ShortcutDialog.hpp"
#include "../ui/ui_ShortcutDialog.h"
#include <QSettings>

BEGIN_CENTAUR_NAMESPACE

struct ShortcutDialog::Impl
{
    inline Impl() :
        ui { new Ui::ShortcutDialog } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::ShortcutDialog> ui;
};

ShortcutDialog::ShortcutDialog(QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);

    ui()->titleFrame->overrideParent(this);

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    ui()->closeButton->setButtonClass(SystemPushButton::ButtonClass::override);
    connect(ui()->closeButton, &SystemPushButton::systemPressed, this, [&]() {
        reject();
    });

    connect(ui()->acceptButton, &QPushButton::released, this, &ShortcutDialog::onAccept);
    connect(ui()->cancelButton, &QPushButton::released, this, &ShortcutDialog::reject);

    ui()->keySequenceEdit->setClearButtonEnabled(true);

    restoreInterface();

    ui()->keySequenceEdit->setFocus();
}

ShortcutDialog::~ShortcutDialog() = default;

Ui::ShortcutDialog *ShortcutDialog::ui()
{
    return _impl->ui.get();
}

void ShortcutDialog::onAccept() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("ShortcutDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    accept();
}

void ShortcutDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("ShortcutDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void ShortcutDialog::setShortcutInformation(const QString &text, const QString &shortcut) noexcept
{
    ui()->label->setText(text);
    ui()->keySequenceEdit->setKeySequence(shortcut);
}

QKeySequence ShortcutDialog::getShortcut() noexcept
{
    return ui()->keySequenceEdit->keySequence();
}

END_CENTAUR_NAMESPACE
