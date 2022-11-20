/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PORTFOLIOMODEL_HPP
#define CENTAUR_PORTFOLIOMODEL_HPP

#include "Centaur.hpp"
#include <QAbstractListModel>
#include <QWidget>

BEGIN_CENTAUR_NAMESPACE

class PortfolioModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        SymbolNameRole = Qt::UserRole + 1,
        SourceRole,
        IconRole,
        SymbolShortRole,
        BalanceRole,
        LinkRole
    };

public:
    explicit PortfolioModel(QObject *parent = nullptr);
    ~PortfolioModel() override;

public:
    C_NODISCARD int rowCount(const QModelIndex &parent) const override;
    C_NODISCARD QVariant data(const QModelIndex &index, int role) const override;

public:
    void insertPortfolioElement(const QPixmap &icon, const QString &symbol, const QString &source, const QString &shortName, const QString &link, const QString &balance);

    void updateBalance(const QString &symbol, const QString &source, const QString &balance);

public:
    C_NODISCARD std::pair<QString, QString> sourceFromIndex(const QModelIndex &index) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_PORTFOLIOMODEL_HPP
