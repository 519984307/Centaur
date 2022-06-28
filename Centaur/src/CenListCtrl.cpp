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
            if (m_remove)
            {
                // We are in the watchlist
                auto exchInfo = CentaurApp::pluginInformationFromBase(exch);
                auto pl       = g_app->getSupportedCandleViewPlugins(exchInfo);
                if (pl != nullptr)
                {
                    auto tfs = g_app->getCandleViewTimeframeSupport(pl);
                    if (tfs != std::nullopt)
                    {
                        auto timeframes = tfs->get();
                        auto candleMenu = new QMenu(LS("ui-docs-symbols-candleview"), &contextMenu);
                        if (!timeframes.empty())
                        {
                            contextMenu.addSeparator();
                            contextMenu.addMenu(candleMenu);

                            g_app->m_candleEmitter = { itemData, pl };
                        }

                        for (const auto &tf : timeframes)
                        {
                            switch (tf)
                            {
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::nullTime: candleMenu->addSeparator(); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_1: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aSeconds_1); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_5: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aSeconds_5); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_10: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aSeconds_10); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_30: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aSeconds_30); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Seconds_45: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aSeconds_45); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_1: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_1); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_2: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_2); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_3: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_3); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_5: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_5); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_10: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_10); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_15: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_15); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_30: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_30); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Minutes_45: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMinutes_45); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_1: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aHours_1); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_2: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aHours_2); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_4: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aHours_4); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_6: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aHours_6); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_8: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aHours_8); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Hours_12: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aHours_12); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_1: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aDays_1); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Days_3: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aDays_3); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Weeks_1: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aWeeks_1); break;
                                case CENTAUR_PLUGIN_NAMESPACE::ICandleView::TimeFrame::Months_1: candleMenu->addAction(g_app->m_candleViewTimeFrameActions->aMonths_1); break;
                            }
                        }
                    }
                }
            }

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

        // Release the emitter
        g_app->m_candleEmitter = {};
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
