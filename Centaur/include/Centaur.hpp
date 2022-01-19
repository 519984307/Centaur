/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CENTAUR_HPP
#define CENTAUR_CENTAUR_HPP

#define CENTAUR_MAJOR_VERSION 0
#define CENTAUR_MINOR_VERSION 3
#define CENTAUR_PATCH_VERSION 0

#define CENTAUR_VERSION_CODE(x, y, z) \
    (((x)*100000) + ((y)*100) + (z))

#define CENTAUR_VERSION CENTAUR_VERSION_CODE(CENTAUR_MAJOR_VERSION, CENTAUR_MINOR_VERSION, CENTAUR_PATCH_VERSION)

#define CENTAUR_STR(x)    CENTAUR_DO_STR(x)
#define CENTAUR_DO_STR(x) #x

#define CENTAUR_JOIN(a, b)   CENTAUR_JOIN_(a, b)
#define CENTAUR_JOIN_(a, b)  CENTAUR_JOIN_2(a, b)
#define CENTAUR_JOIN_2(a, b) a##b

#if defined(__clang__)

#define CENTAUR_PRAGMA(x)      _Pragma(CENTAUR_STR(x))
#define CENTAUR_WARN_PRAGMA(x) CENTAUR_PRAGMA(GCC diagnostic x)
#define CENTAUR_WARN_OFF(x)    CENTAUR_WARN_PRAGMA(ignored CENTAUR_STR(CENTAUR_JOIN(-W, x)))
#define CENTAUR_WARN_PUSH      CENTAUR_WARN_PRAGMA(push)
#define CENTAUR_WARN_POP       CENTAUR_WARN_PRAGMA(pop)

// clang-format off
#define Q_OBJECT_WARN 1
#define Q_OBJECT_WARN_OFF \
    CENTAUR_WARN_PUSH     \
    CENTAUR_WARN_OFF(suggest-override)
#define Q_OBJECT_WARN_RESTORE \
    CENTAUR_WARN_POP
// clang-format on
#else
#define RAPIDJSON_DIAG_PUSH
#define RAPIDJSON_DIAG_POP
#endif

#ifndef DONT_INCLUDE_RAPIDJSON
#ifdef __clang__
// clang-format off
CENTAUR_WARN_PUSH
CENTAUR_WARN_OFF(zero-as-null-pointer-constant)
CENTAUR_WARN_OFF(extra-semi-stmt)
CENTAUR_WARN_OFF(shadow)
CENTAUR_WARN_OFF(ambiguous-reversed-operator)
CENTAUR_WARN_OFF(suggest-override)
CENTAUR_WARN_OFF(suggest-destructor-override)
CENTAUR_WARN_OFF(reserved-id-macro)
CENTAUR_WARN_OFF(deprecated-declarations)
// clang-format on
#endif /*__clang__*/
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>
#ifdef __clang__
CENTAUR_WARN_POP
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

#ifdef __clang__
CENTAUR_WARN_PUSH
// clang-format off
CENTAUR_WARN_OFF(elaborated-enum-base)
// clang-format on
#endif

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif /*Q_OS_MAC*/
#ifdef __clang__
CENTAUR_WARN_POP
#endif

#ifdef __clang__
// clang-format off
CENTAUR_WARN_PUSH
CENTAUR_WARN_OFF(reserved-identifier)
CENTAUR_WARN_OFF(quoted-include-in-framework-header)
CENTAUR_WARN_OFF(double-promotion)
CENTAUR_WARN_OFF(float-equal)
CENTAUR_WARN_OFF(documentation-unknown-command)
CENTAUR_WARN_OFF(inconsistent-missing-destructor-override)
CENTAUR_WARN_OFF(weak-vtables)
// clang-format on
#endif /*__clang__*/
#include "../ui/ui_CentaurApp.h"
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#ifdef __clang__
CENTAUR_WARN_POP
#endif /*__clang__*/
#endif /*DONT_INCLUDE_QT*/

namespace cent
{
#if defined(CENTAUR_VERSION) && CENTAUR_VERSION == 300
    constexpr unsigned char CentaurVersionStringId = { 0x0f };
    constexpr char CentaurVersionString[]          = "0.3.0";
#endif
} // namespace cent

#endif // CENTAUR_CENTAUR_HPP
