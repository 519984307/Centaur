/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_OPTIONSTABLEWIDGET_HPP
#define CENTAUR_OPTIONSTABLEWIDGET_HPP

#include "Centaur.hpp"
#include "OptionsWidget.hpp"
#include <QAction>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTableView>

namespace CENTAUR_NAMESPACE
{

    class EditorDelegate : public QStyledItemDelegate
    {
        Q_OBJECT
    public:
        explicit EditorDelegate(QObject *parent = nullptr) noexcept;
        ~EditorDelegate() override = default;

    protected:
        bool eventFilter(QObject *editor, QEvent *event) override;

    signals:
        void sEditingCancelled(const QModelIndex &index);
        void sFunctionKeyPressed(const QModelIndex &index);
        void sFocusLost(const QModelIndex &index);
    };

    class OptionsTableWidget : public QTableView
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
        void initialize(QLineEdit *filterEdit, int cols, int filterColumn, int optionsColumn, QAction *editable);

    public:
        QStandardItemModel *getModel();
        void setButtons(int btns) noexcept;
        void setDeletionColumnData(int col);
        void allowDeletionWhenEditorIsClear(bool allow);
        void insertRowWithOptions(int row, const QList<QStandardItem *> &items, bool immediateEdit);
        C_NODISCARD int getRowCount() const noexcept;
        QSortFilterProxyModel *getProxyModel() noexcept;
        void removeItemRow(const QModelIndex &modelIndex) noexcept;

    private:
        void init(int filterColumn) noexcept;

    protected slots:
        void itemChangedLocal(QStandardItem *item) noexcept;
        void editorEditingCancelledLocal(const QModelIndex &index) noexcept;
        void editorLostFocusLocal(const QModelIndex &index) noexcept;
        void editorFunctionKeyPressedLocal(const QModelIndex &index) noexcept;
        void viewButtonPressed(QStandardItem *item) noexcept;

    signals:
        void editorEditingCancelled(const QModelIndex &index);
        void editorFunctionKeyPressed(const QModelIndex &index);
        void editorLostFocus(const QModelIndex &index);
        void itemHasChanged(QStandardItem *item);
        void itemDeleted(const QString &itemData);
        void viewItem(QStandardItem *item);

    protected:
        void editButtonPressed(QStandardItem *item) noexcept;
        void deleteButtonPressed(QStandardItem *item) noexcept;

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
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_OPTIONSTABLEWIDGET_HPP
