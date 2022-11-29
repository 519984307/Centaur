/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 24/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_TESTDIALOG_HPP
#define CENTAUR_TESTDIALOG_HPP

#include "CentaurPlugin.hpp"
#include <QDialog>



class TestDialog : public QDialog
{
public:
    explicit TestDialog(QWidget *parent = nullptr);
    ~TestDialog() override;


};

extern QList<QPair<CENTAUR_PLUGIN_NAMESPACE::IExchange::Timestamp, CENTAUR_PLUGIN_NAMESPACE::CandleData>> testData;

#endif // CENTAUR_TESTDIALOG_HPP
