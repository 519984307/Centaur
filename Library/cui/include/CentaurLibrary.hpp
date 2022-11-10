////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#ifndef FINANCEABLE_CENTAURFINANCEABLELIBRARY_HPP
#define FINANCEABLE_CENTAURFINANCEABLELIBRARY_HPP

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#define Q_DECL_HIDDEN
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#define Q_DECL_HIDDEN __attribute__((visibility("hidden")))
#endif

#if defined(CENTFIN_LIBRARY_)
#define CENTFIN_LIBRARY Q_DECL_EXPORT
#else
#define CENTFIN_LIBRARY Q_DECL_IMPORT
#endif

#define CENTFI_HIDDEN Q_DECL_HIDDEN

#endif // FINANCEABLE_CENTAURFINANCEABLELIBRARY_HPP
