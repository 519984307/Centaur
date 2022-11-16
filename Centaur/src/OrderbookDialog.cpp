/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "OrderbookDialog.hpp"
#include "../ui/ui_OrderbookDialog.h"
#include <CentaurApp.hpp>
#include <QPainter>
#include <QSettings>
#include <QStyledItemDelegate>
#include <utility>

BEGIN_CENTAUR_NAMESPACE
namespace
{
    class ProgressDelegate : public QStyledItemDelegate
    {
    public:
        enum class Type
        {
            ask,
            bid
        };

    public:
        explicit ProgressDelegate(Type type, QObject *parent) :
            QStyledItemDelegate(parent),
            _type { type }
        {
            switch (_type)
            {
                case Type::ask:
                    redBrush = QBrush(QColor(197, 29, 7, 128));
                    break;
                case Type::bid:
                    greenBrush = QBrush(QColor(1, 185, 6, 128));
                    break;
            }
        }

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            if (index.column() == 2)
            {
                QStyleOptionViewItem options = option;

                initStyleOption(&options, index);

                painter->save();

                QString text = options.text;
                options.text = "";
                options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

                qreal min   = index.data(Qt::UserRole + 1).toReal();
                qreal max   = index.data(Qt::UserRole + 2).toReal();
                qreal value = index.data(Qt::UserRole + 3).toReal();

                qreal percent      = (value - min) / (max - min);
                qreal newLeft      = static_cast<qreal>(options.rect.right()) - (static_cast<qreal>(options.rect.right()) * percent);
                QRect progressRect = options.rect;
                progressRect.setLeft(static_cast<int>(newLeft));

                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(_type == Type::ask ? redBrush : greenBrush);
                painter->drawRect(progressRect);
                painter->restore();

                options.rect.setWidth(options.rect.width() - 10);
                options.widget->style()->drawItemText(
                    painter,
                    options.rect,
                    static_cast<int>(options.displayAlignment),
                    options.palette,
                    options.state & QStyle::State_Enabled,
                    text,
                    QPalette::Text);

                painter->restore();
            }
            else
                QStyledItemDelegate::paint(painter, option, index);
        }

    protected:
        Type _type;
        QBrush greenBrush;
        QBrush redBrush;
    };
} // namespace

struct OrderbookDialog::Impl
{
    inline Impl(QString sym, CENTAUR_PLUGIN_NAMESPACE::IExchange *exch) :
        ui { new Ui::OrderbookDialog },
        exchange { exch },
        symbol { std::move(sym) }
    {
        base  = exchange->getBaseFromSymbol(symbol);
        quote = exchange->getQuoteFromSymbol(symbol);
    }
    inline ~Impl() = default;

    std::unique_ptr<Ui::OrderbookDialog> ui;

    CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange;
    QString symbol;
    QString base;
    QString quote;
    QString source;

    double oldPrice { 0.0 };
};

OrderbookDialog::OrderbookDialog(QString symbol, CENTAUR_PLUGIN_NAMESPACE::IExchange *exchange, QWidget *parent) :
    QDialog { parent },
    _impl { new Impl(std::move(symbol), exchange) }
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

    ui()->asksTable->setHorizontalHeaderLabels({ QString(tr("Price (%1)")).arg(_impl->quote), QString(tr("Amount (%1)")).arg(_impl->base), QString(tr("Total (%1)")).arg(_impl->quote) });
    ui()->asksTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui()->asksTable->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui()->asksTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui()->asksTable->setItemDelegateForColumn(2, new ProgressDelegate(ProgressDelegate::Type::ask, this));

    ui()->bidsTable->setHorizontalHeaderLabels({ QString(tr("Price (%1)")).arg(_impl->quote), QString(tr("Amount (%1)")).arg(_impl->base), QString(tr("Total (%1)")).arg(_impl->quote) });
    ui()->bidsTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui()->bidsTable->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui()->bidsTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui()->bidsTable->setItemDelegateForColumn(2, new ProgressDelegate(ProgressDelegate::Type::bid, this));

    // This will connect our dialog with the emitter
    // clang-format off
    connect(exchange->getPluginObject(),
                SIGNAL(snOrderbookUpdate(QString,QString,quint64,QMap<qreal,QPair<qreal,qreal> >,QMap<qreal,QPair<qreal,qreal> >)),
                this,
                SLOT(onOrderbookUpdate(QString,QString,quint64,QMap<qreal,QPair<qreal,qreal> >,QMap<qreal,QPair<qreal,qreal> >))
            );
    // clang-format on

    ui()->symbolLabel->setText(QString("%1/%2").arg(_impl->base, _impl->quote));

    // LINK BOTH TABLES HEADER SIZES
    connect(ui()->asksTable->horizontalHeader(), &QHeaderView::sectionResized, this, [&](int index, C_UNUSED int oldSize, int newSize) {
        auto *otherHeader = ui()->bidsTable->horizontalHeader();
        if (otherHeader->sectionSize(index) != newSize)
            otherHeader->resizeSection(index, newSize);
    });
    connect(ui()->bidsTable->horizontalHeader(), &QHeaderView::sectionResized, this, [&](int index, C_UNUSED int oldSize, int newSize) {
        auto *otherHeader = ui()->asksTable->horizontalHeader();
        if (otherHeader->sectionSize(index) != newSize)
            otherHeader->resizeSection(index, newSize);
    });

    ui()->asksTable->sortByColumn(0, Qt::SortOrder::DescendingOrder);
    ui()->bidsTable->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    _impl->source = QString::fromStdString(exchange->getPluginUUID().to_string());

    restoreInterface();

    // Start receiving the information
    exchange->updateOrderbook(_impl->symbol);
}

