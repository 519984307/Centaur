/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef P_APPLE
#error "Apple platform specifics file"
#endif /*P_APPLE*/

#ifndef CENTAUR_APPLEDIRECTORY_HPP
#define CENTAUR_APPLEDIRECTORY_HPP

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

extern const char *getApplicationSupportDirectory();

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // CENTAUR_APPLEDIRECTORY_HPP
