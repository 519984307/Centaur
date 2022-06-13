/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 04/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "CoinInfoDialog.hpp"
#include <QMessageBox>
#include <QSettings>

namespace
{
    constexpr int withdrawDescRole        = Qt::UserRole + 1;
    constexpr int depositDescRole         = Qt::UserRole + 2;
    constexpr int specialTipsRole         = Qt::UserRole + 3;
    constexpr int specialWithdrawTipsRole = Qt::UserRole + 4;
} // namespace

CENTAUR_NAMESPACE::CoinInfoDialog::CoinInfoDialog(BINAPI_NAMESPACE::AllCoinsInformation *data, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, const QString &filter, QWidget *parent) :
    QDialog(parent),
    m_ui { new Ui::CoinInfoDialog },
    m_info { data },
    m_config { config }
{
    m_ui->setupUi(this);

    m_ui->searchEdit->addAction(QIcon(":/img/glass"), QLineEdit::ActionPosition::LeadingPosition);
    m_ui->searchEdit->setPlaceholderText("Search...");

    m_ui->tableView->initialize(m_ui->searchEdit, 14, -1, 13, 0);
    connect(m_ui->tableView, &OptionsTableWidget::viewItem, this, &CoinInfoDialog::onViewItem);

    restoreInterfaceState();

    m_ui->tableView->getModel()->setHorizontalHeaderLabels({ "Coin",
        "Name",
        "Free",
        "Locked",
        "Freeze",
        "Storage",
        "Withdrawing",
        "IPOable",
        "IPOing",
        "Legal Money",
        "Trading",
        "Withdrawing All Enable",
        "Deposit All Enable",
        "Network List" });

    m_ui->tableView->getModel()->horizontalHeaderItem(0)->setFont(QFont("Roboto", 13));
    m_ui->tableView->getModel()->horizontalHeaderItem(1)->setFont(QFont("Roboto", 13));
    m_ui->tableView->getModel()->horizontalHeaderItem(2)->setFont(QFont("Roboto", 13));
    m_ui->tableView->getModel()->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignCenter);
    m_ui->tableView->getModel()->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);
    m_ui->tableView->setColumnWidth(1, 250);
    m_ui->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    m_ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);

    m_ui->tableView->setDeletionColumnData(0);
    m_ui->tableView->allowDeletionWhenEditorIsClear(false);
    m_ui->tableView->setButtons(OptionsWidget::Buttons::view);

    m_ui->closeButton->setText("Close");

    connect(m_ui->closeButton, &QPushButton::released, this, [&]() {saveInterfaceState(); accept(); });

    int curRow = 0;
    for (const auto &[coin, data] : *m_info)
    {
        auto coinItem            = new QStandardItem(coin.c_str());
        auto coinName            = new QStandardItem(data.name.c_str());
        auto coinFree            = new QStandardItem(QString("$ %1").arg(QLocale(QLocale::English).toString(data.free, 'f', 5)));
        auto coinLocked          = new QStandardItem(QString("$ %1").arg(QLocale(QLocale::English).toString(data.locked, 'f', 5)));
        auto coinFreeze          = new QStandardItem(QString("$ %1").arg(QLocale(QLocale::English).toString(data.freeze, 'f', 5)));
        auto coinStorage         = new QStandardItem(QString("$ %1").arg(QLocale(QLocale::English).toString(data.storage, 'f', 5)));
        auto coinWithdrawing     = new QStandardItem(QString("$ %1").arg(QLocale(QLocale::English).toString(data.withdrawing, 'f', 5)));
        auto coinIPOable         = new QStandardItem(QString("$ %1").arg(QLocale(QLocale::English).toString(data.ipoable, 'f', 5)));
        auto coinIPOing          = new QStandardItem(QString("$ %1").arg(QLocale(QLocale::English).toString(data.ipoing, 'f', 5)));
        auto coinLegalMoney      = new QStandardItem(QString("%1").arg(data.isLegalMoney ? "Yes" : "No"));
        auto coinTrading         = new QStandardItem(QString("%1").arg(data.trading ? "Yes" : "No"));
        auto coinWithdrawingAll  = new QStandardItem(QString("%1").arg(data.withdrawAllEnable ? "Yes" : "No"));
        auto coinDeposit         = new QStandardItem(QString("%1").arg(data.depositAlLEnable ? "Yes" : "No"));
        auto coinViewNetworkList = new QStandardItem("");

        QPixmap img;
        if (m_config->getSymbolImage(16, 0, coinItem->text().toUpper(), &img))
            coinItem->setIcon(img);

        m_ui->tableView->insertRowWithOptions(curRow, { coinItem, coinName, coinFree, coinLocked, coinFreeze, coinStorage, coinWithdrawing, coinIPOable, coinIPOing, coinLegalMoney, coinTrading, coinWithdrawingAll, coinDeposit, coinViewNetworkList }, false);

        ++curRow;
    }

    if (!filter.isEmpty())
        m_ui->searchEdit->setText(filter);
}

