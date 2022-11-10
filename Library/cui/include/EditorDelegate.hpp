////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef CENTAUR_EDITOR_DELEGATE_HPP
#define CENTAUR_EDITOR_DELEGATE_HPP

#include "Centaur.hpp"
#include "CentaurLibrary.hpp"
#include <QStyledItemDelegate>

BEGIN_CENTAUR_NAMESPACE

class CENTFIN_LIBRARY EditorDelegate : public QStyledItemDelegate
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

END_CENTAUR_NAMESPACE

#endif /*CENTAUR_EDITOR_DELEGATE_HPP*/
