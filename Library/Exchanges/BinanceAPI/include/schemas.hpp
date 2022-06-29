////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#pragma once

#ifndef SCHEMAS_HPP
#define SCHEMAS_HPP

#include "BinanceAPIGlobal.hpp"

namespace BINAPI_NAMESPACE::local::schemas
{
    extern char api_error[];
    extern char sapi_error[];
    extern char empty_schema[];
    namespace SPOT
    {
        extern char system_status[];
        extern char all_coins_information[];
        extern char snapshot_spot[];
        extern char snapshot_margin[];
        extern char snapshot_futures[];
        extern char *disable_fast_withdraw_switch; // pointer to empty_schema
        extern char *enable_fast_withdraw_switch;  // pointer to empty_schema
        extern char withdraw[];
        extern char deposit_history[];
        extern char withdraw_history[];
        extern char deposit_address[];
        extern char account_status[];
        extern char account_api_trading_status[];
        extern char dustlog[];
        extern char assets_convert_to_bnb[];
        extern char dust_transfer[];
        extern char asset_dividend_record[];
        extern char asset_detail[];
        extern char trade_fee[];
        extern char user_universal_transfer[];
        extern char query_user_universal_transfer[];
        extern char funding_wallet[];
        extern char get_api_key_permissions[];
        extern char *test_connectivity;
        extern char check_server_time[];
        extern char exchange_info[];
        extern char order_book[];
        extern char recent_trade_list[];
        extern char old_trade_lookup[];
        extern char compressed_aggregate_trades_list[];
        extern char candlestick_data[];
        extern char current_average_price[];
        extern char ticker_price_change_statistics[];
        extern char ticker_price_change_statistics_multiple[];
        extern char symbol_price_ticker[];
        extern char symbol_price_ticker_multiple[];
        extern char symbol_order_book_ticker[];
        extern char symbol_order_book_ticker_multiple[];
        extern char rolling_window_price_change_statistics[];
        extern char rolling_window_price_change_statistics_multiple[];

    } // namespace SPOT

    namespace Futures
    {
        extern char exchange_info[];
        extern char query_order[];
    } // namespace Futures

} // namespace BINAPI_NAMESPACE::local::schemas

#endif // SCHEMAS_HPP