OrderbookDialog::~OrderbookDialog() = default;

Ui::OrderbookDialog *OrderbookDialog::ui()
{
    return _impl->ui.get();
}

void OrderbookDialog::onCloseButton() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup(objectName());
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup(objectName() + "OrderbookAsksState");
    settings.setValue("geometry", ui()->asksTable->saveGeometry());
    settings.setValue("h-geometry", ui()->asksTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->asksTable->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup(objectName() + "OrderbookBidsState");
    settings.setValue("geometry", ui()->bidsTable->saveGeometry());
    settings.setValue("h-geometry", ui()->bidsTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->bidsTable->horizontalHeader()->saveState());
    settings.endGroup();

    _impl->exchange->stopOrderbook(_impl->symbol);

    emit closeButtonPressed();
}

void OrderbookDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup(objectName());
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();

    settings.beginGroup(objectName() + "OrderbookAsksState");
    ui()->asksTable->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->asksTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->asksTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup(objectName() + "OrderbookBidsState");
    ui()->bidsTable->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->bidsTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->bidsTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void OrderbookDialog::setPrice(double price)
{
    static constexpr char stylesheetRed[]   = { "QLabel{font-size: 14px; color: rgb(255, 129, 112);}" };
    static constexpr char stylesheetGreen[] = { "QLabel{font-size: 14px; color: rgb(0,255,0);}" };
    static constexpr char stylesheetGray[]  = { "QLabel{font-size: 14px; color: rgb(200,200,200);}" };
    if (price > _impl->oldPrice)
        ui()->priceLabel->setStyleSheet(stylesheetGreen);
    else if (price < _impl->oldPrice)
        ui()->priceLabel->setStyleSheet(stylesheetRed);
    else if (qFuzzyCompare(price, _impl->oldPrice))
        ui()->priceLabel->setStyleSheet(stylesheetGray);
    else
    {
        [[unlikely]];
    }

    ui()->priceLabel->setText("$ " + QLocale(QLocale::English).toString(price, 'f', 4));
    _impl->oldPrice = price;
}

