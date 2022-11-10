////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "EditorDelegate.hpp"
#include <QFocusEvent>
#include <QTableView>

BEGIN_CENTAUR_NAMESPACE

EditorDelegate::EditorDelegate(QObject *parent) noexcept :
    QStyledItemDelegate(parent)
{
    assert(parent != nullptr);
    installEventFilter(this);
}

bool EditorDelegate::eventFilter(QObject *editor, QEvent *event)
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

END_CENTAUR_NAMESPACE
