/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 26/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#ifndef CENTAUR_PLUGINSDIALOG_HPP
#define CENTAUR_PLUGINSDIALOG_HPP

#include "CentaurPlugin.hpp"
#include "CentaurUIState.hpp"
#include <QDialog>

namespace CENTAUR_NAMESPACE
{
    QT_BEGIN_NAMESPACE
    namespace Ui
    {
        class PluginsDialog;
    }
    QT_END_NAMESPACE

    class PluginsDialog final : public QDialog
    {
        Q_OBJECT

    public:
        explicit PluginsDialog(QWidget *parent = nullptr);
        ~PluginsDialog() override;

    private:
        void loadInterfaceState() noexcept;
        void saveInterfaceState() noexcept;

    protected:
        void accept() override;

    public:
        void populate();
        void loadLocalData();

    private:
        PluginsDialogInterfaceState m_state;
        Ui::PluginsDialog *m_ui;
        std::map<std::pair<uint32_t, uint16_t>, QString> m_localData;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_PLUGINSDIALOG_HPP
