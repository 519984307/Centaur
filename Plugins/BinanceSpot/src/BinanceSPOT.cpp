/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include <QObject>

#include "BinanceSPOT.hpp"

CENTAUR_NAMESPACE::BinanceSpotPlugin::BinanceSpotPlugin(QObject *parent) :
    QObject(parent),
    m_spotWS { nullptr }
{
    m_sevenDayLastUpdate = QDate::currentDate();
}

CENTAUR_NAMESPACE::BinanceSpotPlugin::~BinanceSpotPlugin()
{
    if (m_spotWSThread != nullptr && m_spotWSThread->joinable())
    {
        m_spotWS->terminate();
        m_spotWSThread->join();
    }
    m_spotWSThread.reset();
    m_spotWS.reset();
}

/*
bool CENTAUR_NAMESPACE::BinanceSpotPlugin::addMenuAction(QAction *action, const uuid &menuId) noexcept
{
    if (menuId == uuid { "{5445eaec-d22c-4204-b720-ab07a570ab2e}" })
    {
        connect(action, &QAction::triggered, this, &BinanceSpotPlugin::onSpotStatus);
        return true;
    }
    else if (menuId == uuid { "{394f3857-c797-49c0-9343-37e1e66e028a}" })
    {
        connect(action, &QAction::triggered, this, &BinanceSpotPlugin::onCoinInformation);
        return true;
    }
    else if (menuId == uuid { "{3cd3662b-a52f-4c3d-ab15-bde1c1962cb9}" })
    {
        connect(action, &QAction::triggered, this, [&]() { onShowFees(); });
        return true;
    }

    return false;
}*/
