/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 24/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "TestDialog.hpp"
#include "CandleChartWidget.hpp"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpacerItem>
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
    auto timeTracker      = createButton("Time Tracker");
    auto horzGrid         = createButton("Horz Grid");
    auto vertGrid         = createButton("Vert Grid");
    auto magTime          = createButton("Mangt Time");

    hLayout->addWidget(hShowVert);
    hLayout->addWidget(hShowHorz);
    hLayout->addWidget(priceOrientation);
    hLayout->addWidget(priceTracker);
    hLayout->addWidget(timeTracker);
    hLayout->addWidget(horzGrid);
    hLayout->addWidget(vertGrid);
    hLayout->addWidget(magTime);

    layout->addWidget(widget);
    layout->addItem(hLayout);

    hShowVert->setChecked(widget->isVerticalLineVisible());
    hShowHorz->setChecked(widget->isHorizontalLineVisible());
    priceTracker->setChecked(widget->isPriceTrackerVisible());
    timeTracker->setChecked(widget->isTimeTrackerVisible());
    horzGrid->setChecked(widget->isHorizontalGridLineVisible());
    vertGrid->setChecked(widget->isVerticalGridLineVisible());
    magTime->setChecked(widget->isTimeCrosshairMagnetize());

    connect(hShowVert, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowVerticalLine);
    connect(hShowHorz, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowHorizontalLine);

    connect(priceOrientation, &QPushButton::clicked, widget, [&, widget](bool checked) {
        if (checked)
            widget->setPriceAxisOrientation(Qt::AlignLeft);
        else
            widget->setPriceAxisOrientation(Qt::AlignRight);
    });
    connect(priceTracker, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowPriceAxisTracker);
    connect(timeTracker, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowTimeAxisTracker);
    connect(horzGrid, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowHorizontalGridLines);
    connect(vertGrid, &QPushButton::clicked, widget, &cen::CandleChartWidget::onShowVerticalGridLines);
    connect(magTime, &QPushButton::clicked, widget, &cen::CandleChartWidget::onMagnetizeTimeCrosshair);

    widget->setAutoFillBackground(true);
    widget->scene()->setBackgroundBrush(QColor(255, 255, 255, 255));
    widget->setBackgroundBrush(QColor(0, 0, 0));
    widget->setChartTimeFrame(CENTAUR_PLUGIN_NAMESPACE::TimeFrame::Hours_1);

    widget->setLinkTimestamp(testData1h[testData1h.size() - 1].first);

    qreal minPrice = testData1h[testData1h.size() - 3].second.low;
    qreal maxPrice = 0;
    for (int i = 3; i < 400; ++i)
    {
        minPrice = std::min(minPrice, testData1h[testData1h.size() - i].second.low);
        maxPrice = std::max(maxPrice, testData1h[testData1h.size() - i].second.high);
        widget->addCandle(testData1h[testData1h.size() - i].first,
            testData1h[testData1h.size() - i].second.open,
            testData1h[testData1h.size() - i].second.close,
            testData1h[testData1h.size() - i].second.high,
            testData1h[testData1h.size() - i].second.low);
    }

    widget->onSetMinMaxPrice(5, 20);

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

    auto h2Layout = new QHBoxLayout;

    auto *spacingLabel = new QLabel("Candle Spacing", this);
    auto *widthLabel   = new QLabel("Candle Width", this);
    auto cdWidth       = new QDoubleSpinBox(this);
    auto cdSpacing     = new QDoubleSpinBox(this);

    cdWidth->setValue(5.0);
    cdWidth->setRange(2.0, 30.0);
    cdWidth->setSingleStep(0.5);

    cdSpacing->setValue(1.0);
    cdSpacing->setRange(0.0, 30.0);
    cdSpacing->setSingleStep(1.0);

    h2Layout->addWidget(widthLabel);
    h2Layout->addWidget(cdWidth);
    h2Layout->addWidget(spacingLabel);
    h2Layout->addWidget(cdSpacing);
    h2Layout->addSpacerItem(new QSpacerItem(241, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addItem(h2Layout);

    connect(cdSpacing, &QDoubleSpinBox::valueChanged, widget, &cen::CandleChartWidget::onChangeCandleSpacing);
    connect(cdWidth, &QDoubleSpinBox::valueChanged, widget, &cen::CandleChartWidget::onChangeCandleWidth);

    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("test_candle");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

TestDialog::~TestDialog() = default;
