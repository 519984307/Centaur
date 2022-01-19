/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_GLOBALS_HPP
#define CENTAUR_GLOBALS_HPP

#include <QString>

namespace cent
{
    struct Globals
    {
        QString appPath;
        QString installPath;
        QString pluginsPath;
        QString resPath;
    };

    extern Globals *g_globals;
} // namespace cent

#endif // CENTAUR_GLOBALS_HPP