void OrderbookDialog::onOrderbookUpdate(const QString &source, const QString &symbol, quint64 receivedTime, const QMap<qreal, QPair<qreal, qreal>> &bids, const QMap<qreal, QPair<qreal, qreal>> &asks) noexcept
{
    if (_impl->source != source || _impl->symbol != symbol)
        return;

    auto insertTable = [](const QString &text, QTableWidget *ui, int row, int col, int type) -> QTableWidgetItem * {
        QTableWidgetItem *item = ui->item(row, col);
        if (item == nullptr)
        {
            item = new QTableWidgetItem(text);
            switch (col)
            {
                case 1:
                    item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
                    break;
                case 2:
                    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                    break;
                case 0:
                default:
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                    if (type == 1)
                        item->setForeground(QBrush(QColor(0xFFA7AC)));
                    else
                        item->setForeground(QBrush(QColor(0x9CFFB4)));
                    break;
            }

            ui->setItem(row, col, item);
        }

        // Quite an expensive function
        item->setText(text);
        return item;
    };

    int nRowIndex;

    double maxAskItem = 0, minAskItem = 0, maxBidItem = 0, minBidItem = 0;
    auto maxAskItemIter = std::max_element(asks.begin(), asks.end(),
        [](const QPair<qreal, qreal> &p, const QPair<qreal, qreal> &d) -> bool {
            return p.first < d.first;
        });
    if (maxAskItemIter != asks.end())
        maxAskItem = maxAskItemIter->first;

    auto minAskItemIter = std::min_element(asks.begin(), asks.end(),
        [](const QPair<qreal, qreal> &p, const QPair<qreal, qreal> &d) -> bool {
            return p.first < d.first;
        });
    if (minAskItemIter != asks.end())
        minAskItem = minAskItemIter->first;

    auto maxBidItemIter = std::max_element(bids.begin(), bids.end(),
        [](const QPair<qreal, qreal> &p, const QPair<qreal, qreal> &d) -> bool {
            return p.first < d.first;
        });
    if (maxBidItemIter != bids.end())
        maxBidItem = maxBidItemIter->first;

    auto minBidItemIter = std::min_element(bids.begin(), bids.end(),
        [](const QPair<qreal, qreal> &p, const QPair<qreal, qreal> &d) -> bool {
            return p.first < d.first;
        });
    if (minBidItemIter != bids.end())
        minBidItem = minBidItemIter->first;

    ui()->asksTable->setRowCount(static_cast<int>(asks.size()));
    nRowIndex = 0;
    for (auto iter = asks.begin(); iter != asks.end(); ++iter)
    {
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.key(), 'f', 5)), ui()->asksTable, nRowIndex, 0, 1);
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().first, 'f', 5)), ui()->asksTable, nRowIndex, 1, 1);
        auto item = insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().second, 'f', 5)), ui()->asksTable, nRowIndex, 2, 1);

        item->setData(Qt::UserRole + 1, minAskItem);
        item->setData(Qt::UserRole + 2, maxAskItem);
        item->setData(Qt::UserRole + 3, iter.value().first);

        ++nRowIndex;
    }

    ui()->bidsTable->setRowCount(static_cast<int>(bids.size()));
    nRowIndex = 0;
    for (auto iter = bids.begin(); iter != bids.end(); ++iter)
    {
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.key(), 'f', 5)), ui()->bidsTable, nRowIndex, 0, 0);
        insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().first, 'f', 5)), ui()->bidsTable, nRowIndex, 1, 0);
        auto item = insertTable(QString("$ %1").arg(QLocale(QLocale::English).toString(iter.value().second, 'f', 5)), ui()->bidsTable, nRowIndex, 2, 0);

        item->setData(Qt::UserRole + 1, minBidItem);
        item->setData(Qt::UserRole + 2, maxBidItem);
        item->setData(Qt::UserRole + 3, iter.value().first);

        ++nRowIndex;
    }

    if (ui()->asksTable->rowCount())
        ui()->asksTable->scrollToItem(ui()->asksTable->item(ui()->asksTable->rowCount() - 1, 0));
    if (ui()->bidsTable->rowCount())
        ui()->bidsTable->scrollToItem(ui()->asksTable->item(0, 0));

    // Calculate latency
    const auto ms      = static_cast<quint64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    const auto latency = receivedTime > ms ? 0ull : ms - receivedTime;

    auto changeColor = [](QLabel *ctr, const QColor &color) {
        QPalette pal = ctr->palette();
        pal.setColor(ctr->foregroundRole(), color);
        ctr->setPalette(pal);
    };
    if (latency <= 85)
    {
        changeColor(ui()->latencyLabel, QColor(0, 255, 0));
    }
    else if (latency >= 86 && latency <= 185)
    {
        changeColor(ui()->latencyLabel, QColor(255, 215, 0));
    }
    else if (latency >= 186 && latency <= 10000000)
    {
        changeColor(ui()->latencyLabel, QColor(255, 129, 112));
    }

    ui()->latencyLabel->setText(QString("%1 ms").arg(latency));

    emit redirectOrderbook(asks, bids);
}

END_CENTAUR_NAMESPACE
