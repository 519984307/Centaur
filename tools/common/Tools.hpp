/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_TOOLS_HPP
#define CENTAUR_TOOLS_HPP

#define CENTAUR_TOOL_STR(x)    CENTAUR_TOOL_DO_STR(x)
#define CENTAUR_TOOL_DO_STR(x) #x

#define CENTAUR_TOOL_JOIN(a, b)   CENTAUR_TOOL_JOIN_(a, b)
#define CENTAUR_TOOL_JOIN_(a, b)  CENTAUR_TOOL_JOIN_2(a, b)
#define CENTAUR_TOOL_JOIN_2(a, b) a##b

#if defined(__clang__) || defined(__GNUC__)

#define CENTAUR_TOOL_PRAGMA(x)      _Pragma(CENTAUR_TOOL_STR(x))
#define CENTAUR_TOOL_WARN_PRAGMA(x) CENTAUR_TOOL_PRAGMA(GCC diagnostic x)
#define CENTAUR_TOOL_WARN_OFF(x)    CENTAUR_TOOL_WARN_PRAGMA(ignored CENTAUR_TOOL_STR(CENTAUR_TOOL_JOIN(-W, x)))
#define CENTAUR_TOOL_WARN_PUSH      CENTAUR_TOOL_WARN_PRAGMA(push)
#define CENTAUR_TOOL_WARN_POP       CENTAUR_TOOL_WARN_PRAGMA(pop)

#endif

#ifndef C_NODISCARD
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define C_NODISCARD [[nodiscard]]
#else
#define T_NODISCARD
#endif /* defined ...*/
#endif /*C_NODISCARD*/

#ifndef C_UNUSED
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define C_UNUSED [[maybe_unused]]
#else
#define C_UNUSED
#endif /* defined ...*/
#endif /*C_UNUSED*/

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_TOOL_WARN_PUSH
// clang-format off
CENTAUR_TOOL_WARN_OFF(zero-as-null-pointer-constant)
CENTAUR_TOOL_WARN_OFF(extra-semi-stmt)
CENTAUR_TOOL_WARN_OFF(shadow)
CENTAUR_TOOL_WARN_OFF(ambiguous-reversed-operator)
CENTAUR_TOOL_WARN_OFF(suggest-override)
CENTAUR_TOOL_WARN_OFF(suggest-destructor-override)
CENTAUR_TOOL_WARN_OFF(reserved-id-macro)
CENTAUR_TOOL_WARN_OFF(deprecated-declarations)
// clang-format on
#endif /*__clang__*/

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_TOOL_WARN_POP
#endif /*__clang__*/

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_TOOL_WARN_PUSH
// clang-format off
CENTAUR_TOOL_WARN_OFF(documentation)
CENTAUR_TOOL_WARN_OFF(reserved-identifier)
CENTAUR_TOOL_WARN_OFF(documentation-unknown-command)
CENTAUR_TOOL_WARN_OFF(old-style-cast)
CENTAUR_TOOL_WARN_OFF(duplicate-enum)
// clang-format on
#endif /*__clang__*/

#include <libwebsockets.h>

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_TOOL_WARN_POP
#endif /*__clang__*/

#endif // CENTAUR_TOOLS_HPP
