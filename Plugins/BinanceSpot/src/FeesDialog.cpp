/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "FeesDialog.hpp"
#include <QSettings>
#include <QStandardItem>

cen::TradeFeesDialog::TradeFeesDialog(const binapi::SpotTradingFees &fees, const QString &symbolFilter, QDialog *parent) :
    QDialog { parent },
    m_ui { new Ui::FeesDialog }
{
    m_ui->setupUi(this);

    restoreInterfaceState();

    connect(m_ui->closeButton, &QPushButton::released, this, [&]() {saveInterfaceState(); accept(); });

    m_ui->searchEdit->addAction(QIcon(":/img/glass"), QLineEdit::ActionPosition::LeadingPosition);
    m_ui->searchEdit->setPlaceholderText("Search...");

    m_searchModel       = new QStandardItemModel(0, 3, m_ui->tableView);
    auto sortProxyModel = new QSortFilterProxyModel(m_ui->tableView);
    sortProxyModel->setSourceModel(m_searchModel);
    m_ui->tableView->setModel(sortProxyModel);

    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(m_ui->searchEdit, &QLineEdit::textChanged, sortProxyModel, &QSortFilterProxyModel::setFilterFixedString);

    m_ui->tableView->sortByColumn(0, Qt::AscendingOrder);
    m_ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_searchModel->setHorizontalHeaderLabels({ "Symbol", "Maker Commission", "Taker Commission" });

    int row = 0;
    for (const auto &[sym, fee] : fees)
    {
        auto item  = new QStandardItem(QString::fromStdString(sym));
        auto taker = new QStandardItem(QString("%1 %%").arg(QLocale(QLocale::English).toString(fee.takerCommission * 100., 'f', 5)));
        auto maker = new QStandardItem(QString("%1 %%").arg(QLocale(QLocale::English).toString(fee.makerCommission * 100., 'f', 5)));

        m_searchModel->insertRow(row, { item, maker, taker });
        ++row;
    }

    m_ui->searchEdit->setText(symbolFilter);
}

cen::TradeFeesDialog::~TradeFeesDialog() = default;

void cen::TradeFeesDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("FeesDialog");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("tableColumns_geometry", m_ui->tableView->horizontalHeader()->saveGeometry());
    settings.setValue("tableColumns_state", m_ui->tableView->horizontalHeader()->saveState());
    settings.endGroup();
}

void cen::TradeFeesDialog::restoreInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("FeesDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->tableView->horizontalHeader()->restoreGeometry(settings.value("tableColumns_geometry").toByteArray());
    m_ui->tableView->horizontalHeader()->restoreState(settings.value("tableColumns_state").toByteArray());
    settings.endGroup();
}
