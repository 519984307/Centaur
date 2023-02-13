/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include "../ui/ui_SettingsDialog.h"
#include "PixmapMemCacheDialog.hpp"
#include "SettingsDialog.hpp"

#include <QPixmapCache>
#include <QSettings>

namespace
{

    template <typename T>
    concept integer = std::numeric_limits<T>::is_integer;

    auto bytesSizeMaxUnit(integer auto bytes) -> int
    {
        decltype(bytes) mem = bytes / 1000;
        int maxUnit         = 0;
        while (mem && maxUnit < 4)
        {
            ++maxUnit;
            mem /= 1000;
        }

        return maxUnit;
    }

    auto bytesToString(integer auto bytes, int unit = -1, bool appendUnit = true) -> QString
    {
        switch (unit == -1 ? bytesSizeMaxUnit(bytes) : unit)
        {
            case 1:
                return QString("%1%2").arg(static_cast<qreal>(bytes) / 1'000., 0, 'f', 3).arg(appendUnit ? " KB" : "");
            case 2:
                return QString("%1%2").arg(static_cast<qreal>(bytes) / 1'000'000., 0, 'f', 4).arg(appendUnit ? " MB" : "");
            case 3:
                return QString("%1%2").arg(static_cast<qreal>(bytes) / 1'000'000'000., 0, 'f', 5).arg(appendUnit ? " GB" : "");
            case 0:
            default:
                return QString("%1%2").arg(bytes).arg(appendUnit ? " bytes" : "");
        }
    }
} // namespace

BEGIN_CENTAUR_NAMESPACE

SettingsDialog::AdvancedImpl::AdvancedImpl() :
    _pixmapCache { new QStandardItem(QIcon(":/ui/settings/advanced/table/image_cache"), tr("Image Cache Memory")) },
    _pixmapCacheValue { new QStandardItem() }
{
    _pixmapCache->setFlags(_pixmapCacheValue->flags() & ~Qt::ItemIsEditable);
    _pixmapCacheValue->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _pixmapCacheValue->setText(bytesToString(QPixmapCache::cacheLimit()));
}

void SettingsDialog::initAdvancedWidget() noexcept
{
    ui()->memTableView->initialize(ui()->editAdvanceSearch, 3, 0, 2, nullptr, 1);

    ui()->memTableView->sortByColumn(0, Qt::AscendingOrder);
    ui()->memTableView->getModel()->setHorizontalHeaderLabels({ tr("Option"), tr("Value"), { "" } });
    ui()->memTableView->horizontalHeader()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);

    ui()->memTableView->setButtons(OptionsWidget::edit);

    ui()->memTableView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    ui()->memTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui()->memTableView->setGridStyle(Qt::NoPen);
    ui()->memTableView->setSortingEnabled(true);
    ui()->memTableView->sortByColumn(0, Qt::AscendingOrder);

    ui()->memTableView->insertRowWithOptions(0, { _advImpl->_pixmapCache, _advImpl->_pixmapCacheValue, new QStandardItem() }, false);

    connect(ui()->memTableView, &OptionsTableWidget::editItemPressed, this,
        [&](QStandardItem *item) {
            if (item == _advImpl->_pixmapCacheValue)
            {
                int unit = bytesSizeMaxUnit(QPixmapCache::cacheLimit());
                PixmapMemCacheDialog dlg(this);

                dlg.setData(bytesToString(QPixmapCache::cacheLimit(), unit, false), unit);

                if (dlg.exec() == QDialog::Accepted)
                {
                    _advImpl->_pixmapCacheValue->setText(bytesToString(dlg.memory));

                    QSettings settings("CentaurProject", "Centaur");
                    settings.beginGroup("advancedSettings-PixmapCache");
                    settings.setValue("size", dlg.memory);
                    settings.endGroup();

                    QPixmapCache::setCacheLimit(dlg.memory);
                }
            }
        });
}

END_CENTAUR_NAMESPACE
