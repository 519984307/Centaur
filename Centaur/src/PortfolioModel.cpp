/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "PortfolioModel.hpp"

BEGIN_CENTAUR_NAMESPACE
struct PortfolioModelKey
{
    PortfolioModelKey() = default;

    PortfolioModelKey(QString sym, QString src) :
        symbol { std::move(sym) },
        source { std::move(src) } { }

    bool operator==(const CENTAUR_NAMESPACE::PortfolioModelKey &p2) const noexcept
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
    struct hash<CENTAUR_NAMESPACE::PortfolioModelKey>
    {
        inline std::size_t operator()(const CENTAUR_NAMESPACE::PortfolioModelKey &key) const
        {
            QString newKey = key.symbol + key.source;
            return static_cast<std::size_t>(qHash(newKey));
        }
    };
} // namespace std

BEGIN_CENTAUR_NAMESPACE

struct PortfolioModelData
{
public:
    PortfolioModelData() = default;
    PortfolioModelData(/*QString sym, QString src, */ QPixmap icn, QString shrt, QString lnk, QString blnc) :
        /*symbol { std::move(sym) },
        source { std::move(src) },*/
        icon { std::move(icn) },
        shortName { std::move(shrt) },
        link { std::move(lnk) },
        balance { std::move(blnc) }
    {
    }

public:
    QPixmap icon;
    QString shortName;
    QString link;
    QString balance;
};

struct PortfolioModel::Impl
{
    std::unordered_map<PortfolioModelKey, PortfolioModelData> data;
};

PortfolioModel::PortfolioModel(QObject *parent) :
    QAbstractListModel(parent),
    _impl { new PortfolioModel::Impl }
{
}
PortfolioModel::~PortfolioModel() = default;

int PortfolioModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(_impl->data.size());
}

QVariant PortfolioModel::data(const QModelIndex &index, int role) const
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
            C_FALLTHROUGH;
        case SymbolNameRole:
            return { iter->first.symbol };
        case SourceRole:
            return { iter->first.source };
        case SymbolShortRole:
            return { iter->second.shortName };
        case BalanceRole:
            return { iter->second.balance };
        case LinkRole:
            return { iter->second.link };
        default:
            return {};
    }
}

void PortfolioModel::insertPortfolioElement(const QPixmap &icon, const QString &symbol, const QString &source, const QString &shortName, const QString &link, const QString &balance)
{
    beginResetModel();

    _impl->data[{ symbol, source }] = { icon, shortName, link, balance };

    endResetModel();
}

void PortfolioModel::updateBalance(const QString &symbol, const QString &source, const QString &balance)
{
    if (auto iter = _impl->data.find({ symbol, source }); iter != _impl->data.end())
    {
        iter->second.balance = balance;
        auto toIndex         = index(static_cast<int>(std::distance(_impl->data.begin(), iter)));
        emit dataChanged(toIndex, toIndex, { BalanceRole });
    }
}

std::pair<QString, QString> PortfolioModel::sourceFromIndex(const QModelIndex &index) noexcept
{
    if (!index.isValid())
        return {};

    const auto row = static_cast<qint64>(index.row());
    auto iter      = std::next(_impl->data.begin(), row);

    return { iter->first.symbol, iter->first.source };
}

END_CENTAUR_NAMESPACE
