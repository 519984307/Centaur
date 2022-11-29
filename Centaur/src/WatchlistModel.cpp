/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 17/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "WatchListModel.hpp"
#include <QHash>
#include <unordered_map>

BEGIN_CENTAUR_NAMESPACE
struct WatchlistModelKey
{
    WatchlistModelKey() = default;

    WatchlistModelKey(QString sym, QString src) :
        symbol { std::move(sym) },
        source { std::move(src) } { }

    bool operator==(const CENTAUR_NAMESPACE::WatchlistModelKey &p2) const noexcept
    {
        return symbol == p2.symbol && p2.source == source;
    }

    QString symbol;
    QString source;
};
END_CENTAUR_NAMESPACE

namespace std
{
    template <>
    struct hash<CENTAUR_NAMESPACE::WatchlistModelKey>
    {
        inline std::size_t operator()(const CENTAUR_NAMESPACE::WatchlistModelKey &key) const
        {
            QString newKey = key.symbol + key.source;
            return static_cast<std::size_t>(qHash(newKey));
        }
    };
} // namespace std

BEGIN_CENTAUR_NAMESPACE

struct WatchlistModelData
{
public:
    WatchlistModelData() = default;
    WatchlistModelData(/*QString sym, QString src, */ QPixmap icn, qreal price_, qreal diff_, qint64 latency_) :
        /*symbol { std::move(sym) },
        source { std::move(src) },*/
        icon { std::move(icn) },
        price { price_ },
        lastPrice { 0.0 },
        diff { diff_ },
        latency { latency_ }
    {
    }

public:
    QPixmap icon;
    qreal price;
    qreal lastPrice;
    qreal diff;
    qint64 latency;
};

struct WatchlistModel::Impl
{
    std::unordered_map<WatchlistModelKey, WatchlistModelData> data;
};

WatchlistModel::WatchlistModel(QObject *parent) :
    QAbstractListModel(parent),
    _impl { new Impl }
{
}

WatchlistModel::~WatchlistModel() = default;

int WatchlistModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(_impl->data.size());
}

QVariant WatchlistModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > index.row() > static_cast<int>(_impl->data.size()) || !index.isValid())
        return {};

    const auto row = static_cast<qint64>(index.row());
    auto iter      = std::next(_impl->data.begin(), row);

    switch (role)
    {
        case IconRole:
            return iter->second.icon;
        case Qt::DisplayRole:
            return { iter->first.symbol };
        case SourceRole:
            return { iter->first.source };
        case PriceRole:
            return { iter->second.price };
        case LatencyRole:
            return { iter->second.latency };
        case DiffRole:
            return { iter->second.diff };
        case LastPriceRole:
            return { iter->second.lastPrice };
        default:
            return {};
    }
}

void WatchlistModel::insertWatchListElement(const QPixmap &icon, const QString &symbol, const QString &source, qreal price, qreal diff, qint64 lat)
{
    beginResetModel();

    _impl->data[{ symbol, source }] = { icon, price, diff, lat };

    endResetModel();
}

void WatchlistModel::updatePrice(const QString &symbol, const QString &source, qreal price)
{
    if (auto iter = _impl->data.find({ symbol, source }); iter != _impl->data.end())
    {
        iter->second.lastPrice = iter->second.price;
        iter->second.price     = price;
        auto toIndex           = index(static_cast<int>(std::distance(_impl->data.begin(), iter)));
        emit dataChanged(toIndex, toIndex, { PriceRole, LastPriceRole });
    }
}

void WatchlistModel::updateDiff(const QString &symbol, const QString &source, qreal diff)
{
    if (auto iter = _impl->data.find({ symbol, source }); iter != _impl->data.end())
    {
        iter->second.diff = diff;
        auto toIndex      = index(static_cast<int>(std::distance(_impl->data.begin(), iter)));
        emit dataChanged(toIndex, toIndex, { DiffRole });
    }
}

void WatchlistModel::updateLatency(const QString &symbol, const QString &source, qint64 lat)
{
    if (auto iter = _impl->data.find({ symbol, source }); iter != _impl->data.end())
    {
        iter->second.latency = lat;
        auto toIndex         = index(static_cast<int>(std::distance(_impl->data.begin(), iter)));
        emit dataChanged(toIndex, toIndex, { LatencyRole });
    }
}

void WatchlistModel::updatePriceAndLatency(const QString &symbol, const QString &source, qreal price, qint64 lat)
{
    if (auto iter = _impl->data.find({ symbol, source }); iter != _impl->data.end())
    {
        iter->second.lastPrice = iter->second.price;
        iter->second.latency   = lat;
        iter->second.price     = price;
        auto toIndex           = index(static_cast<int>(std::distance(_impl->data.begin(), iter)));
        emit dataChanged(toIndex, toIndex, { LatencyRole, PriceRole, LastPriceRole });
    }
}

std::pair<QString, QString> WatchlistModel::sourceFromIndex(const QModelIndex &index) noexcept
{
    if (!index.isValid())
        return {};

    const auto row = static_cast<qint64>(index.row());
    auto iter      = std::next(_impl->data.begin(), row);

    return { iter->first.symbol, iter->first.source };
}

void WatchlistModel::removeItem(const QString &symbol, const QString &source) noexcept
{
    auto iter = _impl->data.find({ symbol, source });
    if (iter == _impl->data.end())
        return;
    else
    {
        beginResetModel();
        _impl->data.erase(iter);
        endResetModel();
    }
}

END_CENTAUR_NAMESPACE
