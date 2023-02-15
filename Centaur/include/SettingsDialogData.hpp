/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

// This header file is only intended to be included in SettingsDialog.hpp

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#include <QCheckBox>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QStandardItem>

#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include <fstream>

#ifndef CENTAUR_SETTINGSDIALOG_HPP
#include "Centaur.hpp"
#include "SettingsDialog.hpp"
#endif /*CENTAUR_SETTINGSDIALOG_HPP*/

#ifndef CENTAUR_SETTINGSDIALOGDATA_HPP
#define CENTAUR_SETTINGSDIALOGDATA_HPP

BEGIN_CENTAUR_NAMESPACE

struct SettingsDialog::Impl
{
    Impl();

    QWidget *currencySettingsWidget { nullptr };
    QTreeWidgetItem *sessionItem { nullptr };
    QTreeWidgetItem *advancedItem { nullptr };
    QTreeWidgetItem *pluginsItem { nullptr };
    QTreeWidgetItem *shortcutsItem { nullptr };

    std::unique_ptr<Ui::SettingsDialog> ui;

    bool forceShowPluginWarning { false };
};

struct SettingsDialog::SessionImpl final
{
    Q_DECLARE_TR_FUNCTIONS(SessionImpl)

public:
    inline SessionImpl() :
        _name { new QTableWidgetItem(QIcon(":/login/u_table/person"), tr("User display name")) },
        _user { new QTableWidgetItem(QIcon(":/login/u_table/user"), tr("Log in name")) },
        _psw { new QTableWidgetItem(QIcon(":/login/u_table/psw"), tr("User password")) },
        _email { new QTableWidgetItem(QIcon(":/login/u_table/email"), tr("User e-mail")) },
        _picture { new QTableWidgetItem(QIcon(":/login/u_table/pic"), tr("User picture")) },
        _2fa { new QTableWidgetItem(QIcon(":/login/u_table/lock"), tr("Allow Two-factor authentication")) },
        _nameValue { new QTableWidgetItem(tr("")) },
        _userValue { new QTableWidgetItem(tr("")) },
        _pswValue { new QTableWidgetItem(tr("")) },
        _emailValue { new QTableWidgetItem(tr("")) },
        _pictureValue { new QTableWidgetItem(tr("")) },
        _checkBoxWidget { new QWidget },
        _checkBox { new QCheckBox }
    {
        // "Trick" to make the checkbox H-centered
        auto *checkBoxLayout = new QHBoxLayout(_checkBoxWidget);
        checkBoxLayout->addWidget(_checkBox);
        checkBoxLayout->setAlignment(Qt::AlignCenter);
        checkBoxLayout->setContentsMargins(0, 0, 0, 0);
        _checkBoxWidget->setLayout(checkBoxLayout);
    }

public:
    QTableWidgetItem *_name;
    QTableWidgetItem *_user;
    QTableWidgetItem *_psw;
    QTableWidgetItem *_email;
    QTableWidgetItem *_picture;
    QTableWidgetItem *_2fa;
    QTableWidgetItem *_nameValue;
    QTableWidgetItem *_userValue;
    QTableWidgetItem *_pswValue;
    QTableWidgetItem *_emailValue;
    QTableWidgetItem *_pictureValue;
    QWidget *_checkBoxWidget;
    QCheckBox *_checkBox;
};

struct SettingsDialog::AdvancedImpl final
{
    Q_DECLARE_TR_FUNCTIONS(AdvancedImpl)

public:
    AdvancedImpl();

public:
    QStandardItem *_pixmapCache;
    QStandardItem *_pixmapCacheValue;
};

struct SettingsDialog::ShortcutsImpl final
{
    Q_DECLARE_TR_FUNCTIONS(ShortcutsImpl)

public:
    struct Wrapper
    {
        explicit Wrapper(QString fl) :
            jsonDoc(new rapidjson::Document),
            file { std::move(fl) }
        {
        }

        ~Wrapper()
        {
            if (writeAtDestruction)
                write();
            delete jsonDoc;
        }

        Wrapper(Wrapper &&rp) :
            jsonDoc { rp.jsonDoc },
            file { std::move(rp.file) }
        {
            rp.jsonDoc = nullptr;
        }

        // Avoid memory leaks
        Wrapper(const Wrapper &) = delete;

        void write()
        {
            if (jsonDoc == nullptr)
                return;

            std::ofstream stream(file.toUtf8().constData());
            rapidjson::OStreamWrapper r_stream(stream);
            rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(r_stream);
            jsonDoc->Accept(writer);
        }

        rapidjson::Document &operator()()
        {
            return *jsonDoc;
        }

        rapidjson::Document *jsonDoc;
        QString file;
        bool writeAtDestruction { true };
    };

public:
    QStandardItemModel *itemModel;

    std::map<QString, Wrapper> docKeymap;

    QAction *duplicate { nullptr };
    QAction *rename { nullptr };
    QAction *restore { nullptr };
    QAction *del { nullptr };
};

END_CENTAUR_NAMESPACE

#endif // CENTAUR_SETTINGSDIALOGDATA_HPP
