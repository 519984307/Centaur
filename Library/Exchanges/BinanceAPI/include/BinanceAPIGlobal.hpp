////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef BINAPI_GLOBAL_HPP
#define BINAPI_GLOBAL_HPP

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#define BINAPI_VERSION_STRING "1.0.0"
#define BINAPI_VERSION        100000

#define BINAPI_MAJOR_VERSION 1
#define BINAPI_MINOR_VERSION 0
#define BINAPI_PATCH_VERSION 0

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#define Q_DECL_HIDDEN
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#define Q_DECL_HIDDEN __attribute__((visibility("hidden")))
#endif

#if defined(BINAPI_LIBRARY)
#define BINAPI_EXPORT Q_DECL_EXPORT
#else
#define BINAPI_EXPORT Q_DECL_IMPORT
#endif

#define BINAPI_HIDDEN Q_DECL_HIDDEN

#define BINAPI_STR(x)    BINAPI_DO_STR(x)
#define BINAPI_DO_STR(x) #x

#define BINAPI_JOIN(a, b)   BINAPI_JOIN_(a, b)
#define BINAPI_JOIN_(a, b)  BINAPI_JOIN_2(a, b)
#define BINAPI_JOIN_2(a, b) a##b

#if defined(__clang__) || defined(__GNUC__)

#define BINAPI_PRAGMA(x)      _Pragma(BINAPI_STR(x))
#define BINAPI_WARN_PRAGMA(x) BINAPI_PRAGMA(GCC diagnostic x)
#define BINAPI_WARN_OFF(x)    BINAPI_WARN_PRAGMA(ignored BINAPI_STR(BINAPI_JOIN(-W, x)))
#define BINAPI_WARN_PUSH      BINAPI_WARN_PRAGMA(push)
#define BINAPI_WARN_POP       BINAPI_WARN_PRAGMA(pop)

#endif

// General BINAPI namespace for libraries
#ifndef BINAPI_NAMESPACE
#define BINAPI_NAMESPACE binapi
#endif /*BINAPI_NAMESPACE*/

#ifndef T_NODISCARD
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define T_NODISCARD [[nodiscard]]
#else
#define T_NODISCARD
#endif /* defined ...*/
#endif /*T_NODISCARD*/

#ifndef T_UNUSED
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define T_UNUSED [[maybe_unused]]
#else
#define T_UNUSED
#endif /* defined ...*/
#endif /*T_UNUSED*/

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_PUSH
// clang-format off
BINAPI_WARN_OFF(zero-as-null-pointer-constant)
BINAPI_WARN_OFF(extra-semi-stmt)
BINAPI_WARN_OFF(shadow)
BINAPI_WARN_OFF(ambiguous-reversed-operator)
BINAPI_WARN_OFF(suggest-override)
BINAPI_WARN_OFF(suggest-destructor-override)
BINAPI_WARN_OFF(reserved-id-macro)
BINAPI_WARN_OFF(deprecated-declarations)
// clang-format on
#endif /*__clang__*/

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_POP
#endif /*__clang__*/

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_PUSH
// clang-format off
BINAPI_WARN_OFF(old-style-cast)
BINAPI_WARN_OFF(padded)
BINAPI_WARN_OFF(deprecated-volatile)
BINAPI_WARN_OFF(weak-vtables)
BINAPI_WARN_OFF(documentation-unknown-command)
BINAPI_WARN_OFF(reserved-id-macro)
BINAPI_WARN_OFF(documentation-deprecated-sync)
BINAPI_WARN_OFF(undefined-func-template)
BINAPI_WARN_OFF(nonportable-system-include-path)
BINAPI_WARN_OFF(language-extension-token)
BINAPI_WARN_OFF(newline-eof)
BINAPI_WARN_OFF(non-virtual-dtor)
BINAPI_WARN_OFF(undef)
BINAPI_WARN_OFF(old-style-cast)
BINAPI_WARN_OFF(deprecated-copy-with-dtor)
// clang-format on
#endif /*__clang__*/

#include <curl/curl.h>
// C++ CURL Wrapper C++Requests (CPR)
#include <cpr/cpr.h>

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_POP
#endif /*__clang__*/

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_PUSH
// clang-format off
BINAPI_WARN_OFF(documentation)
BINAPI_WARN_OFF(reserved-identifier)
BINAPI_WARN_OFF(documentation-unknown-command)
BINAPI_WARN_OFF(old-style-cast)
BINAPI_WARN_OFF(duplicate-enum)
// clang-format on
#endif /*__clang__*/

#include <libwebsockets.h>

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_POP
#endif /*__clang__*/

#endif // BINAPI_GLOBAL_HPP
