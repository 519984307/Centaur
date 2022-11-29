/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 12/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "SplashDialog.hpp"
#include "../ui/ui_SplashDialog.h"
#include <QScreen>
#include <QSettings>

BEGIN_CENTAUR_NAMESPACE

struct SplashDialog::Impl
{
    inline Impl() :
        ui { new Ui::SplashDialog } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::SplashDialog> ui;
};

SplashDialog::SplashDialog(QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);
    ui()->logMainFrame->overrideParent(this);

    // Center the dialog
    QRect screenGeometry = QGuiApplication::screens()[0]->geometry();

    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif
}

SplashDialog::~SplashDialog() = default;

Ui::SplashDialog *SplashDialog::ui()
{
    return _impl->ui.get();
}

/**
 * Since this dialog is only intended to be shown in the startup of the code
 * QCoreApplication::processEvents();
 * is mandatory to display all changes
 */

void SplashDialog::setDisplayText(const QString &text)
{
    ui()->displayText->setText(text);
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
}

void SplashDialog::setProgressRange(int min, int max)
{
    ui()->progressBar->setRange(min, max);
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
}

void SplashDialog::setProgressPos(int pos)
{
    ui()->progressBar->setValue(pos);
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
}

void SplashDialog::step()
{
    auto pos = ui()->progressBar->value();
    setProgressPos(pos + 1);
}

std::pair<int, int> SplashDialog::getProgressRange()
{
    return { ui()->progressBar->minimum(),
        ui()->progressBar->maximum() };
}

END_CENTAUR_NAMESPACE
