/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_APPLICATION_GLOBALS_HPP
#define CENTAUR_APPLICATION_GLOBALS_HPP

#include "Centaur.hpp"

#define CENTAUR_MAJOR_VERSION 0
#define CENTAUR_MINOR_VERSION 3
#define CENTAUR_PATCH_VERSION 0

#define CENTAUR_VERSION CENTAUR_VERSION_CODE(CENTAUR_MAJOR_VERSION, CENTAUR_MINOR_VERSION, CENTAUR_PATCH_VERSION)

#ifndef DONT_INCLUDE_RAPIDJSON
#if defined(__clang__) || defined(__GNUC__)
// clang-format off
CENTAUR_WARN_PUSH()
CENTAUR_WARN_OFF("-Wzero-as-null-pointer-constant")
CENTAUR_WARN_OFF("-Wextra-semi-stmt")
CENTAUR_WARN_OFF("-Wshadow")
CENTAUR_WARN_OFF("-Wambiguous-reversed-operator")
CENTAUR_WARN_OFF("-Wsuggest-override")
CENTAUR_WARN_OFF("-Wsuggest-destructor-override")
CENTAUR_WARN_OFF("-Wreserved-id-macro")
CENTAUR_WARN_OFF("-Wdeprecated-declarations")
// clang-format on
#endif /*__clang__*/
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>
#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_POP()
#endif /*__clang__*/
#endif /*#DONT_INCLUDE_RAPIDJSON*/

#ifndef DONT_INCLUDE_QT
#include <QCoreApplication>
#include <QMainWindow>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QWidget>

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_PUSH()
// clang-format off
CENTAUR_WARN_OFF("-Welaborated-enum-base")
// clang-format on
#endif

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif /*Q_OS_MAC*/
#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_POP()
#endif

#endif /*DONT_INCLUDE_QT*/

#endif // CENTAUR_APPLICATION_GLOBALS_HPP
