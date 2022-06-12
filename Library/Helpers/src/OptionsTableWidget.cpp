/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include <QHeaderView>
//////////////////
#include "OptionsTableWidget.hpp"
#include "OptionsWidget.hpp"

#include <QFocusEvent>
#include <QTableView>

CENTAUR_NAMESPACE::EditorDelegate::EditorDelegate(QObject *parent) noexcept :
    QStyledItemDelegate(parent)
{
    assert(parent != nullptr);
    installEventFilter(this);
}

bool CENTAUR_NAMESPACE::EditorDelegate::eventFilter(QObject *editor, QEvent *event)
{
    // qDebug() << event;
    if (editor != nullptr)
    {
        auto table = dynamic_cast<QTableView *>(parent());
        if (event->type() == QEvent::ShortcutOverride)
        {
            auto key = dynamic_cast<QKeyEvent *>(event);
            if (key->key() == Qt::Key_Escape)
            {
                emit sEditingCancelled(table->currentIndex());
            }
            emit sFunctionKeyPressed(table->currentIndex());
        }
        else if (event->type() == QEvent::FocusOut)
        {

            emit sFocusLost(table->currentIndex());
        }
    }

    return QStyledItemDelegate::eventFilter(editor, event);
}

CENTAUR_NAMESPACE::OptionsTableWidget::OptionsTableWidget(QWidget *parent) :
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

CENTAUR_NAMESPACE::OptionsTableWidget::~OptionsTableWidget() = default;

void CENTAUR_NAMESPACE::OptionsTableWidget::initialize(QLineEdit *filterEdit, int cols, int filterColumn, int optionsColumn, int editableCol)
{
    m_mode           = EditableMode::byColumn;
    m_filterEdit     = filterEdit;
    m_columns        = cols;
    m_editableColumn = editableCol;
    m_optionsColumn  = optionsColumn;
    init(filterColumn);
}
void CENTAUR_NAMESPACE::OptionsTableWidget::initialize(QLineEdit *filterEdit, int cols, int filterColumn, int optionsColumn, QAction *editable)
{
    m_mode           = EditableMode::byAction;
    m_filterEdit     = filterEdit;
    m_editAction     = editable;
    m_columns        = cols;
    m_editableColumn = -1;
    m_optionsColumn  = optionsColumn;
    init(filterColumn);
}

void CENTAUR_NAMESPACE::OptionsTableWidget::init(int filterColumn) noexcept
{
    m_itemModel         = new QStandardItemModel(0, m_columns, this);
    auto sortProxyModel = new QSortFilterProxyModel(this);
    sortProxyModel->setFilterKeyColumn(filterColumn);
    sortProxyModel->setSourceModel(m_itemModel);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->setModel(sortProxyModel);

    int localEditableColumn;

    switch (m_mode)
    {
        case EditableMode::byColumn:
            {
                localEditableColumn = m_editableColumn;

                auto delegate       = new EditorDelegate(this);
                this->setItemDelegateForColumn(m_editableColumn, delegate);

                connect(delegate, &EditorDelegate::sEditingCancelled, this, &CENTAUR_NAMESPACE::OptionsTableWidget::editorEditingCancelledLocal);
                connect(delegate, &EditorDelegate::sFocusLost, this, &CENTAUR_NAMESPACE::OptionsTableWidget::editorLostFocusLocal);
                connect(delegate, &EditorDelegate::sFunctionKeyPressed, this, &CENTAUR_NAMESPACE::OptionsTableWidget::editorFunctionKeyPressedLocal);
            }
            break;
        case EditableMode::byAction:
            localEditableColumn = 0;
            break;
    }

    connect(m_filterEdit, &QLineEdit::textChanged, sortProxyModel, [this, localEditableColumn = localEditableColumn](const QString &str) {
        auto proxy = getProxyModel();

        proxy->setFilterFixedString(str);

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
    });

    connect(m_itemModel, &QStandardItemModel::itemChanged, this, &CENTAUR_NAMESPACE::OptionsTableWidget::itemChangedLocal);

    if (this->horizontalHeader() != nullptr)
    {
        connect(horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [&](int logicalIndex, Qt::SortOrder order) {
            m_itemModel->sort(logicalIndex, order);
        });
    }
}

