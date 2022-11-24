/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 22/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

// IStatus Implementation

#include "BinanceSPOT.hpp"
#include "StatusDialog.hpp"
#include <QApplication>
#include <QMessageBox>

cen::plugin::IStatus::DisplayMode cen::BinanceSpotPlugin::initialize() noexcept
{
    m_image.load(":/status/icon");
    return DisplayMode::TextIcon;
}

QString cen::BinanceSpotPlugin::text() noexcept
{
    return "SPOT";
}
QPixmap cen::BinanceSpotPlugin::image() noexcept
{
    return m_image;
}
QFont cen::BinanceSpotPlugin::font() noexcept
{
    return QApplication::font();
}

QBrush cen::BinanceSpotPlugin::brush(cen::plugin::IStatus::DisplayRole role) noexcept
{
    switch (role)
    {
        case DisplayRole::Icon: C_FALLTHROUGH;
        case DisplayRole::Text: C_FALLTHROUGH;
        case DisplayRole::Font: C_FALLTHROUGH;
        case DisplayRole::Background:
            return Qt::NoBrush;
        case DisplayRole::Foreground:
            return QBrush(QColor(255, 5, 0));
    }
}

QAction *cen::BinanceSpotPlugin::action() noexcept
{
    return m_statusAction;
}

void cen::BinanceSpotPlugin::onStatusButtonClicked(C_UNUSED bool status) noexcept
{
    try
    {
        auto permissions = getAPI()->getAPIKeyPermission();
        auto status      = getAPI()->accountAPITradingStatus();
        StatusDialog dlg(
            &permissions,
            &status,
            tr("Account status available"),
            nullptr);
        dlg.exec();

    } catch (C_UNUSED const std::exception &ex)
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("API trading status is not available"), QMessageBox::Ok);
    }
}
