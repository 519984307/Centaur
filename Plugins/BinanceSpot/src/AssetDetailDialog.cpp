/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "AssetDetailDialog.hpp"
#include <QSettings>

CENTAUR_NAMESPACE::AssetDetailDialog::AssetDetailDialog(const binapi::SPOT::AssetDetail &detail, const QString &asset, cen::interface::IConfiguration *config, QDialog *parent) :
    QDialog(parent),
    m_ui { new Ui::AssetDetailDialog }
{
    m_ui->setupUi(this);

    restoreInterfaceState();

    auto setReadOnly = [&](QCheckBox *checkBox) -> void {
        checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        checkBox->setFocusPolicy(Qt::NoFocus);
    };

    connect(m_ui->btnClose, &QPushButton::released, this, [&]() { saveInterfaceState(); accept(); });

    setReadOnly(m_ui->checkBoxDeposit);
    setReadOnly(m_ui->checkBoxWithdraw);

    QPixmap img;
    if (config->getSymbolImage(64, 0, asset.toUpper(), &img))
        m_ui->image->setPixmap(img);
    m_ui->coinName->setText(asset);

    m_ui->checkBoxDeposit->setChecked(detail.depositStatus);
    m_ui->checkBoxWithdraw->setChecked(detail.withdrawStatus);

    m_ui->textEditTip->setText(QString::fromStdString(detail.depositTip));

    m_ui->lineEditFee->setText(QString("$ %1").arg(QLocale(QLocale::English).toString(detail.withdrawFee, 'f', 5)));
    m_ui->lineEditMinimum->setText(QString("$ %1").arg(QLocale(QLocale::English).toString(detail.minWithdrawAmount, 'f', 5)));
}

void CENTAUR_NAMESPACE::AssetDetailDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("AssetDetailDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void CENTAUR_NAMESPACE::AssetDetailDialog::restoreInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("AssetDetailDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

CENTAUR_NAMESPACE::AssetDetailDialog::~AssetDetailDialog() = default;
