/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "HTMLDelegate.hpp"
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QTextDocument>

CENTAUR_NAMESPACE::HTMLDelegate::HTMLDelegate()  = default;

CENTAUR_NAMESPACE::HTMLDelegate::~HTMLDelegate() = default;

void CENTAUR_NAMESPACE::HTMLDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    // shift text right to make icon visible
    QSize iconSize = options.icon.actualSize(options.rect.size());
    painter->translate(options.rect.left() + iconSize.width(), options.rect.top());
    QRect clip(0, 0, options.rect.width() + iconSize.width(), options.rect.height());

    // doc.drawContents(painter, clip);

    painter->setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext ctx;
    // set text color to red for selected item
    if (option.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, QColor("red"));
    ctx.clip = clip;
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}

QSize CENTAUR_NAMESPACE::HTMLDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(static_cast<int>(doc.idealWidth()), static_cast<int>(doc.size().height()));
}
