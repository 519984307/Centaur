/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CENTAUR_GLOBALS
#define CENTAUR_CENTAUR_GLOBALS

// Main GUI Application Version
#define CENTAUR_MAJOR_VERSION 0
#define CENTAUR_MINOR_VERSION 3
#define CENTAUR_PATCH_VERSION 0

// Centaur Communication Protocol Version
#define CENTAUR_PROTOCOL_MAJOR_VERSION 0
#define CENTAUR_PROTOCOL_MINOR_VERSION 1
#define CENTAUR_PROTOCOL_PATCH_VERSION 0

#define CENTAUR_VERSION_CODE(x, y, z) \
    (((x)*100000) + ((y)*100) + (z))

#define CENTAUR_VERSION CENTAUR_VERSION_CODE(CENTAUR_MAJOR_VERSION, CENTAUR_MINOR_VERSION, CENTAUR_PATCH_VERSION)

#define CENTAUR_PROTOCOL_VERSION CENTAUR_VERSION_CODE(CENTAUR_PROTOCOL_MAJOR_VERSION, CENTAUR_PROTOCOL_MINOR_VERSION, CENTAUR_PROTOCOL_PATCH_VERSION)

#define CENTAUR_STR(x)    CENTAUR_DO_STR(x)
#define CENTAUR_DO_STR(x) #x

#define CENTAUR_JOIN(a, b)   CENTAUR_JOIN_(a, b)
#define CENTAUR_JOIN_(a, b)  CENTAUR_JOIN_2(a, b)
#define CENTAUR_JOIN_2(a, b) a##b

#if defined(__clang__) || defined(__GNUC__)

#define CENTAUR_PRAGMA(x)      _Pragma(CENTAUR_STR(x))
#define CENTAUR_WARN_PRAGMA(x) CENTAUR_PRAGMA(GCC diagnostic x)
#define CENTAUR_WARN_OFF(x)    CENTAUR_WARN_PRAGMA(ignored CENTAUR_STR(CENTAUR_JOIN(-W, x)))
#define CENTAUR_WARN_PUSH      CENTAUR_WARN_PRAGMA(push)
#define CENTAUR_WARN_POP       CENTAUR_WARN_PRAGMA(pop)
#endif

// General Centaur namespace for libraries
#ifndef CENTAUR_NAMESPACE
#define CENTAUR_NAMESPACE cen
#endif /*CENTAUR_NAMESPACE*/

#ifndef NODISCARD
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif /* defined ...*/
#endif /*NODISCARD*/

#ifndef UNUSED
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define UNUSED [[maybe_unused]]
#else
#define UNUSED
#endif /* defined ...*/
#endif /*UNUSED*/

namespace CENTAUR_NAMESPACE
{
#if defined(CENTAUR_VERSION) && CENTAUR_VERSION == 300
    constexpr char CentaurVersionString[] = "0.3.0";
#endif

#if defined(CENTAUR_VERSION) && CENTAUR_PROTOCOL_VERSION == 100
    constexpr char CentaurProtocolVersionString[] = "0.1.0";
#endif
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CENTAUR_GLOBALS