QStandardItemModel *CENTAUR_NAMESPACE::OptionsTableWidget::getModel()
{
    return m_itemModel;
}

void CENTAUR_NAMESPACE::OptionsTableWidget::setDeletionColumnData(int col)
{
    m_deletionColumnData = col;
}

void CENTAUR_NAMESPACE::OptionsTableWidget::allowDeletionWhenEditorIsClear(bool allow)
{
    m_allowDeletion = allow;
}

void CENTAUR_NAMESPACE::OptionsTableWidget::editButtonPressed(QStandardItem *item) noexcept
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
            m_editAction->setData(QVariant::fromValue(item));
            emit m_editAction->triggered(m_editAction->isChecked());
            break;
    }
}

void CENTAUR_NAMESPACE::OptionsTableWidget::deleteButtonPressed(QStandardItem *item) noexcept
{
    if (m_deletionColumnData != -1)
    {
        auto data = m_itemModel->item(item->row(), m_deletionColumnData);
        emit itemDeleted(data->index().data().toString());
    }

    m_itemModel->removeRow(item->index().row());
}

void CENTAUR_NAMESPACE::OptionsTableWidget::viewButtonPressed(QStandardItem *item) noexcept
{
    emit viewItem(item);
}

void CENTAUR_NAMESPACE::OptionsTableWidget::itemChangedLocal(QStandardItem *item) noexcept
{
    emit itemHasChanged(item);
}

void CENTAUR_NAMESPACE::OptionsTableWidget::editorEditingCancelledLocal(const QModelIndex &index) noexcept
{
    auto proxy = getProxyModel();

    auto item  = m_itemModel->itemFromIndex(proxy->mapToSource(proxy->index(index.row(), m_editableColumn)));

    if (item->data(Qt::DisplayRole).toString().isEmpty() && m_allowDeletion)
    {
        deleteButtonPressed(item);
    }
    else
    {
        emit editorEditingCancelled(index);
    }
}

void CENTAUR_NAMESPACE::OptionsTableWidget::editorLostFocusLocal(const QModelIndex &index) noexcept
{
    if (index.isValid())
        editorEditingCancelledLocal(index);
    emit editorLostFocus(index);
}

void CENTAUR_NAMESPACE::OptionsTableWidget::editorFunctionKeyPressedLocal(const QModelIndex &index) noexcept
{
    emit editorFunctionKeyPressed(index);
}

void CENTAUR_NAMESPACE::OptionsTableWidget::insertRowWithOptions(int row, const QList<QStandardItem *> &items, bool immediateEdit)
{
    assert(items.size() == m_columns);

    m_itemModel->insertRow(row, items);

    auto proxy        = getProxyModel();
    auto optionsIndex = proxy->mapFromSource(items[m_optionsColumn]->index());

    auto opts         = new OptionsWidget(m_buttons);
    this->setIndexWidget(optionsIndex, opts);

    const int itemCol = [&]() {
        if (m_editableColumn == -1)
            return 0;
        return m_editableColumn;
    }();

    connect(opts->editButton(), &QPushButton::released, this, [=, this]() { editButtonPressed(items[itemCol]); });
    connect(opts->deleteButton(), &QPushButton::released, this, [=, this]() { deleteButtonPressed(items[itemCol]); });
    connect(opts->viewButton(), &QPushButton::released, this, [=, this]() { viewButtonPressed(items[itemCol]); });

    if (immediateEdit)
    {
        auto editableData = proxy->mapFromSource(items[m_editableColumn]->index());
        this->setCurrentIndex(editableData);
        this->edit(editableData);
    }
}

int CENTAUR_NAMESPACE::OptionsTableWidget::getRowCount() const noexcept
{
    return m_itemModel->rowCount();
}
QSortFilterProxyModel *CENTAUR_NAMESPACE::OptionsTableWidget::getProxyModel() noexcept
{
    return qobject_cast<QSortFilterProxyModel *>(this->model());
}

void CENTAUR_NAMESPACE::OptionsTableWidget::removeItemRow(const QModelIndex &modelIndex) noexcept
{
    m_itemModel->removeRow(modelIndex.row());
}

void CENTAUR_NAMESPACE::OptionsTableWidget::setButtons(int btns) noexcept
{
    m_buttons = btns;
}
