/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 01/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "LongOperation.hpp"
#include "CentaurApp.hpp"

cen::LongOperation::LongOperation()
{
    m_operation = new LongOperationDialog(g_app);
}

cen::LongOperation::~LongOperation()
{
    delete m_operation;
}

void cen::LongOperation::show(const QString &message, const QIcon &icon, bool modal)
{
    m_operation->setModal(modal);
    m_operation->setParameters(message, icon);
    m_operation->show();
}

void cen::LongOperation::hide()
{
    m_operation->hide();
}

cen::LongOperationDialog::LongOperationDialog(QWidget *parent) :
    QDialog(parent),
    m_ui { new Ui::LongOperation }
{
    m_ui->setupUi(this);

    m_ui->progressBar->setRange(0, 0);
}

cen::LongOperationDialog::~LongOperationDialog()
{
}

void cen::LongOperationDialog::setParameters(const QString &message, const QIcon &icon)
{
    m_ui->textLabel->setText(message);
    if (!icon.isNull())
    {
        m_ui->iconLabel->setWindowIcon(icon);
        m_ui->iconLabel->show();
    }
    else
        m_ui->iconLabel->hide();
}
