/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_HTMLDELEGATE_HPP
#define CENTAUR_HTMLDELEGATE_HPP

#include "../../Centaur.hpp"
#include <QStyledItemDelegate>

namespace CENTAUR_NAMESPACE
{
    class HTMLDelegate : public QStyledItemDelegate
    {
    public:
        HTMLDelegate();

        ~HTMLDelegate() override;

    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

        [[nodiscard]] inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_HTMLDELEGATE_HPP
