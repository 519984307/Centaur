////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#ifndef CENTAUR_QRCODEDIALOG_HPP
#define CENTAUR_QRCODEDIALOG_HPP

#include "Centaur.hpp"
#include <QDialog>

BEGIN_CENTAUR_NAMESPACE

namespace Ui
{
    class QRCodeDialog;
}

class QRCodeDialog : public QDialog
{
public:
    explicit QRCodeDialog(const QString &mail, const QString &key, QWidget *parent = nullptr);
    ~QRCodeDialog() override;

protected slots:
    void dlgLinkActivated(const QString &link);

protected:
    void restoreInterface() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

protected:
    Ui::QRCodeDialog *ui();
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_QRCODEDIALOG_HPP
