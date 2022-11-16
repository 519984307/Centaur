////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include <QHeaderView>

#include "EditorDelegate.hpp"
#include "OptionsTableWidget.hpp"
#include "OptionsWidget.hpp"

BEGIN_CENTAUR_NAMESPACE

OptionsTableWidget::OptionsTableWidget(QWidget *parent) :
    QTableView { parent },
    m_mode { EditableMode::byColumn },
    m_filterEdit { nullptr },
    m_editAction { nullptr },
    m_columns { -1 },
    m_editableColumn { -1 },
    m_optionsColumn { -1 },
    m_deletionColumnData { -1 },
    m_buttons { 0 },
    m_allowDeletion { false }
{
}

OptionsTableWidget::~OptionsTableWidget() = default;

void OptionsTableWidget::initialize(QLineEdit *filterEdit, int cols, int filterColumn, int optionsColumn, int editableCol)
{
    m_mode           = EditableMode::byColumn;
    m_filterEdit     = filterEdit;
    m_columns        = cols;
    m_editableColumn = editableCol;
    m_optionsColumn  = optionsColumn;
    init(filterColumn);
}

void OptionsTableWidget::initialize(QLineEdit *filterEdit, int cols, int filterColumn, int optionsColumn, QAction *editable)
{
    m_mode           = EditableMode::byAction;
    m_filterEdit     = filterEdit;
    m_editAction     = editable;
    m_columns        = cols;
    m_editableColumn = -1;
    m_optionsColumn  = optionsColumn;
    init(filterColumn);
}

void OptionsTableWidget::init(int filterColumn) noexcept
{
    m_itemModel         = new QStandardItemModel(0, m_columns, this);
    auto sortProxyModel = new QSortFilterProxyModel(this);
    sortProxyModel->setFilterKeyColumn(filterColumn);
    sortProxyModel->setSourceModel(m_itemModel);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->setModel(sortProxyModel);
    m_selectionModel = new QItemSelectionModel(sortProxyModel, this);
    this->setSelectionModel(m_selectionModel);

    int localEditableColumn;

    connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &OptionsTableWidget::onModelSelectionChanged);

    switch (m_mode)
    {
        case EditableMode::byColumn:
            {
                localEditableColumn = m_editableColumn;

                auto delegate = new EditorDelegate(this);
                this->setItemDelegateForColumn(m_editableColumn, delegate);

                connect(delegate, &EditorDelegate::sEditingCancelled, this, &OptionsTableWidget::editorEditingCancelledLocal);
                connect(delegate, &EditorDelegate::sFocusLost, this, &OptionsTableWidget::editorLostFocusLocal);
                connect(delegate, &EditorDelegate::sFunctionKeyPressed, this, &OptionsTableWidget::editorFunctionKeyPressedLocal);
            }
            break;
        case EditableMode::byAction:
            localEditableColumn = 0;
            break;
    }

    connect(m_filterEdit, &QLineEdit::textChanged, sortProxyModel, [this, localEditableColumn = localEditableColumn](const QString &str) {
        auto proxy = getProxyModel();

        proxy->setFilterFixedString(str);

        if (m_optionsColumn != -1)
        {
            // This fixes a bug where the QWidget is deleted when the filter is applied
            for (int r = 0; r < proxy->rowCount(); ++r)
            {
                auto widget = this->indexWidget(proxy->index(r, m_optionsColumn));

                if (widget == nullptr)
                {
                    auto opts = new OptionsWidget(m_buttons);

                    auto item = m_itemModel->itemFromIndex(proxy->mapToSource(proxy->index(r, localEditableColumn)));
                    this->setIndexWidget(proxy->index(r, m_optionsColumn), opts);

                    connect(opts->editButton(), &QPushButton::released, this, [=, this]() { editButtonPressed(item); });
                    connect(opts->deleteButton(), &QPushButton::released, this, [=, this]() { deleteButtonPressed(item); });
                    connect(opts->viewButton(), &QPushButton::released, this, [=, this]() { viewButtonPressed(item); });
                }
            }
        }
    });

    connect(m_itemModel, &QStandardItemModel::itemChanged, this, &OptionsTableWidget::itemChangedLocal);

    if (this->horizontalHeader() != nullptr)
    {
        connect(horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [&](int logicalIndex, Qt::SortOrder order) {
            m_itemModel->sort(logicalIndex, order);
        });
    }
}

QStandardItemModel *OptionsTableWidget::getModel()
{
    return m_itemModel;
}

void OptionsTableWidget::setDeletionColumnData(int col)
{
    m_deletionColumnData = col;
}

void OptionsTableWidget::allowDeletionWhenEditorIsClear(bool allow)
{
    m_allowDeletion = allow;
}

void OptionsTableWidget::editButtonPressed(QStandardItem *item) noexcept
{
    switch (m_mode)
    {
        case EditableMode::byColumn:
            {
                auto proxy                        = getProxyModel();
                const QModelIndex &editIndexProxy = proxy->mapFromSource(item->index());
                this->setCurrentIndex(editIndexProxy);
                this->edit(editIndexProxy);
            }
            break;
        case EditableMode::byAction:
            // TODO: reformat code to eliminate the editing action
            // m_editAction->setData(QVariant::fromValue(item));
            // emit m_editAction->triggered(m_editAction->isChecked());
            emit editItemPressed(item);
            break;
    }
}

