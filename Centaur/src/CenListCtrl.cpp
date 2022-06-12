/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "CenListCtrl.hpp"
#include "CentaurApp.hpp"
#include <QMenu>
#include <QMouseEvent>

CENTAUR_NAMESPACE::CenListCtrl::CenListCtrl(QWidget *parent) :
    QTableView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &CENTAUR_NAMESPACE::CenListCtrl::customContextMenuRequested, this, &CENTAUR_NAMESPACE::CenListCtrl::showContextMenu);
}

void CENTAUR_NAMESPACE::CenListCtrl::setRemove(bool status)
{
    m_remove = status;
}

void CENTAUR_NAMESPACE::CenListCtrl::allowClickMessages(bool status)
{
    m_allowClickMessages = status;
}

void CENTAUR_NAMESPACE::CenListCtrl::showContextMenu(const QPoint &pos)
{
    QModelIndex index = indexAt(pos).siblingAtColumn(0);
    auto itemData     = index.data(Qt::DisplayRole).toString();

    if (!itemData.isEmpty())
    {
        QMenu contextMenu("Context menu", this);

        QAction action(
            !m_remove
                ? QString(tr("Add '%1' to the watchlist")).arg(itemData)
                : QString(tr("Remove '%1' from the watchlist")).arg(itemData),
            this);
        contextMenu.addAction(&action);
        QList<QAction *> dynActions;

        CENTAUR_PLUGIN_NAMESPACE::IExchange *exch;
        if (!m_remove)
        {
            connect(&action, &QAction::triggered, this,
                [&]() { emit snAddToWatchList(itemData, objectName(), true); });

            exch = g_app->exchangeFromWatchlistRow(objectName());
        }
        else
        {
            connect(&action, &QAction::triggered, this,
                [&]() { emit snRemoveWatchList(index.row()); });
            exch = g_app->exchangeFromWatchlistRow(index.row());
        }


        if (exch != nullptr)
        {
            contextMenu.addSeparator();
            auto list = exch->dynamicWatchListMenuItems();
            for (const auto &ls : list)
            {
                if (ls.second == uuid { "{00000000-0000-0000-0000-000000000000}" })
                    contextMenu.addSeparator();
                else
                {
                    dynActions.push_back(new QAction(ls.first));
                    auto r = exch->setDynamicWatchListMenuAction(dynActions.last(), itemData, ls.second);
                    if (!r)
                        dynActions.last()->setDisabled(true);
                    contextMenu.addAction(dynActions.last());
                }
            }
        }

        contextMenu.exec(mapToGlobal(pos));

        // Release memory
        for (const auto &actions : dynActions)
        {
            delete actions;
        }
    }
}

void CENTAUR_NAMESPACE::CenListCtrl::mousePressEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);
    if (!m_allowClickMessages)
        return;

    auto itemModel = indexAt(event->pos());

    if (itemModel.isValid())
    {
        QString source = itemModel.siblingAtColumn(4).data().toString();
        QString symbol = itemModel.siblingAtColumn(0).data().toString();
        emit snSetSelection(source, symbol);
    }
    else
    {
        emit snRemoveSelection();
    }
}
