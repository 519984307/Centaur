/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CENTAURUISTATE_HPP
#define CENTAUR_CENTAURUISTATE_HPP

namespace CENTAUR_NAMESPACE
{

    struct PluginsDialogInterfaceState
    {
        struct DataCols
        {
            int name { 80 };
            int version { 80 };
            int manu { 80 };
            int plid { 80 };
            int cver { 80 };
            int lded { 120 };
        } datacols;
    };

    struct FavoritesDialogInterfaceState
    {
        struct DataCols
        {
            int id { 80 };
            int symbol { 80 };
            int plugin { 80 };
        } datacols;
    };

} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CENTAURUISTATE_HPP