void OptionsTableWidget::deleteButtonPressed(QStandardItem *item) noexcept
{
    m_removedItem = item;

    if (m_deletionColumnData != -1)
    {
        auto data = m_itemModel->item(item->row(), m_deletionColumnData);
        emit deleteItemPressed(data->index().data().toString());
    }

    // m_itemModel->removeRow(item->index().row());
}

QModelIndex OptionsTableWidget::getRemovedItem()
{
    QStandardItem *temp = m_removedItem;
    m_removedItem       = nullptr;
    if (temp)
    {
        return getItemIndexFromSource(temp);
    }
    else
        return {};
}

void OptionsTableWidget::viewButtonPressed(QStandardItem *item) noexcept
{
    emit viewItemPressed(item);
}

void OptionsTableWidget::itemChangedLocal(QStandardItem *item) noexcept
{
    emit itemHasChanged(item);
}

void OptionsTableWidget::editorEditingCancelledLocal(const QModelIndex &index) noexcept
{
    auto proxy = getProxyModel();

    auto item = m_itemModel->itemFromIndex(proxy->mapToSource(proxy->index(index.row(), m_editableColumn)));

    if (item->data(Qt::DisplayRole).toString().isEmpty() && m_allowDeletion)
    {
        deleteButtonPressed(item);
    }
    else
    {
        emit editorEditingCancelled(index);
    }
}

void OptionsTableWidget::editorLostFocusLocal(const QModelIndex &index) noexcept
{
    if (index.isValid())
        editorEditingCancelledLocal(index);
    emit editorLostFocus(index);
}

void OptionsTableWidget::editorFunctionKeyPressedLocal(const QModelIndex &index) noexcept
{
    emit editorFunctionKeyPressed(index);
}

void OptionsTableWidget::insertRowWithOptions(int row, const QList<QStandardItem *> &items, bool immediateEdit)
{
    assert(items.size() == m_columns);

    m_itemModel->insertRow(row, items);
    auto proxy = getProxyModel();

    if (m_optionsColumn != -1)
    {

        auto optionsIndex = proxy->mapFromSource(items[m_optionsColumn]->index());

        auto opts = new OptionsWidget(m_buttons);
        this->setIndexWidget(optionsIndex, opts);

        const int itemCol = [&]() {
            if (m_editableColumn == -1)
                return 0;
            return m_editableColumn;
        }();

        connect(opts->editButton(), &QPushButton::released, this, [=, this]() { editButtonPressed(items[itemCol]); });
        connect(opts->deleteButton(), &QPushButton::released, this, [=, this]() { deleteButtonPressed(items[itemCol]); });
        connect(opts->viewButton(), &QPushButton::released, this, [=, this]() { viewButtonPressed(items[itemCol]); });
    }

    if (immediateEdit)
    {
        auto editableData = proxy->mapFromSource(items[m_editableColumn]->index());
        this->setCurrentIndex(editableData);
        this->edit(editableData);
    }
}

void OptionsTableWidget::onModelSelectionChanged(const QModelIndex &current, C_UNUSED const QModelIndex &previous)
{
    emit itemSelectionChanged(current);
}

void OptionsTableWidget::focusOutEvent(QFocusEvent *event)
{
    m_selectionModel->clear();
    QTableView::focusOutEvent(event);
}

int OptionsTableWidget::getRowCount() const noexcept
{
    return m_itemModel->rowCount();
}

QSortFilterProxyModel *OptionsTableWidget::getProxyModel() noexcept
{
    return qobject_cast<QSortFilterProxyModel *>(this->model());
}

bool OptionsTableWidget::removeItemRow(const QModelIndex &modelIndex) noexcept
{
    return m_itemModel->removeRow(modelIndex.row());
}

void OptionsTableWidget::setButtons(int btns) noexcept
{
    m_buttons = btns;
}

QModelIndex OptionsTableWidget::getItemIndexFromSource(QStandardItem *item)
{
    return getProxyModel()->mapFromSource(item->index());
}

QModelIndex OptionsTableWidget::getItemIndexToSource(QStandardItem *item)
{
    return getProxyModel()->mapToSource(item->index());
}

QModelIndex OptionsTableWidget::getItemIndex(int row, int col)
{
    return getProxyModel()->index(row, col);
}

int OptionsTableWidget::getItemColumn(QStandardItem *item)
{
    return getItemIndexFromSource(item).column();
}

int OptionsTableWidget::getItemRow(QStandardItem *item)
{
    return getItemIndexFromSource(item).row();
}

void OptionsTableWidget::setItemText(int row, int col, const QString &text)
{
    auto sourceIndex = getProxyModel()->mapToSource(getItemIndex(row, col));
    getModel()->item(sourceIndex.row(), sourceIndex.column())->setText(text);
}

void OptionsTableWidget::setItemData(int row, int col, int role, const QVariant &data)
{
    auto sourceIndex = getProxyModel()->mapToSource(getItemIndex(row, col));
    getModel()->item(sourceIndex.row(), sourceIndex.column())->setData(data, role);
}

QString OptionsTableWidget::getItemText(int row, int col)
{
    return getItemIndex(row, col).data().toString();
}

QVariant OptionsTableWidget::getItemData(int row, int col, int role)
{
    return getItemIndex(row, col).data(role);
}

END_CENTAUR_NAMESPACE
