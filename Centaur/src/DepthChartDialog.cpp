/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "DepthChartDialog.hpp"
#include "../ui/ui_DepthChartDialog.h"
#include <QAreaSeries>
#include <QSettings>
#include <QSplineSeries>
#include <QValueAxis>
BEGIN_CENTAUR_NAMESPACE

struct DepthChartDialog::Impl
{
    inline Impl() :
        ui { new Ui::DepthChartDialog } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::DepthChartDialog> ui;

    // Plotting
    QSplineSeries *asksDepth { nullptr };
    QSplineSeries *asksDepthFill { nullptr };
    QSplineSeries *bidsDepth { nullptr };
    QSplineSeries *bidsDepthFill { nullptr };

    double price { -1.0 };
};

DepthChartDialog::DepthChartDialog(const QString &symbol, QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);

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

    ui()->symbolLabel->setText(symbol);

    ui()->depthChartView->setRenderHint(QPainter::Antialiasing);
    _impl->bidsDepth = new QSplineSeries;
    _impl->bidsDepth->setUseOpenGL(true);
    _impl->bidsDepthFill = new QSplineSeries;
    _impl->bidsDepthFill->setUseOpenGL(true);

    _impl->asksDepth = new QSplineSeries;
    _impl->asksDepth->setUseOpenGL(true);
    _impl->asksDepthFill = new QSplineSeries;
    _impl->asksDepthFill->setUseOpenGL(true);

    auto bidsAreaSeries = new QAreaSeries(_impl->bidsDepth, _impl->bidsDepthFill);
    bidsAreaSeries->setPen(Qt::NoPen);
    bidsAreaSeries->setColor(QColor(0, 255, 0, 125));

    auto asksAreaSeries = new QAreaSeries(_impl->asksDepth, _impl->asksDepthFill);
    asksAreaSeries->setPen(Qt::NoPen);
    asksAreaSeries->setColor(QColor(255, 0, 0, 125));

    auto depthChart = new QChart;
    depthChart->legend()->hide();
    depthChart->addSeries(asksAreaSeries);
    depthChart->addSeries(bidsAreaSeries);
    depthChart->createDefaultAxes();
    depthChart->setBackgroundBrush(Qt::NoBrush);

    depthChart->createDefaultAxes();

    auto yAxisAsksDefault = qobject_cast<QValueAxis *>(depthChart->axes(Qt::Vertical).first());
    depthChart->removeAxis(yAxisAsksDefault);

    auto yAxisAsks = new QValueAxis(this);
    depthChart->addAxis(yAxisAsks, Qt::AlignRight);
    asksAreaSeries->attachAxis(yAxisAsks);
    bidsAreaSeries->attachAxis(yAxisAsks);

    auto xAxisAsks = qobject_cast<QValueAxis *>(depthChart->axes(Qt::Horizontal).first());
    xAxisAsks->setLinePen(QPen(QBrush(QColor(80, 80, 80)), 2));
    xAxisAsks->setLabelsColor(QColor(200, 200, 200));
    xAxisAsks->setGridLineVisible(false);
    xAxisAsks->setLabelsColor(QColor(200, 200, 200));
    yAxisAsks->setGridLineVisible(false);
    yAxisAsks->setLinePen(QPen(QBrush(QColor(80, 80, 80)), 2));
    yAxisAsks->setLabelsColor(QColor(200, 200, 200));

    depthChart->setMargins(QMargins(0, 0, 0, 0));

    ui()->depthChartView->setChart(depthChart);

    restoreInterface();
}

DepthChartDialog::~DepthChartDialog() = default;

Ui::DepthChartDialog *DepthChartDialog::ui()
{
    return _impl->ui.get();
}

void DepthChartDialog::onCloseButton() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("DepthChartDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    emit closeButtonPressed();
}

void DepthChartDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("DepthChartDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void DepthChartDialog::setPrice(double price)
{
    _impl->price = price;
}
void DepthChartDialog::onOrderBookDepth(const QMap<qreal, QPair<qreal, qreal>> &asks, const QMap<qreal, QPair<qreal, qreal>> &bids) noexcept
{
    if (_impl->price < 0)
        return; // Not initialized price

    if (asks.empty() && bids.empty())
        return;

    auto yAxis = qobject_cast<QValueAxis *>(ui()->depthChartView->chart()->axes(Qt::Vertical).first());
    auto xAxis = qobject_cast<QValueAxis *>(ui()->depthChartView->chart()->axes(Qt::Horizontal).first());

    // Calculate the steps in the range
    // We need to traverse the bids map, backwards
    // this conversion will make the task easier, however, this conversion is not free
    auto stlMap = bids.toStdMap();

    // Determinate the min and max of each ask and bids list
    double asksMin = 0.0, asksMax = 0.0;
    if (!asks.empty())
    {
        // Of these we can be sure because it is a sorted map
        asksMin = asks.firstKey();
        asksMax = asks.lastKey();
    }
    double bidsMin = 0.0, bidsMax = 0.0;
    if (!bids.empty())
    {
        // Of these we can be sure because it is a sorted map
        bidsMin = bids.firstKey();
        bidsMax = bids.lastKey();
    }

    double minY;
    // Find the lowest point of quants
    if (asks.empty() && !bids.empty())
        minY = stlMap.rbegin()->second.first;
    else if (!asks.empty() && bids.empty())
        minY = asks.begin().value().first;
    else
        minY = std::min(stlMap.rbegin()->second.first, asks.begin().value().first);

    // Calculate the step from asks and bids list min and max
    const double askStep = (asksMax - asksMin) / (asks.empty() || asks.size() == 1 ? 1 : static_cast<double>(asks.size() - 1));
    const double bidStep = (bidsMax - bidsMin) / (bids.empty() || bids.size() == 1 ? 1 : static_cast<double>(bids.size() - 1));

    // Actually determinate from askStep and bidStep, however, the step will not be higher than 1.0
    const double step = std::min({ 1.0, askStep, bidStep });

    double maxY = 0.0;

    // Generate the asks points
    double prevAsksQuant   = 0.0;
    int currentIntStep     = 1;
    auto currentDoubleStep = static_cast<double>(currentIntStep);

    _impl->asksDepthFill->clear();
    _impl->asksDepth->clear();

    double priceMaxDeviation = step * currentDoubleStep;
    double prevY             = minY;

    *_impl->asksDepth << QPointF { _impl->price, minY };
    *_impl->asksDepthFill << QPointF { _impl->price, minY };

    int maxAskStep;
    double maxAskQuant = 0.0;
    for (auto iter = asks.begin(); iter != asks.end(); ++iter)
    {
        const double price = iter.key();
        const double quant = iter.value().first;

        if (price >= _impl->price + (step * currentDoubleStep))
        {
            // Make a scaffold effect
            *_impl->asksDepth << QPointF { _impl->price + (step * currentDoubleStep), prevY };
            *_impl->asksDepthFill << QPointF { _impl->price + (step * currentDoubleStep), minY };
            // Add a point
            *_impl->asksDepth << QPointF { _impl->price + (step * currentDoubleStep), prevAsksQuant };
            *_impl->asksDepthFill << QPointF { _impl->price + (step * currentDoubleStep), minY }; // Always the lowest Y point is the point with lower quantity
                                                                                                  // and because we are adding quants, the lowest point is minY
            maxAskQuant       = prevAsksQuant;
            prevY             = prevAsksQuant;
            maxY              = std::max(maxY, prevAsksQuant);
            priceMaxDeviation = std::max(priceMaxDeviation, step * currentDoubleStep);

            ++currentIntStep;
            currentDoubleStep = static_cast<double>(currentIntStep); // By personal experience, this conversion, prevents weird things in the rounding of floating points
        }

        prevAsksQuant += quant;
    }

    maxAskStep = currentIntStep;

    double prevBidsQuant = 0.0;
    currentIntStep       = 1;
    currentDoubleStep    = static_cast<double>(currentIntStep);

    _impl->bidsDepthFill->clear();
    _impl->bidsDepth->clear();

    prevY = minY;
    *_impl->bidsDepth << QPointF { _impl->price, minY };
    *_impl->bidsDepthFill << QPointF { _impl->price, minY };
    int maxBidStep     = 0.0;
    double maxBidQuant = 0.0;
    for (auto iter = stlMap.rbegin(); iter != stlMap.rend(); ++iter)
    {
        const auto &[price, data] = *iter;
        const auto quant          = data.first;

        if (price <= _impl->price - (step * currentDoubleStep)) // Now we go towards lower prices, this we know, because we have the reverse iterator in a sorted map
        {
            // Make a scaffold effect
            *_impl->bidsDepth << QPointF { _impl->price - (step * currentDoubleStep), prevY };
            *_impl->bidsDepthFill << QPointF { _impl->price - (step * currentDoubleStep), minY };

            *_impl->bidsDepth << QPointF { _impl->price - (step * currentDoubleStep), prevBidsQuant };
            *_impl->bidsDepthFill << QPointF { _impl->price - (step * currentDoubleStep), minY }; // Always the lowest Y point is the point with lower quantity
                                                                                                  // and because we are adding quants, the lowest point is minY

            maxBidQuant       = prevBidsQuant;
            prevY             = prevBidsQuant;
            maxY              = std::max(maxY, prevBidsQuant); // keep looking for the max point
            priceMaxDeviation = std::max(priceMaxDeviation, step * currentDoubleStep);

            ++currentIntStep;
            currentDoubleStep = static_cast<double>(currentIntStep);
        }

        prevBidsQuant += quant;
    }

    maxBidStep = currentIntStep;

    // There are cases where are more asks than bids and the number of steps are not the same
    // this make the graphs even on the lower scale on bids and the upper scale on asks
    if (maxAskStep > maxBidStep)
    {
        *_impl->bidsDepth << QPointF { _impl->price - (step * static_cast<double>(maxAskStep)), maxBidQuant };
        *_impl->bidsDepthFill << QPointF { _impl->price - (step * static_cast<double>(maxAskStep)), minY };
    }
    else
    {
        *_impl->asksDepth << QPointF { _impl->price + (step * static_cast<double>(maxBidStep)), maxAskQuant };
        *_impl->asksDepthFill << QPointF { _impl->price + (step * static_cast<double>(maxBidStep)), minY };
    }

    const double minX = _impl->price - priceMaxDeviation;
    const double maxX = _impl->price + priceMaxDeviation;

    yAxis->setRange(minY, maxY);
    xAxis->setRange(minX, maxX);
}

END_CENTAUR_NAMESPACE
