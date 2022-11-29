/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 24/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "TestDialog.hpp"
#include "CandleChartWidget.hpp"
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

TestDialog::TestDialog(QWidget *parent) :
    QDialog(parent)
{
    auto *widget = new cen::CandleChartWidget(this);
    auto layout  = new QVBoxLayout(this);

    auto createButton = [&](const QString &name) -> QPushButton * {
        auto button = new QPushButton(name, this);
        button->setAutoDefault(false);
        button->setCheckable(true);
        return button;
    };

    auto hLayout          = new QHBoxLayout;
    auto hShowVert        = createButton("Show Vert Line");
    auto hShowHorz        = createButton("Show Horz Line");
    auto priceOrientation = createButton("P.A.O.R");
    auto priceTracker     = createButton("Price Tracker");

    hLayout->addWidget(hShowVert);
    hLayout->addWidget(hShowHorz);
    hLayout->addWidget(priceOrientation);
    hLayout->addWidget(priceTracker);

    layout->addWidget(widget);
    layout->addItem(hLayout);

    hShowVert->setChecked(widget->isVerticalLineVisible());
    hShowHorz->setChecked(widget->isHorizontalLineVisible());
    priceTracker->setChecked(widget->isPriceTrackerVisible());

    connect(hShowVert, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowVerticalLine);
    connect(hShowHorz, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowHorizontalLine);

    connect(priceOrientation, &QPushButton::clicked, widget, [&, widget](bool checked) {
        if (checked)
            widget->setPriceAxisOrientation(Qt::AlignLeft);
        else
            widget->setPriceAxisOrientation(Qt::AlignRight);
    });
    connect(priceTracker, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowAxisPriceTracker);

    widget->setAutoFillBackground(true);
    widget->scene()->setBackgroundBrush(QColor(255, 255, 255, 255));
    widget->setBackgroundBrush(QColor(0, 0, 0));
    widget->setChartTimeFrame(CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_4);

    widget->addCandle(testData[0].first, testData[0].second.open, testData[0].second.close, testData[0].second.high, testData[0].second.low);

    //   widget->addCandle(testData[1].first, testData[1].second.open, testData[1].second.close, testData[1].second.high, testData[1].second.low);
    // widget->addCandle(testData[1].first, testData[0].second.open, testData[0].second.close, testData[0].second.high, testData[0].second.low);
    //
    /*
        for (const auto &[ts, cd] : testData)
        {
            widget->addCandle(ts, cd.open, cd.close, cd.high, cd.low);
        }*/

    connect(this, &QDialog::rejected, this, [&]() {
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("test_candle");
        settings.setValue("geometry", saveGeometry());
        settings.endGroup();
    });
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("test_candle");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

TestDialog::~TestDialog() = default;
