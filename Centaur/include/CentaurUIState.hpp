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
    struct UserInterfaceState
    {
        struct WatchlistColumns
        {
            int symbol { 85 };
            int price { 90 };
            int sender { 90 };
            int latency { 60 };
        } wlcols;

        struct LoggerlistCols
        {
            int date { 110 };
            int user { 70 };
            int session { 50 };
            int type { 50 };
            int source { 130 };
            int message { 950 };
        } lgcols;

        struct BalancesTreeCols
        {
            int name { 150 };
            int value { 80 };
        } blcols;

        struct OrderbookCols
        {
            int price { 100 };
            int amount { 80 };
            int total { 100 };
        } bidscols, askscols;
    };

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
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_CENTAURUISTATE_HPP
