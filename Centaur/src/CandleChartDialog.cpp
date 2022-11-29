/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 24/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CandleChartDialog.hpp"
#include "../ui/ui_CandleChartDialog.h"
#include <QSettings>

BEGIN_CENTAUR_NAMESPACE

struct CandleChartDialog::Impl
{
    inline Impl(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf) :
        ui { new Ui::CandleChartDialog },
        timeframe { tf } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::CandleChartDialog> ui;

    CENTAUR_PLUGIN_NAMESPACE::TimeFrame timeframe;
    QString source;
    QString symbol;
};

CandleChartDialog::CandleChartDialog(CENTAUR_PLUGIN_NAMESPACE::TimeFrame tf, const QString &symbol, const QString &source, QWidget *parent) :
    QDialog { parent },
    _impl { new Impl(tf) }
{
    ui()->setupUi(this);

    _impl->symbol = symbol;
    _impl->source = source;

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    ui()->titleFrame->overrideParent(this);
    ui()->mainTableFrame->overrideParent(this);

    ui()->closeButton->setButtonClass(SystemPushButton::ButtonClass::override);
    ui()->minimizeButton->setButtonClass(SystemPushButton::ButtonClass::override);

    // Minimize will only hide the dialog
    connect(ui()->minimizeButton, &QPushButton::released, this, [&]() { hide(); });
    // Close will have special meaning
    connect(ui()->closeButton, &QPushButton::released, this, [&]() { onCloseButton(); });

    connect(ui()->onTopButton, &QPushButton::clicked, this, [&](bool checked) {
        this->setWindowFlag(Qt::WindowType::WindowStaysOnTopHint, checked);
        this->show();
    });

    restoreInterface();
}

CandleChartDialog::~CandleChartDialog() = default;

Ui::CandleChartDialog *CandleChartDialog::ui()
{
    return _impl->ui.get();
}

void CandleChartDialog::onCloseButton() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("CandleChartDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    emit closeButtonPressed();
}

void CandleChartDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("CandleChartDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

END_CENTAUR_NAMESPACE