void CENTAUR_NAMESPACE::CoinInfoDialog::onViewItem(QStandardItem *item) noexcept
{
    auto coinName = item->data(Qt::DisplayRole).toString();
    if (coinName.isEmpty())
    {
        QMessageBox::information(this, "Error", "Invalid coin name", QMessageBox::Ok);
        return;
    }

    auto infoData = m_info->find(coinName.toStdString());
    if (infoData == m_info->end())
    {
        QMessageBox::information(this, "Error", QString("Not available data for '%1'").arg(coinName), QMessageBox::Ok);
        return;
    }

    NetworkListDialog dlg(infoData->second.name.c_str(), infoData->second.coin.c_str(), &infoData->second.networkList, m_config, this);
    dlg.setModal(true);
    dlg.exec();
}

void CENTAUR_NAMESPACE::CoinInfoDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("CoinInfoDialog");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("tableColumns_geometry", m_ui->tableView->horizontalHeader()->saveGeometry());
    settings.setValue("tableColumns_state", m_ui->tableView->horizontalHeader()->saveState());
    settings.endGroup();
}

void CENTAUR_NAMESPACE::CoinInfoDialog::restoreInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("CoinInfoDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->tableView->horizontalHeader()->restoreGeometry(settings.value("tableColumns_geometry").toByteArray());
    m_ui->tableView->horizontalHeader()->restoreState(settings.value("tableColumns_state").toByteArray());
    settings.endGroup();
}

