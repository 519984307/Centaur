////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAURTAXABLE_OPTIONSTABLEWIDGET_HPP
#define CENTAURTAXABLE_OPTIONSTABLEWIDGET_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"

#include <QAction>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableView>

#include "OptionsWidget.hpp"

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY OptionsTableWidget : public QTableView
{
    enum class EditableMode
    {
        byColumn,
        byAction
    };
    Q_OBJECT

public:
    explicit OptionsTableWidget(QWidget *parent = nullptr);
    ~OptionsTableWidget() override;

    void initialize(QLineEdit *filterEdit, int cols, int filterColumn, int optionsColumn, int editableCol);
    void initialize(QLineEdit *filterEdit, int cols, int filterColumn, int optionsColumn, QAction *editable, int editableCol = -1);

public:
    QStandardItemModel *getModel();
    void setButtons(int btns) noexcept;
    void setDeletionColumnData(int col);
    void allowDeletionWhenEditorIsClear(bool allow);
    void insertRowWithOptions(int row, const QList<QStandardItem *> &items, bool immediateEdit);
    [[nodiscard]] int getRowCount() const noexcept;
    QSortFilterProxyModel *getProxyModel() noexcept;
    bool removeItemRow(const QModelIndex &modelIndex) noexcept;
    /// \brief Once consumed. It will return an invalid model index
    QModelIndex getRemovedItem();

public:
    QModelIndex getItemIndexFromSource(QStandardItem *item);
    QModelIndex getItemIndexToSource(QStandardItem *item);
    /// \brief Get the indexes
    /// \param row row based on source indexes
    /// \param col col based on source indexes
    QModelIndex getItemIndex(int row, int col);
    /// \brief Column is in sourced based
    int getItemColumn(QStandardItem *item);
    /// \brief Column is in sourced based
    int getItemRow(QStandardItem *item);
    /// \brief Set model text in proxy model indexes (beware that if the table is filtered this indexes will change if not filtered,
    /// use getItemIndexFromSource for the correct acquisition of the index)
    void setItemText(int row, int col, const QString &text);
    /// \brief Set model data in model indexes (beware that if the table is filtered this indexes will change if not filtered)
    /// use getItemIndexFromSource for the correct acquisition of the index)
    void setItemData(int row, int col, int role, const QVariant &data);
    /// \brief Set model data in model indexes (beware that if the table is filtered this indexes will change if not filtered)
    /// use getItemIndexToSource for the correct acquisition of the index row and col)
    QString getItemText(int row, int col);
    /// \brief Set model data in model indexes (beware that if the table is filtered this indexes will change if not filtered)
    /// use getItemIndexToSource for the correct acquisition of the index)
    QVariant getItemData(int row, int col, int role);

private:
    void init(int filterColumn) noexcept;

protected slots:
    void itemChangedLocal(QStandardItem *item) noexcept;
    void editorEditingCancelledLocal(const QModelIndex &index) noexcept;
    void editorLostFocusLocal(const QModelIndex &index) noexcept;
    void editorFunctionKeyPressedLocal(const QModelIndex &index) noexcept;
    void viewButtonPressed(QStandardItem *item) noexcept;
    void onModelSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    void editorEditingCancelled(const QModelIndex &index);
    void editorFunctionKeyPressed(const QModelIndex &index);
    void editorLostFocus(const QModelIndex &index);
    void itemHasChanged(QStandardItem *item);
    void itemSelectionChanged(const QModelIndex &index);

    /// \brief itemDeleteButton does not remove the item from the table. removeItemRow to actually remove the item from the table
    /// To acquire the item use getRemovedItem
    void deleteItemPressed(const QString &itemData);
    void viewItemPressed(QStandardItem *item);
    void editItemPressed(QStandardItem *item);

protected:
    void editButtonPressed(QStandardItem *item) noexcept;
    void deleteButtonPressed(QStandardItem *item) noexcept;

protected:
    void focusOutEvent(QFocusEvent *event) override;

private:
    EditableMode m_mode;
    QLineEdit *m_filterEdit;
    QAction *m_editAction;
    int m_columns;
    int m_editableColumn;
    int m_optionsColumn;
    int m_deletionColumnData;
    int m_buttons;
    bool m_allowDeletion;
    QStandardItemModel *m_itemModel;
    QItemSelectionModel *m_selectionModel;
    QStandardItem *m_removedItem { nullptr };
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_OPTIONSTABLEWIDGET_HPP
