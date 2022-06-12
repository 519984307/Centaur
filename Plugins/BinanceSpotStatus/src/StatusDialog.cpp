/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "StatusDialog.hpp"
#include <QDateTime>

cen::StatusDialog::StatusDialog(binapi::SPOT::APIKeyPermissions *apiKeyPermissions, binapi::SPOT::AccountAPITradingStatus *apiTradingStatus, const QString &status, QWidget *parent) :
    QDialog(parent),
    m_ui { new Ui::StatusDialog }
{
    m_ui->setupUi(this);

    auto setReadOnly = [&](QCheckBox *checkBox) -> void {
        checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        checkBox->setFocusPolicy(Qt::NoFocus);
    };

    connect(m_ui->btnClose, &QPushButton::released, this, [&]() { accept(); });

    setReadOnly(m_ui->checkBoxLocked);
    setReadOnly(m_ui->checkBoxIPRestrict);
    setReadOnly(m_ui->checkBoxWithdrawals);
    setReadOnly(m_ui->checkBoxInternalTransfer);
    setReadOnly(m_ui->checkBoxUniversalTransfer);
    setReadOnly(m_ui->checkBoxVanillaOptions);
    setReadOnly(m_ui->checkBoxReading);
    setReadOnly(m_ui->checkBoxFutures);
    setReadOnly(m_ui->checkBoxMargin);
    setReadOnly(m_ui->checkBoxSpotAndMargin);

    QPalette pal = m_ui->lineEditStatus->palette();
    if (status == "BinanceSPOT Account Status: Normal")
        pal.setColor(QPalette::Text, QColor(0, 104, 18));
    else
        pal.setColor(QPalette::Text, QColor(172, 6, 0));
    m_ui->lineEditStatus->setPalette(pal);
    m_ui->lineEditStatus->setText(status);

    if( apiTradingStatus->updateTime > 0)
    {
        QDateTime updateTime;
        updateTime.setMSecsSinceEpoch(static_cast<qint64>(apiTradingStatus->updateTime));
        m_ui->lineEditUpdateTime->setText(updateTime.toString("dd-MM-yyyy hh:mm:ss.zzz"));
    }
    else
        m_ui->lineEditUpdateTime->setText("No update time");

    m_ui->checkBoxLocked->setChecked(apiTradingStatus->isLocked);
    if (apiTradingStatus->recoverTime > 0)
    {
        QDateTime recoverTime;
        recoverTime.setMSecsSinceEpoch(static_cast<qint64>(apiTradingStatus->recoverTime));
        m_ui->lineEditRecoverTime->setText(recoverTime.toString("dd-MM-yyyy hh:mm:ss.zzz"));
    }
    else
        m_ui->lineEditRecoverTime->setText("No recover time");

    m_ui->lineEditGCR->setText(QString("%1").arg(apiTradingStatus->triggerGCR));
    m_ui->lineEditIFER->setText(QString("%1").arg(apiTradingStatus->triggerIFER));
    m_ui->lineEditUFR->setText(QString("%1").arg(apiTradingStatus->triggerUFR));

    QDateTime creationTime;
    creationTime.setMSecsSinceEpoch(static_cast<qint64>(apiKeyPermissions->createTime));
    m_ui->lineEditCreationTime->setText(creationTime.toString("dd-MM-yyyy hh:mm:ss.zzz"));
    m_ui->checkBoxIPRestrict->setChecked(apiKeyPermissions->ipRestrict);
    m_ui->checkBoxWithdrawals->setChecked(apiKeyPermissions->enableWithdrawals);
    m_ui->checkBoxInternalTransfer->setChecked(apiKeyPermissions->enableInternalTransfer);
    m_ui->checkBoxUniversalTransfer->setChecked(apiKeyPermissions->permitsUniversalTransfer);
    m_ui->checkBoxVanillaOptions->setChecked(apiKeyPermissions->enableVanillaOptions);
    m_ui->checkBoxReading->setChecked(apiKeyPermissions->enableReading);
    m_ui->checkBoxFutures->setChecked(apiKeyPermissions->enableFutures);
    m_ui->checkBoxMargin->setChecked(apiKeyPermissions->enableMargin);
    m_ui->checkBoxSpotAndMargin->setChecked(apiKeyPermissions->enableSpotAndMarginTrading);
    if (apiKeyPermissions->tradingAuthorityExpirationTime > 0)
    {
        QDateTime expirationTime;
        expirationTime.setMSecsSinceEpoch(static_cast<qint64>(apiKeyPermissions->tradingAuthorityExpirationTime));
        m_ui->labelTradingAuthority->setText(expirationTime.toString("dd-MM-yyyy hh:mm:ss.zzz"));
    }
    else
        m_ui->labelTradingAuthority->setText("No expiration time");
}

cen::StatusDialog::~StatusDialog() = default;
