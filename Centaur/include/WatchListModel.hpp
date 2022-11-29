/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 17/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_WATCHLISTMODEL_HPP
#define CENTAUR_WATCHLISTMODEL_HPP

#include "Centaur.hpp"
#include <QAbstractListModel>
#include <QWidget>

BEGIN_CENTAUR_NAMESPACE

class WatchlistModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        PriceRole = Qt::UserRole + 1,
        SourceRole,
        LatencyRole,
        DiffRole,
        LastPriceRole,
        IconRole
    };

public:
    explicit WatchlistModel(QObject *parent = nullptr);
    ~WatchlistModel() override;

public:
    C_NODISCARD int rowCount(const QModelIndex &parent) const override;
    C_NODISCARD QVariant data(const QModelIndex &index, int role) const override;

public:
    void insertWatchListElement(const QPixmap &icon, const QString &symbol, const QString &source, qreal price, qreal diff, qint64 lat);

    void updatePrice(const QString &symbol, const QString &source, qreal price);
    void updateDiff(const QString &symbol, const QString &source, qreal diff);
    void updateLatency(const QString &symbol, const QString &source, qint64 lat);
    void updatePriceAndLatency(const QString &symbol, const QString &source, qreal price, qint64 lat);


    void removeItem(const QString &symbol, const QString &source) noexcept;

public:
    C_NODISCARD std::pair<QString, QString> sourceFromIndex(const QModelIndex &index) noexcept;


private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_WATCHLISTMODEL_HPP
