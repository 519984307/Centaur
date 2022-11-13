/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 26/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "PluginsDialog.hpp"
#include "../ui/ui_PluginsDialog.h"
#include "CentaurApp.hpp"
#include "CentaurPlugin.hpp"
#include <QMessageBox>
#include <QPainter>
#include <QSettings>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QStyledItemDelegate>

BEGIN_CENTAUR_NAMESPACE

namespace
{
    class DelegateItem : public QStyledItemDelegate
    {
    public:
        explicit DelegateItem(QObject *parent = nullptr) :
            QStyledItemDelegate { parent }
        {
        }

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            QStyleOptionViewItem options { option };
            initStyleOption(&options, index);

            if (index.column() == 1 || index.column() == 4)
                options.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;

            if (index.column() == 5)
                options.displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;

            if (option.state & QStyle::State_Selected)
                QStyledItemDelegate::paint(painter, options, index);
            else
            {
                painter->save();
                options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

                painter->restore();
            }
        }
    };

    class PluginsSQLModel final : public QSqlQueryModel
    {
    public:
        explicit PluginsSQLModel(QObject *parent) :
            QSqlQueryModel(parent) { }

        C_NODISCARD inline QVariant data(const QModelIndex &index, int role) const override
        {
            QVariant value = QSqlQueryModel::data(index, role);

            if (value.isValid())
            {
                if (role == Qt::DisplayRole)
                {
                    if (index.column() == 4 && value.toString() == CENTAUR_PLUGIN_NAMESPACE::centaurUUID)
                    {
                        value.setValue(QVariant { CentaurVersionString });
                    }
                    else if (index.column() == 5)
                    {
                        bool loaded = false;
                        auto plid   = this->index(index.row(), 3);

                        for (auto &loadedPlugins : g_app->m_pluginsData)
                        {
                            QString strPlid { loadedPlugins->getPluginUUID().to_string().c_str() };
                            if (strPlid == plid.data())
                            {
                                value.setValue(tr("Loaded"));
                                loaded = true;
                                break;
                            }
                        }
                        if (!loaded)
                            value.setValue(tr("Not loaded"));
                    }
                }
            }
            else
            {
                switch (role)
                {
                    case Qt::DecorationRole:
                        if (index.column() == 0)
                        {
                            value.setValue(QIcon(":/img/extension"));
                        }
                        break;
                    case Qt::BackgroundRole:
                        {
                            auto plid = this->index(index.row(), 5);
                            QString ld { plid.data(Qt::DisplayRole).toString() };
                            if (ld == tr("Loaded"))
                                value.setValue(QColor(0, 255, 0, 25));
                            else
                                value.setValue(QColor(255, 0, 0, 25));
                        }
                        break;
                    default:;
                }
            }

            return value;
        }
    };
} // namespace

struct PluginsDialog::Impl
{
    inline Impl() :
        ui { new Ui::PluginsDialog } { }
    inline ~Impl() = default;

    std::unique_ptr<Ui::PluginsDialog> ui;
};

PluginsDialog::PluginsDialog(QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);
    connect(ui()->acceptButton, &QPushButton::released, this, &PluginsDialog::onAccept);

    restoreInterface();

    ui()->pluginsTable->setItemDelegate(new DelegateItem);

    auto model = new PluginsSQLModel(this);
    model->setQuery("SELECT name,version,manufacturer,uuid,centaur_uuid,id FROM plugins;");
    model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Version"));
    model->setHeaderData(2, Qt::Horizontal, tr("Manufacturer"));
    model->setHeaderData(3, Qt::Horizontal, tr("Identification"));
    model->setHeaderData(4, Qt::Horizontal, tr("Centaur version"));
    model->setHeaderData(5, Qt::Horizontal, tr("Loaded?"));

    if (model->lastError().isValid())
    {
        logError("app", QString(tr("Could not select the data from the plugins DB. %1")).arg(model->lastError().text()));
    }
    else
    {
        ui()->pluginsTable->setModel(model);
    }

    ui()->pluginsTable->setIconSize({ 16, 16 });
}

PluginsDialog::~PluginsDialog() = default;

Ui::PluginsDialog *PluginsDialog::ui()
{
    return _impl->ui.get();
}

void PluginsDialog::onAccept() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("PluginsDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("pluginsInfoTable");
    settings.setValue("geometry", ui()->pluginsTable->saveGeometry());
    settings.setValue("h-geometry", ui()->pluginsTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->pluginsTable->horizontalHeader()->saveState());
    settings.endGroup();
    accept();
}

void PluginsDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("PluginsDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();

    settings.beginGroup("pluginsInfoTable");
    ui()->pluginsTable->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->pluginsTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->pluginsTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

END_CENTAUR_NAMESPACE
