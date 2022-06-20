/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#if (!defined(__cplusplus) || __cplusplus < 202002L)
#error "C++20 compiler or higher needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PROJECT
#define CENTAUR_PROJECT

#define CENTAUR_VERSION_CODE(x, y, z) \
    (((x)*100000) + ((y)*100) + (z))

#define CENTAUR_STR(x)    CENTAUR_DO_STR(x)
#define CENTAUR_DO_STR(x) #x

#define CENTAUR_JOIN(a, b)   CENTAUR_JOIN_(a, b)
#define CENTAUR_JOIN_(a, b)  CENTAUR_JOIN_2(a, b)
#define CENTAUR_JOIN_2(a, b) a##b

#if defined(__clang__) || defined(__GNUC__)

#define CENTAUR_PRAGMA(x)      _Pragma(CENTAUR_STR(x))
#define CENTAUR_WARN_PRAGMA(x) CENTAUR_PRAGMA(GCC diagnostic x)
#define CENTAUR_WARN_OFF(x)    CENTAUR_WARN_PRAGMA(ignored CENTAUR_STR(CENTAUR_JOIN(-W, x)))
#define CENTAUR_WARN_PUSH()    CENTAUR_WARN_PRAGMA(push)
#define CENTAUR_WARN_POP()     CENTAUR_WARN_PRAGMA(pop)
#endif

// General Centaur namespace for libraries
#ifndef CENTAUR_NAMESPACE
#define CENTAUR_NAMESPACE cen
#endif /*CENTAUR_NAMESPACE*/

#ifndef C_NODISCARD
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define C_NODISCARD [[nodiscard]]
#else
#define C_NODISCARD
#endif /* defined ...*/
#endif /*C_NODISCARD*/

#ifndef C_UNUSED
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define C_UNUSED [[maybe_unused]]
#else
#define C_UNUSED
#endif /* defined ...*/
#endif /*C_UNUSED*/

#ifndef C_NORETURN
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define C_NORETURN [[noreturn]]
#else
#define C_NORETURN
#endif /* defined ...*/
#endif /*C_NORETURN*/

#ifndef C_FALLTHROUGH
#if defined(__clang__) || defined(__GNU__) || defined(MSVC)
#define C_FALLTHROUGH [[fallthrough]]
#else
#define C_FALLTHROUGH
#endif /* defined ...*/
#endif /*C_FALLTHROUGH*/

namespace CENTAUR_NAMESPACE
{
    constexpr char CentaurVersionString[]         = "0.3.0";
    constexpr char CentaurProtocolVersionString[] = "0.1.0";
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_PROJECT
