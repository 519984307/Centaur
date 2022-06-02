/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 02/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "FavoritesDialog.hpp"
#include <CentaurApp.hpp>
#include <Globals.hpp>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlTableModel>

namespace CENTAUR_NAMESPACE
{
#if defined(__clang__) || defined(__GNUC__)
    // clang-format off
CENTAUR_WARN_PUSH()
CENTAUR_WARN_OFF(weak-vtables)
    // clang-format on
#endif /*__clang__*/
    class FavoritesSQLModel final : public QSqlTableModel
    {
    public:
        inline explicit FavoritesSQLModel(QObject *parent) :
            QSqlTableModel(parent) { }

        inline ~FavoritesSQLModel() override = default;

        C_NODISCARD inline QVariant data(const QModelIndex &index, int role) const override
        {
            QVariant value = QSqlTableModel::data(index, role);

            if (value.isValid())
            {
                if (role == Qt::DisplayRole)
                {
                    if (index.column() == 2)
                    {
                        for (auto &loadedPlugins : g_app->m_pluginsData)
                        {
                            QString strPlid { loadedPlugins->getPluginUUID().to_string().c_str() };
                            if (strPlid == value.toString())
                            {
                                value.setValue(loadedPlugins->getPluginName());
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                if (role == Qt::DecorationRole)
                {
                    if (index.column() == 1)
                    {
                        value.setValue(g_globals->icons.favoritesIcon);
                    }
                }
            }

            return value;
        }
    };
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_POP()
#endif /*__clang__*/
} // namespace CENTAUR_NAMESPACE

CENTAUR_NAMESPACE::FavoritesDialog::FavoritesDialog(QWidget *parent) noexcept :
    QDialog(parent),
    m_ui { new Ui::FavoritesDialog }
{
    m_ui->setupUi(this);

    loadInterfaceState();

    connect(m_ui->closeBtn,
        &QPushButton::pressed,
        this,
        [&]() {
            saveInterfaceState();
            QDialog::accept();
        });
    connect(m_ui->deleteBtn, &QPushButton::released, this, &FavoritesDialog::onDelete);

    auto model = new FavoritesSQLModel(this);

    model->setTable("favorites");
    model->setEditStrategy(QSqlTableModel::EditStrategy::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, LS("ui-dialog-fav-id"));
    model->setHeaderData(1, Qt::Horizontal, LS("ui-dialog-fav-symbol"));
    model->setHeaderData(2, Qt::Horizontal, LS("ui-dialog-fav-plugin"));

    if (model->lastError().isValid())
    {
        logError("app", QString(LS("error-fav-dialog-db-select")).arg(model->lastError().text()));
    }
    else
    {
        m_ui->favoritesTable->setModel(model);

        m_ui->favoritesTable->hideColumn(0);
        m_ui->favoritesTable->setColumnWidth(0, m_state.datacols.id);
        m_ui->favoritesTable->setColumnWidth(1, m_state.datacols.symbol);
        m_ui->favoritesTable->setColumnWidth(2, m_state.datacols.plugin);
    }

    m_ui->favoritesTable->setIconSize({ 16, 16 });
}

CENTAUR_NAMESPACE::FavoritesDialog::~FavoritesDialog() = default;

void CENTAUR_NAMESPACE::FavoritesDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("favoritesDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("favoritesListState");
    settings.setValue("c0", m_ui->favoritesTable->columnWidth(0));
    settings.setValue("c1", m_ui->favoritesTable->columnWidth(1));
    settings.setValue("c2", m_ui->favoritesTable->columnWidth(2));
    settings.endGroup();
}

void CENTAUR_NAMESPACE::FavoritesDialog::loadInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "Centaur");

    settings.beginGroup("favoritesDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();

    settings.beginGroup("favoritesListState");
    m_state.datacols.id     = settings.value("c0", m_state.datacols.id).toInt();
    m_state.datacols.symbol = settings.value("c1", m_state.datacols.symbol).toInt();
    m_state.datacols.plugin = settings.value("c2", m_state.datacols.plugin).toInt();
    settings.endGroup();
}

void CENTAUR_NAMESPACE::FavoritesDialog::onDelete() noexcept
{
    auto model = dynamic_cast<QSqlTableModel *>(m_ui->favoritesTable->model());

    auto index = m_ui->favoritesTable->currentIndex();

    if (!index.isValid())
        return;

    int row = index.row();

    model->removeRows(row, 1);
    model->submitAll();

    if (model->lastError().isValid())
    {
        QString message = QString(LS("error-fav-dialog-db-delete")).arg(model->lastError().text());
        logError("app", message);
        QMessageBox::critical(this,
            LS("error-error"),
            message);
    }
}