CENTAUR_NAMESPACE::NetworkListDialog::NetworkListDialog(const QString &name, const QString &coin, std::vector<BINAPI_NAMESPACE::SPOT::CoinInformation::NetworkList> *data, CENTAUR_INTERFACE_NAMESPACE::IConfiguration *config, QWidget *parent) :
    QDialog(parent),
    m_ui { new Ui::NetworkListDialog },
    m_list { data },
    m_config { config }
{
    m_ui->setupUi(this);


    m_ui->coinName->setText(QString("%1 (%2)").arg(name, coin));
    m_ui->closeButton->setText("Close");

    m_ui->withdrawDesc->hide();
    m_ui->depositDesc->hide();
    m_ui->specialTips->hide();
    m_ui->specialWithdrawTips->hide();

    connect(m_ui->closeButton, &QPushButton::released, this, [&]() {saveInterfaceState(); accept(); });
    connect(m_ui->tableWidget, &QTableWidget::currentItemChanged, this, &NetworkListDialog::onCurrentItemChanged);

    QPixmap img;
    if (m_config->getSymbolImage(64, 0, coin.toUpper(), &img))
        m_ui->image->setPixmap(img);

    auto insertItem = [&table = m_ui->tableWidget](const QString &text, int row, int col) -> QTableWidgetItem * {
        auto item = new QTableWidgetItem(text);
        table->setItem(row, col, item);
        return item;
    };

    auto insertItemRole = [&table = m_ui->tableWidget](QTableWidgetItem *item, const QString &text, int role) {
        item->setData(role, text);
    };

    auto insertItemCurrency = [&table = m_ui->tableWidget, &insertItem](const BINAPI_NAMESPACE::currency_t val, int row, int col) {
        insertItem(QString("$ %1").arg(QLocale(QLocale::English).toString(val, 'f', 5)), row, col);
    };

    auto insertItemInt = [&table = m_ui->tableWidget, &insertItem](int64_t val, int row, int col) {
        insertItem(QString("%1").arg(QLocale(QLocale::English).toString(val)), row, col);
    };

    auto insertItemUint = [&table = m_ui->tableWidget, &insertItem](uint64_t val, int row, int col) {
        insertItem(QString("%1 min").arg(QLocale(QLocale::English).toString(val)), row, col);
    };

    auto insertItemBool = [&table = m_ui->tableWidget, &insertItem](bool val, int row, int col) {
        insertItem(val ? "Yes" : "No", row, col);
    };

    m_ui->tableWidget->setColumnCount(17);
    m_ui->tableWidget->setHorizontalHeaderLabels({ "Name",
        "Network",
        "Address RegEX",
        "Memo RegEx",
        "Address rule",
        "Withdraw Fee",
        "Withdraw Multiple",
        "Withdraw minimum",
        "Withdraw maximum",
        "Estimated arrival time",
        "Confirmations",
        "Unlock Confirmations",
        "Deposit enable",
        "Withdraw enable",
        "Default",
        "Reset address status",
        "Same Address?" });

    restoreInterfaceState();

    int rows = 0;
    m_ui->tableWidget->setRowCount(static_cast<int>(data->size()));
    for (const auto &item : *data)
    {
        auto rolesItem = insertItem(item.name.c_str(), rows, 0);
        insertItem(item.network.c_str(), rows, 1);
        insertItem(item.addressRegEx.c_str(), rows, 2);
        insertItem(item.memoRegEx.c_str(), rows, 3);
        insertItem(item.addressRule.c_str(), rows, 4);
        insertItemCurrency(item.withdrawFee, rows, 5);
        insertItemCurrency(item.withdrawIntegerMultiple, rows, 6);
        insertItemCurrency(item.withdrawMin, rows, 7);
        insertItemCurrency(item.withdrawMax, rows, 8);
        insertItemUint(item.estimatedArrivalTime, rows, 9);
        insertItemInt(item.minConfirmation, rows, 10);
        insertItemInt(item.unLockConfirm, rows, 11);
        insertItemBool(item.depositEnable, rows, 12);
        insertItemBool(item.withdrawEnable, rows, 13);
        insertItemBool(item.isDefault, rows, 14);
        insertItemBool(item.resetAddressStatus, rows, 15);
        insertItemBool(item.sameAddress, rows, 16);

        insertItemRole(rolesItem, item.withdrawDescription.c_str(), withdrawDescRole);
        insertItemRole(rolesItem, item.depositDescription.c_str(), depositDescRole);
        insertItemRole(rolesItem, item.specialTips.c_str(), specialTipsRole);
        insertItemRole(rolesItem, item.specialWithdrawTips.c_str(), specialWithdrawTipsRole);

        ++rows;
    }
}
void CENTAUR_NAMESPACE::NetworkListDialog::onCurrentItemChanged(QTableWidgetItem *current, C_UNUSED QTableWidgetItem *previous) noexcept
{
    const char format[]           = R"(<html><head/><body><p><span style=" font-weight:800; text-decoration: underline;">%1</span><span style=" font-size:14pt; font-weight:700;"/>%2</p></body></html>)";
    QTableWidgetItem *zeroColItem = nullptr;
    if (current->column() != 0)
    {
        int row     = current->row();
        zeroColItem = m_ui->tableWidget->item(row, 0);
    }
    else
        zeroColItem = current;

    if (zeroColItem == nullptr)
        return;

    const QString withdrawDesc = zeroColItem->data(withdrawDescRole).toString();
    if (!withdrawDesc.isEmpty())
    {
        m_ui->withdrawDesc->setText(QString(format).arg("Withdraw description:", withdrawDesc));
        m_ui->withdrawDesc->show();
    }
    else
        m_ui->withdrawDesc->hide();

    const QString depositDesc = zeroColItem->data(depositDescRole).toString();
    if (!depositDesc.isEmpty())
    {
        m_ui->depositDesc->setText(QString(format).arg("Deposit description:", depositDesc));
        m_ui->depositDesc->show();
    }
    else
        m_ui->depositDesc->hide();

    const QString specialTips = zeroColItem->data(specialTipsRole).toString();
    if (!specialTips.isEmpty())
    {
        m_ui->specialTips->setText(QString(format).arg("Special tips:", specialTips));
        m_ui->specialTips->show();
    }
    else
        m_ui->specialTips->hide();

    const QString specialWithdrawTips = zeroColItem->data(specialWithdrawTipsRole).toString();
    if (!specialWithdrawTips.isEmpty())
    {
        m_ui->specialWithdrawTips->setText(QString(format).arg("Special withdraw tips:", specialWithdrawTips));
        m_ui->specialWithdrawTips->show();
    }
    else
        m_ui->specialWithdrawTips->hide();
}

void CENTAUR_NAMESPACE::NetworkListDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("NetworkListDialog");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("tableColumns_geometry", m_ui->tableWidget->horizontalHeader()->saveGeometry());
    settings.setValue("tableColumns_state", m_ui->tableWidget->horizontalHeader()->saveState());
    settings.endGroup();
}

void CENTAUR_NAMESPACE::NetworkListDialog::restoreInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("NetworkListDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_ui->tableWidget->horizontalHeader()->restoreGeometry(settings.value("tableColumns_geometry").toByteArray());
    m_ui->tableWidget->horizontalHeader()->restoreState(settings.value("tableColumns_state").toByteArray());
    settings.endGroup();
}
