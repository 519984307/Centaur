/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 22/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

// IStatus Implementation

#include "BinanceSPOT.hpp"
#include <QApplication>

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

QAction *cen::BinanceSpotPlugin::action(const QPoint &pt) noexcept
{
    return nullptr;
}
