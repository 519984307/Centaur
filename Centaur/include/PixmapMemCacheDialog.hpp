/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 08/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PIXMAPMEMCACHEDIALOG_HPP
#define CENTAUR_PIXMAPMEMCACHEDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class PixmapMemCacheDialog;
}
class PixmapMemCacheDialog : public QDialog
{
    enum Unit
    {
        bytes = 0,
        Kilo  = 1,
        Mega  = 2,
        Giga  = 3
    };

public:
    explicit PixmapMemCacheDialog(QWidget *parent = nullptr);
    ~PixmapMemCacheDialog() override;

public:
    void setData(const QString &m, int unit) noexcept;

protected slots:
    void onAccept() noexcept;
    void onCancel() noexcept;

protected:
    void restoreInterface() noexcept;

public:
    int memory { 0 };

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::PixmapMemCacheDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_PIXMAPMEMCACHEDIALOG_HPP
