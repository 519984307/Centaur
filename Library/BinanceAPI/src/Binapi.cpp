////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// binapi.cpp

/**
 * @file binapi.cpp
 * @brief Binance API
 *
 * @author Ricardo Romero
 * @date 2021-07-07
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#include "Binapi.hpp"
#include <array>
#include <iostream>
#include <random>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif /*__clang__*/


#include <fmt/core.h>
#include <fmt/format.h>


#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wdeprecated-volatile"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wdocumentation-deprecated-sync"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif /*__clang__*/

#include <openssl/hmac.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif /*__clang__*/

#include "APIException.hpp"
#include "schemas.hpp"

#define JTO_STRING(x, y) \
    std::string { x[y].GetString(), x[y].GetStringLength() }

static rapidjson::SchemaDocument BINAPI_HIDDEN loadJsonSchema(const char *schemaData) noexcept
{
    namespace json = rapidjson;
    json::Document schemaDocument;
    // Since its internal data it will be assumed. all data is correct. in case of data corruption the problem lies in the security of the system
    schemaDocument.Parse(schemaData);
    // Load the rapidjson
    json::SchemaDocument schemaConfig(schemaDocument);
    return json::SchemaDocument(schemaDocument);
}

namespace trader::api::local
{
    enum class HTTPRequest
    {
        GET,
        POST,
        DELETE,
        PUT
    };
    enum binanceAPIRequests
    {
        SYSTEM_STATUS,
        ALL_COINS_INFORMATION_USER,
        DAILY_ACCOUNT_SNAPSHOT,
        DISABLE_FAST_WITHDRAW_SWITCH,
        ENABLE_FAST_WITHDRAW_SWITCH,
        WITHDRAW,
        DEPOSIT_HISTORY,
        WITHDRAW_HISTORY,
        DEPOSIT_ADDRESS,
        ACCOUNT_STATUS,
        ACCOUNT_TRADING_STATUS,
        DUST_LOG,
        DUST_TRANSFER,
        ASSET_DIVIDEND_RECORD,
        ASSET_DETAIL,
        TRADE_FEE,
        USER_UNIVERSAL_TRANSFER,
        QUERY_USER_UNIVERSAL_TRANSFER,
        TEST_CONNECTIVITY,
        CHECK_SERVER_TIME,
        EXCHANGE_INFO,
        ORDER_BOOK_1,
        ORDER_BOOK_5,
        ORDER_BOOK_10,
        ORDER_BOOK_50,
        RECENT_TRADE_LIST,
        OLD_TRADE_LOOKUP,
        COMPRESSED_AGGREGATE_TRADES,
        KLINE_CANDLESTICK_DATA,
        CURRENT_AVERAGE_PRICE,
        DAILY_TICKER_PRICE_CHANGE_STATISTICS_1,
        DAILY_TICKER_PRICE_CHANGE_STATISTICS_40,
        SYMBOL_PRICE_TICKER_1,
        SYMBOL_PRICE_TICKER_2,
        SYMBOL_ORDER_BOOK_TICKER,
        SYMBOL_ORDER_BOOK_TICKER_2,
        TEST_NEW_ORDER,
        NEW_ORDER,
        CANCEL_ORDER,
        CANCEL_ALL_OPEN_ORDERS_ON_A_SYMBOL,
        QUERY_ORDER,
        CURRENT_OPEN_ORDERS_3,
        CURRENT_OPEN_ORDERS_40,
        ALL_ORDERS,
        NEW_OCO,
        CANCEL_OCO,
        QUERY_OCO,
        QUERY_ALL_OCO,
        QUERY_OPEN_OCO,
        ACCOUNT_INFORMATION,
        ACCOUNT_TRADE_LIST,
        TRADE_LIST,
        LISTEN_KEY,
        KEEP_ALIVE_LISTEN_KEY
    };

    enum binanceFuturesAPIRequests
    {
        F_PING,
        F_SERVER_TIME,
        F_EXCHANGE_INFORMATION,
        F_ORDER_BOOK_2,
        F_ORDER_BOOK_5,
        F_ORDER_BOOK_10,
        F_ORDER_BOOK_20,
        F_RECENT_TRADES_LIST,
        F_OLD_TRADES_LOOKUP,
        F_COMPRESSED_AGGREGATE_TRADES_LIST,
        F_KLINE_1,
        F_KLINE_2,
        F_KLINE_5,
        F_KLINE_10,
        F_CONTINUES_CONTRACT_KLINE_1,
        F_CONTINUES_CONTRACT_KLINE_2,
        F_CONTINUES_CONTRACT_KLINE_5,
        F_CONTINUES_CONTRACT_KLINE_10,
        F_INDEX_PRICE_KLINE_1,
        F_INDEX_PRICE_KLINE_2,
        F_INDEX_PRICE_KLINE_5,
        F_INDEX_PRICE_KLINE_10,
        F_MARK_PRICE_KLINE_1,
        F_MARK_PRICE_KLINE_2,
        F_MARK_PRICE_KLINE_5,
        F_MARK_PRICE_KLINE_10,
        F_MARK_PRICE,
        F_FUNDING_RATE_HISTORY,
        F_24HR_TICKER_PRICE_1,
        F_24HR_TICKER_PRICE_40,
        F_SYMBOL_PRICE_TICKER_1,
        F_SYMBOL_PRICE_TICKER_2,
        F_SYMBOL_ORDER_BOOK_TICKER_1,
        F_SYMBOL_ORDER_BOOK_TICKER_2,
        F_OPEN_INTEREST,
        F_OPEN_INTEREST_STATISTICS,
        F_TOP_TRADER_LONG_SHORT_ACCOUNT,
        F_TOP_TRADER_LONG_SHORT_POSITION,
        F_LONG_SHORT_RATIO,
        F_TAKER_BUY_SELL_VOLUME,
        F_HISTORICAL_BLVT,
        F_COMPOSITE_INDEX_SYMBOL,
        F_CHANGE_POSITION_MODE,
        F_GET_POSITION_MODE,
        F_CHANGE_MULTI_ASSET_MODE,
        F_GET_MULTI_ASSET_MODE,
        F_ACCOUNT_BALANCE,
        F_ACCOUNT_INFORMATION,
        F_CHANGE_INITIAL_LEVERAGE,
        F_CHANGE_MARGIN_TYPE,
        F_MODIFY_ISOLATED_POSITION,
        F_GET_POSITION_MARGIN_CHANGE_HISTORY,
        F_POSITION_INFORMATION,
        F_ACCOUNT_TRADE_LIST,
        F_START_USER_STREAM,
        F_KEEP_ALIVE_USER_STREAM,
        F_CLOSE_USER_STREAM,
        F_NEW_LIMIT_ORDER,
        F_CANCEL_ORDER
    };

    struct BinanceAPIRequest final
    {
        /// Some exchanges may define a weight for each call
        /// This parameter will be passed to Limits::secure...Call()
        long long callWeight { 0 };
        /// Request endpoint
        std::string_view endPoint {};
        /// HTTP request type
        HTTPRequest http { HTTPRequest::GET };
        /// Some exchanges make distinctions between API Call for orders and others
        bool isOrderCall { false };
        /// Normal response Schema
        std::unique_ptr<rapidjson::SchemaDocument> docSchema { nullptr };
        /// Error Schema
        std::unique_ptr<rapidjson::SchemaDocument> errorSchema { nullptr };
        /// A user data type
        bool isSAPI;
        /// When signed prevent to send a signature only an API-key in the header. This is the third parameter in secureRequest
        bool preventSignatureWhenSigned { false };
        /// use to determine the URL
        bool isFuture { false };
    };

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif /*__clang__*/
    auto loadSchema = [](const char *data) {
        return std::make_unique<rapidjson::SchemaDocument>(loadJsonSchema(data));
    };
    static std::array<BinanceAPIRequest, 53> g_apiRequests  = { {
        { 1, "sapi/v1/system/status", HTTPRequest::GET, false, loadSchema(schemas::system_status), loadSchema(schemas::sapi_error), true, false, false },
        { 1, "sapi/v1/capital/config/getall", HTTPRequest::GET, false, loadSchema(schemas::system_status), loadSchema(schemas::sapi_error), true, false, false },
        { 1, "sapi/v1/accountSnapshot", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 0, "sapi/v1/account/disableFastWithdrawSwitch", HTTPRequest::POST, false, nullptr, nullptr, true, false, false },
        { 0, "sapi/v1/account/enableFastWithdrawSwitch", HTTPRequest::POST, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/capital/withdraw/apply", HTTPRequest::POST, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/capital/deposit/hisrec", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/capital/withdraw/history", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/capital/deposit/address", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/account/status", HTTPRequest::GET, false, loadSchema(schemas::account_status), loadSchema(schemas::sapi_error), true, false, false },
        { 1, "sapi/v1/account/apiTradingStatus", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/asset/dribblet", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/asset/dust", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/asset/assetDividend", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/asset/assetDetail", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/asset/tradeFee", HTTPRequest::GET, false, loadSchema(schemas::trade_fees), loadSchema(schemas::sapi_error), true, false, false },
        { 1, "sapi/v1/asset/transfer", HTTPRequest::POST, false, nullptr, nullptr, true, false, false },
        { 1, "sapi/v1/asset/transfer", HTTPRequest::GET, false, nullptr, nullptr, true, false, false },
        { 1, "api/v3/ping", HTTPRequest::GET, false, loadSchema(schemas::empty), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/time", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 10, "api/v3/exchangeInfo", HTTPRequest::GET, false, loadSchema(schemas::exchange_info), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::order_book_ticker), loadSchema(schemas::api_error), false, false, false },
        { 5, "api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::order_book_ticker), loadSchema(schemas::api_error), false, false, false },
        { 10, "api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::order_book_ticker), loadSchema(schemas::api_error), false, false, false },
        { 50, "api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::order_book_ticker), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/trades", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 5, "api/v3/historicalTrades", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 1, "api/v3/aggTrades", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 1, "api/v3/klines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/avgPrice", HTTPRequest::GET, false, loadSchema(schemas::average_price), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/ticker/24hr", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 40, "api/v3/ticker/24hr", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 1, "api/v3/ticker/price", HTTPRequest::GET, false, loadSchema(schemas::price_ticker), loadSchema(schemas::api_error), false, false, false },
        { 2, "api/v3/ticker/price", HTTPRequest::GET, false, loadSchema(schemas::price_ticker2), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/ticker/bookTicker", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 2, "api/v3/ticker/bookTicker", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 1, "api/v3/order/test", HTTPRequest::POST, true, loadSchema(schemas::empty), loadSchema(schemas::api_error), false, false, false },
        { 2, "api/v3/order", HTTPRequest::POST, true, loadSchema(schemas::new_order), loadSchema(schemas::api_error), false, false, false },
        { 2, "api/v3/order", HTTPRequest::DELETE, true, loadSchema(schemas::cancel_order), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/openOrders", HTTPRequest::DELETE, true, nullptr, nullptr, false, false, false },
        { 2, "api/v3/order", HTTPRequest::GET, true, loadSchema(schemas::query_order), loadSchema(schemas::api_error), false, false, false },
        { 3, "api/v3/openOrders", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 40, "api/v3/openOrders", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 10, "api/v3/allOrders", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 1, "api/v3/order/oco", HTTPRequest::POST, true, nullptr, nullptr, false, false, false },
        { 1, "api/v3/orderList", HTTPRequest::DELETE, true, nullptr, nullptr, false, false, false },
        { 2, "api/v3/orderList", HTTPRequest::GET, true, nullptr, nullptr, false, false, false },
        { 10, "api/v3/allOrderList", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 3, "api/v3/openOrderList", HTTPRequest::GET, false, nullptr, nullptr, false, false, false },
        { 10, "api/v3/account", HTTPRequest::GET, false, loadSchema(schemas::account_information), loadSchema(schemas::api_error), false, false, false },
        { 10, "api/v3/myTrades", HTTPRequest::GET, false, loadSchema(schemas::trades_list), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/userDataStream", HTTPRequest::POST, true, loadSchema(schemas::listen_key), loadSchema(schemas::api_error), false, false, false },
        { 1, "api/v3/userDataStream", HTTPRequest::PUT, true, loadSchema(schemas::empty), loadSchema(schemas::api_error), false, true, false },
    } };

    static std::array<BinanceAPIRequest, 59> g_fapiRequests = { {
        { 1, "fapi/v1/ping", HTTPRequest::GET, false, loadSchema(schemas::empty), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/time", HTTPRequest::GET, false, loadSchema(schemas::server_time), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/exchangeInfo", HTTPRequest::GET, false, loadSchema(schemas::fapi_exchange_info), loadSchema(schemas::api_error), false, false, true },
        { 2, "fapi/v1/depth", HTTPRequest::GET, false, loadSchema(schemas::fapi_order_book), loadSchema(schemas::api_error), false, false, true },
        { 5, "fapi/v1/depth", HTTPRequest::GET, false, loadSchema(schemas::fapi_order_book), loadSchema(schemas::api_error), false, false, true },
        { 10, "fapi/v1/depth", HTTPRequest::GET, false, loadSchema(schemas::fapi_order_book), loadSchema(schemas::api_error), false, false, true },
        { 20, "fapi/v1/depth", HTTPRequest::GET, false, loadSchema(schemas::fapi_order_book), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/trades", HTTPRequest::GET, false, loadSchema(schemas::fapi_recent_trades_list), loadSchema(schemas::api_error), false, false, true },
        { 20, "fapi/v1/historicalTrades", HTTPRequest::GET, false, loadSchema(schemas::fapi_old_trades_lookup), loadSchema(schemas::api_error), false, false, true },
        { 20, "fapi/v1/aggTrades", HTTPRequest::GET, false, loadSchema(schemas::fapi_compressed_aggregate_trades_list), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/klines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 2, "fapi/v1/klines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 5, "fapi/v1/klines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 10, "fapi/v1/klines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true }, // 13
        { 1, "fapi/v1/continuousKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 2, "fapi/v1/continuousKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 5, "fapi/v1/continuousKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 10, "fapi/v1/continuousKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true }, // 17
        { 1, "fapi/v1/indexPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 2, "fapi/v1/indexPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 5, "fapi/v1/indexPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 10, "fapi/v1/indexPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true }, // 21
        { 1, "fapi/v1/markPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 2, "fapi/v1/markPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 5, "fapi/v1/markPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 10, "fapi/v1/markPriceKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true }, // 25
        { 1, "fapi/v1/premiumIndex", HTTPRequest::GET, false, loadSchema(schemas::fapi_mark_price), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/fundingRate", HTTPRequest::GET, false, loadSchema(schemas::fapi_funding_rate_history), loadSchema(schemas::api_error), false, false, true }, // 27
        { 1, "fapi/v1/ticker/24hr", HTTPRequest::GET, false, loadSchema(schemas::fapi_24hr_ticker), loadSchema(schemas::api_error), false, false, true },
        { 40, "fapi/v1/ticker/24hr", HTTPRequest::GET, false, loadSchema(schemas::fapi_24hr_price_nosymbol), loadSchema(schemas::api_error), false, false, true }, // 29
        { 1, "fapi/v1/ticker/price", HTTPRequest::GET, false, loadSchema(schemas::fapi_symbol_price_ticker), loadSchema(schemas::api_error), false, false, true },
        { 2, "fapi/v1/ticker/price", HTTPRequest::GET, false, loadSchema(schemas::fapi_symbol_price_ticker_nosym), loadSchema(schemas::api_error), false, false, true }, // 31
        { 1, "fapi/v1/ticker/bookTicker", HTTPRequest::GET, false, loadSchema(schemas::fapi_order_book_ticker), loadSchema(schemas::api_error), false, false, true },
        { 2, "fapi/v1/ticker/bookTicker", HTTPRequest::GET, false, loadSchema(schemas::fapi_order_book_ticker_nosym), loadSchema(schemas::api_error), false, false, true }, // 33
        { 1, "fapi/v1/openInterest", HTTPRequest::GET, false, loadSchema(schemas::fapi_open_interest), loadSchema(schemas::api_error), false, false, true },
        { 1, "futures/data/openInterestHist", HTTPRequest::GET, false, loadSchema(schemas::fapi_open_interest_statistics), loadSchema(schemas::api_error), false, false, true },
        { 1, "futures/data/topLongShortAccountRatio", HTTPRequest::GET, false, loadSchema(schemas::fapi_top_trader_account), loadSchema(schemas::api_error), false, true, true },
        { 1, "futures/data/topLongShortPositionRatio", HTTPRequest::GET, false, loadSchema(schemas::fapi_top_trader_account), loadSchema(schemas::api_error), false, false, true },
        { 1, "futures/data/globalLongShortAccountRatio", HTTPRequest::GET, false, loadSchema(schemas::fapi_top_trader_account), loadSchema(schemas::api_error), false, false, true },
        { 1, "futures/data/takerlongshortRatio", HTTPRequest::GET, false, loadSchema(schemas::fapi_taker_buysell_volume), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/lvtKlines", HTTPRequest::GET, false, loadSchema(schemas::candlestick), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/indexInfo", HTTPRequest::GET, false, loadSchema(schemas::fapi_composite_index), loadSchema(schemas::api_error), false, false, true }, // 41
        { 1, "fapi/v1/positionSide/dual", HTTPRequest::POST, false, loadSchema(schemas::fapi_success), loadSchema(schemas::api_error), false, false, true },
        { 30, "fapi/v1/positionSide/dual", HTTPRequest::GET, false, loadSchema(schemas::fapi_current_position), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/multiAssetsMargin", HTTPRequest::POST, false, loadSchema(schemas::fapi_success), loadSchema(schemas::api_error), false, false, true },
        { 30, "fapi/v1/multiAssetsMargin", HTTPRequest::GET, false, loadSchema(schemas::fapi_multiasset_mode), loadSchema(schemas::api_error), false, false, true }, // 45
        { 5, "fapi/v2/balance", HTTPRequest::GET, false, loadSchema(schemas::fapi_futures_account_balance), loadSchema(schemas::api_error), false, false, true },    // 46
        { 5, "fapi/v2/account", HTTPRequest::GET, false, loadSchema(schemas::fapi_futures_account_information), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/leverage", HTTPRequest::POST, false, loadSchema(schemas::fapi_change_initial_leverage), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v1/marginType", HTTPRequest::POST, false, loadSchema(schemas::fapi_success), loadSchema(schemas::api_error), false, false, true },                             // 49
        { 1, "fapi/v1/positionMargin", HTTPRequest::POST, false, loadSchema(schemas::fapi_modify_isolated_position_margin), loadSchema(schemas::api_error), false, false, true }, // 50
        { 1, "fapi/v1/positionMargin/history", HTTPRequest::GET, false, loadSchema(schemas::fapi_position_margin_changed_history), loadSchema(schemas::api_error), false, false, true },
        { 1, "fapi/v2/positionRisk", HTTPRequest::GET, false, loadSchema(schemas::fapi_position_info), loadSchema(schemas::api_error), false, false, true },
        { 5, "fapi/v1/userTrades", HTTPRequest::GET, false, loadSchema(schemas::fapi_account_trade_list), loadSchema(schemas::api_error), false, false, true }, // 53
        { 1, "fapi/v1/listenKey", HTTPRequest::POST, false, loadSchema(schemas::listen_key), loadSchema(schemas::api_error), false, true, true },               // 54
        { 1, "fapi/v1/listenKey", HTTPRequest::PUT, false, loadSchema(schemas::empty), loadSchema(schemas::api_error), false, true, true },                     // 55
        { 1, "fapi/v1/listenKey", HTTPRequest::DELETE, false, loadSchema(schemas::empty), loadSchema(schemas::api_error), false, true, true },                  // 56
        { 1, "fapi/v1/order", HTTPRequest::POST, true, loadSchema(schemas::fapi_order), loadSchema(schemas::api_error), false, false, true },                   // 57
        { 1, "fapi/v1/order", HTTPRequest::DELETE, true, loadSchema(schemas::fapi_order), loadSchema(schemas::api_error), false, false, true },                 // 58
    } };

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/
} // namespace trader::api::local

using namespace trader::api::local;

trader::api::BinanceAPI::BinanceAPI(BinanceKeys *keys, BinanceLimits *exl) :
    m_spotAddress { "https://api.binance.com" }, m_futuresAddres { "https://fapi.binance.com" }, m_userAgent { "CPP_interface_api/curl-openssl/7.70.0/Ricardo" }, m_binanceKeys { keys }, m_limits { exl }
{
}

trader::api::BinanceAPI::~BinanceAPI() = default;

std::string trader::api::BinanceAPI::HMAC256(const std::string &key, const std::string &plainText) noexcept
{
    std::string hmac;
    unsigned int hashSize = EVP_MAX_MD_SIZE;

    unsigned char *str    = ::HMAC(EVP_sha256(),
           key.c_str(),
           static_cast<int>(key.size()),
           reinterpret_cast<const unsigned char *>(plainText.c_str()),
           plainText.size(),
           nullptr,
           &hashSize);

    for (auto i = 0ul; i < hashSize; i++)
        hmac += fmt::format("{:02x}", str[i]);

    return hmac;
}

rapidjson::Document trader::api::BinanceAPI::request(BinanceAPIRequest *request, cpr::Parameters &parameters, const bool &secure, double *const timeLog)
{
    cpr::Url url { fmt::format("{}/{}", request->isFuture ? m_futuresAddres : m_spotAddress, request->endPoint) };

    cpr::Session session;

    if (secure)
    {
        // Do proper changes to the data
        secureRequest(session, parameters, request->preventSignatureWhenSigned);
    }

    session.SetUrl(url);
    session.SetParameters(parameters);
    session.SetUserAgent(m_userAgent);

    bool secureRequest;
    secureRequest = m_limits->secureCall(request->callWeight);
    if (request->isOrderCall && secureRequest)
        secureRequest = m_limits->secureOrderCall(request->callWeight);

    if (!secureRequest)
    {
        throw APIException(APIException::Type::limits, "limits reached");
    }

    cpr::Response apiCall;
    switch (request->http)
    {
        case HTTPRequest::GET:
            apiCall = session.Get();
            break;
        case HTTPRequest::DELETE:
            apiCall = session.Delete();
            break;
        case HTTPRequest::POST:
            apiCall = session.Post();
            break;
        case HTTPRequest::PUT:
            apiCall = session.Put();
            break;
    }

    // Log Time
    if (timeLog != nullptr)
        *timeLog = apiCall.elapsed;

    // CURL Checking
    if (apiCall.error.code != cpr::ErrorCode::OK)
    {
        // Error Code, URL, API Call, Error Message
        throw APIException(APIException::Type::request, static_cast<int>(apiCall.error.code), apiCall.url, apiCall.error.message);
    }

    // HTTP Error checking
    if (apiCall.status_code != 200)
    {
        if (apiCall.header["content-type"] == "application/json;charset=UTF-8" || apiCall.header["content-type"] == "application/json")
        {
            int code;
            std::string msg;
            handleHTTPErrorResponse(apiCall.text, code, msg);
            throw APIException(APIException::Type::http2, apiCall.status_code, code, apiCall.url, apiCall.reason, msg);
        }
        else
        {
            throw APIException(APIException::Type::http1, apiCall.status_code, apiCall.url, apiCall.reason);
        }
    }

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(apiCall.text.c_str());
    if (jsonDoc.HasParseError())
    {
        throw APIException(APIException::Type::json, apiCall.url, fmt::format("{}. At Offset: {}", rapidjson::GetParseError_En(jsonDoc.GetParseError()), jsonDoc.GetErrorOffset()));
    }

    rapidjson::SchemaValidator errorValidator(*request->errorSchema);
    if (jsonDoc.Accept(errorValidator))
    {
        int code;
        std::string msg;
        if (handleAPIRequestError(request->isSAPI, jsonDoc, code, msg))
            throw APIException(APIException::Type::api, code, apiCall.url, msg);
    }

    // Validate against the returned endpoint data
    rapidjson::SchemaValidator schemaValidator(*request->docSchema);

    if (!jsonDoc.Accept(schemaValidator))
    {
        rapidjson::StringBuffer sbSchema, sbDocument;
        schemaValidator.GetInvalidSchemaPointer().StringifyUriFragment(sbSchema);
        schemaValidator.GetInvalidDocumentPointer().StringifyUriFragment(sbDocument);

        std::string errorKeyWord;

        if (schemaValidator.GetInvalidSchemaKeyword() != nullptr)
            errorKeyWord = schemaValidator.GetInvalidSchemaKeyword();

        std::string what = fmt::format("Invalid schema: {}\nInvalid keyword: {}\nInvalid document: {}",
            sbSchema.GetString(),
            errorKeyWord,
            sbDocument.GetString());

        sbSchema.Clear();
        sbDocument.Clear();

        throw APIException(APIException::Type::schema, apiCall.url, what);
    }

    return jsonDoc;
}

void trader::api::BinanceAPI::secureRequest(cpr::Session &session, cpr::Parameters &parameters, const bool &preventSignature) noexcept
{
    cpr::Header header = cpr::Header { { "X-MBX-APIKEY", m_binanceKeys->apiKey } };
    session.SetHeader(header);

    // For USER_STREAM data.  This does not content any parameters
    if (!parameters.content.empty() && !preventSignature)
    {
        std::string signature = fmt::format("{}", HMAC256(m_binanceKeys->secretKey, parameters.content));
        cpr::CurlHolder holder;
        parameters.AddParameter({ "signature", signature }, holder);
    }
}

bool trader::api::BinanceAPI::handleAPIRequestError(const bool &data, const rapidjson::Value &value, int &code, std::string &message)
{
    if (data) /* Means a Binance WAPI call*/
    {
        // Error
        auto member = value.FindMember("success");
        auto msg    = value.FindMember("msg");
        if ((member != value.MemberEnd() && !value["success"].GetBool()) || (value.MemberCount() == 1 && msg != value.MemberEnd()))
        {
            code    = 0; // We don't have a code for
            message = value["msg"].GetString();
            return true;
        }
    }
    else
    {
        code    = value["code"].GetInt();
        message = value["msg"].GetString();
        return true;
    }
    return false;
}

void trader::api::BinanceAPI::handleHTTPErrorResponse(const std::string &jsonObject, int &code, std::string &message)
{
    // This actually return some JSON Data
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonObject.c_str());
    code    = jsonDoc["code"].GetInt();
    message = jsonDoc["msg"].GetString();
}

rapidjson::Document trader::api::BinanceAPI::apiRequest(const size_t &apiCall, const bool &secure)
{
    cpr::Parameters params;
    return request(&g_apiRequests[apiCall], params, secure, &m_lastCallSeconds);
}

rapidjson::Document trader::api::BinanceAPI::apiRequest(const size_t &apiCall, cpr::Parameters &parameters, const bool &secure)
{
    return request(&g_apiRequests[apiCall], parameters, secure, &m_lastCallSeconds);
}

rapidjson::Document trader::api::BinanceAPI::fapiRequest(const size_t &apiCall, const bool &secure)
{
    cpr::Parameters params;
    return request(&g_fapiRequests[apiCall], params, secure, &m_lastCallSeconds);
}

rapidjson::Document trader::api::BinanceAPI::fapiRequest(const size_t &apiCall, cpr::Parameters &parameters, const bool &secure)
{
    return request(&g_fapiRequests[apiCall], parameters, secure, &m_lastCallSeconds);
}

double trader::api::BinanceAPI::getLastCallTime() const noexcept
{
    return m_lastCallSeconds;
}

uint64_t trader::api::BinanceAPI::getTime() noexcept
{
    return static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

void trader::api::BinanceAPI::apiPing()
{
    apiRequest(TEST_CONNECTIVITY);
}

bool trader::api::BinanceAPI::apiExchangeStatus()
{
    auto doc = apiRequest(SYSTEM_STATUS);
    return doc["status"].GetInt() == 0;
}

std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> trader::api::BinanceAPI::getCandlesTimesAndLimits(const trader::api::BinanceTimeIntervals &interval, const uint64_t &startTime, const uint64_t &endTime, uint64_t &totalExpected)
{
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> data;

    // 'now' and 'then' must hold the time in milliseconds
    uint64_t now  = endTime,
             then = startTime;

    // 'now' and 'then' is in minutes so: 'now' and 'then' % MOD gives the
    // milliseconds that passed in a MOD milliseconds constrain in the time: example:
    // 12:34: 'now' and 'then' % 15000 will result in 4000.
    // We then subtract that difference in milliseconds
    // This way we don't have to worry from extraneous timestamps from the interval
    uint64_t mod = fromIntervalToMilliseconds(interval);

    // Make the necessary adjustments
    now  = now - (now % mod);
    then = then - (then % mod);

    // The interval is practically the mod
    const uint64_t &msInterval = mod;
    // Since all times must be already fixed to the nearest time, a mod operation is not needed

    // Compute the total number of items expected
    totalExpected = (now - then) / msInterval;

    // Max item count for the exchange is 1000 but what if we need 1030
    // We need to store those 30 elements as well
    const uint64_t lastItems       = totalExpected % 1000;
    const uint64_t itemsFittedIn1K = totalExpected / 1000ull;

    auto timeCounter               = startTime;
    for (auto i = 0ull; i < itemsFittedIn1K; ++i)
    {
        data.emplace_back(timeCounter, timeCounter + (msInterval * 1001), 1000);

        timeCounter = timeCounter + (msInterval * 1000);
    }
    data.emplace_back(timeCounter, 0, lastItems);

    return data;
}

std::vector<trader::api::Candlestick> trader::api::BinanceAPI::apiCandlestick(const std::string &symbol, const trader::api::BinanceTimeIntervals &interval, const size_t &startTime, const size_t &endTime, const size_t &limit)
{
    rapidjson::Document doc;
    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol },
        { "interval", std::string(fromIntervalToString(interval).data()) },
        { "startTime", fmt::format("{}", startTime) },
        { "limit", fmt::format("{}", limit) }
    };

    if (endTime != 0)
    {

        cpr::CurlHolder holder;
        parameters.AddParameter({ "endTime", fmt::format("{}", endTime) }, holder);
    }

    doc = apiRequest(KLINE_CANDLESTICK_DATA, parameters, false);

    std::vector<trader::api::Candlestick> cdl;
    for (const auto &candleData : doc.GetArray())
    {
        std::size_t i = 0;
        Candlestick csd {};
        for (const auto &candle : candleData.GetArray())
        {
            switch (i)
            {
                case 0:
                    csd.openTime = candle.GetUint64();
                    break;
                case 1:
                    csd.open = std::stod(candle.GetString());
                    break;
                case 2:
                    csd.high = std::stod(candle.GetString());
                    break;
                case 3:
                    csd.low = std::stod(candle.GetString());
                    break;
                case 4:
                    csd.close = std::stod(candle.GetString());
                    break;
                case 5:
                    csd.volume = std::stod(candle.GetString());
                    break;
                case 6:
                    csd.closeTime = candle.GetUint64();
                    break;
                case 7:
                    csd.quoteAssetVolume = std::stod(candle.GetString());
                    break;
                case 8:
                    csd.numberOfTrades = candle.GetUint64();
                    break;
                case 9:
                    csd.takerBaseAssetVolume = std::stod(candle.GetString());
                    break;
                case 10:
                    csd.takerQuoteAssetVolume = std::stod(candle.GetString());
                    break;
                default:
                    break;
            }
            ++i;
        }
        cdl.push_back(csd);
    }

    return cdl;
}

trader::api::ExchangeInformation trader::api::BinanceAPI::apiExchangeInformation()
{
    auto doc = apiRequest(EXCHANGE_INFO);

    ExchangeInformation exchangeInfo;
    // Parse all data
    exchangeInfo.timezone = std::string { doc["timezone"].GetString(), doc["timezone"].GetStringLength() };

    // Extract Limits
    for (const auto &limit : doc["rateLimits"].GetArray())
    {
        std::string type { limit["rateLimitType"].GetString(), limit["rateLimitType"].GetStringLength() };
        std::string interval { limit["interval"].GetString(), limit["interval"].GetStringLength() };
        uint32_t number         = limit["intervalNum"].GetUint();
        uint32_t rateLimit      = limit["limit"].GetUint();

        uint32_t intervalNumber = 0;

        if (interval == "MINUTE")
            intervalNumber = 60;
        else if (interval == "SECOND")
            intervalNumber = 1;
        else if (interval == "DAY")
            intervalNumber = 24 * 60 * 60;

        if (type == "REQUEST_WEIGHT")
        {
            exchangeInfo.limitWeight.seconds = intervalNumber * number;
            exchangeInfo.limitWeight.limit   = rateLimit;
        }
        else if (type == "ORDERS")
        {
            if (intervalNumber == 1)
            {
                // SECOND
                exchangeInfo.limitOrderSecond.seconds = intervalNumber * number;
                exchangeInfo.limitOrderSecond.limit   = rateLimit;
            }
            else
            {
                exchangeInfo.limitOrderDay.seconds = intervalNumber * number;
                exchangeInfo.limitOrderDay.limit   = rateLimit;
            }
        }
        else
        {
            // POSSIBLE BUG IF BINANCE CHANGE THE FORMAT OF THE OUTPUT DATA
        }
    }

    // Symbols
    for (const auto &symbol : doc["symbols"].GetArray())
    {
        ExchangeInformationSymbol sym;

        sym.symbol = { symbol["symbol"].GetString(), symbol["symbol"].GetStringLength() };

        // Strings are limited by the Schema
        if (std::string status = symbol["status"].GetString(); status == "TRADING")
            sym.status = TradingStatus::Trading;
        else
            sym.status = TradingStatus::Break;

        sym.baseAsset                = { symbol["baseAsset"].GetString(), symbol["baseAsset"].GetStringLength() };

        sym.baseAssetPrecision       = symbol["baseAssetPrecision"].GetInt();

        sym.quoteAsset               = { symbol["quoteAsset"].GetString(), symbol["quoteAsset"].GetStringLength() };

        sym.quotePrecision           = symbol["quotePrecision"].GetInt();
        sym.quoteAssetPrecision      = symbol["quoteAssetPrecision"].GetInt();
        sym.baseCommissionPrecision  = symbol["baseCommissionPrecision"].GetInt();
        sym.quoteCommissionPrecision = symbol["quoteCommissionPrecision"].GetInt();
        // Order Types
        for (const auto &orderTypes : symbol["orderTypes"].GetArray())
        {
            std::string type = orderTypes.GetString();
            if (type == "LIMIT")
                sym.orderType.set(static_cast<uint64_t>(OrderType::limit));
            else if (type == "LIMIT_MAKER")
                sym.orderType.set(static_cast<uint64_t>(OrderType::limitMaker));
            else if (type == "MARKET")
                sym.orderType.set(static_cast<uint64_t>(OrderType::market));
            else if (type == "STOP_LOSS_LIMIT")
                sym.orderType.set(static_cast<uint64_t>(OrderType::stopLossLimit));
            else if (type == "TAKE_PROFIT_LIMIT")
                sym.orderType.set(static_cast<uint64_t>(OrderType::takeProfitLimit));
            else
            {
                // The schema forbids this scenario
            }
        }
        sym.icebergAllowed             = symbol["icebergAllowed"].GetBool();
        sym.ocoAllowed                 = symbol["ocoAllowed"].GetBool();
        sym.quoteOrderQtyMarketAllowed = symbol["quoteOrderQtyMarketAllowed"].GetBool();
        // Filters
        for (const auto &filters : symbol["filters"].GetArray())
        {
            std::string filterType = filters["filterType"].GetString();

            if (filterType == "PRICE_FILTER")
            {
                sym.eifPrice.present  = true;
                sym.eifPrice.minPrice = std::stod(filters["minPrice"].GetString());
                sym.eifPrice.maxPrice = std::stod(filters["maxPrice"].GetString());
                sym.eifPrice.tickSize = std::stod(filters["tickSize"].GetString());
            }
            else if (filterType == "PERCENT_PRICE")
            {
                sym.eifPercentPrice.present          = true;
                sym.eifPercentPrice.multiplierUp     = std::stod(filters["multiplierUp"].GetString());
                sym.eifPercentPrice.multiplierDown   = std::stod(filters["multiplierDown"].GetString());
                sym.eifPercentPrice.averagePriceMins = filters["avgPriceMins"].GetInt();
            }
            else if (filterType == "LOT_SIZE")
            {
                sym.eifLotSize.present  = true;
                sym.eifLotSize.minQty   = std::stod(filters["minQty"].GetString());
                sym.eifLotSize.maxQty   = std::stod(filters["maxQty"].GetString());
                sym.eifLotSize.stepSize = std::stod(filters["stepSize"].GetString());
            }
            else if (filterType == "MIN_NOTIONAL")
            {
                sym.eifMinNotional.present          = true;
                sym.eifMinNotional.minNotional      = std::stod(filters["minNotional"].GetString());
                sym.eifMinNotional.applyToMarket    = filters["applyToMarket"].GetBool();
                sym.eifMinNotional.averagePriceMins = filters["avgPriceMins"].GetInt();
            }
            else if (filterType == "ICEBERG_PARTS")
            {
                sym.eifIcebergParts.present = true;
                sym.eifIcebergParts.limit   = filters["limit"].GetInt();
            }
            else if (filterType == "MARKET_LOT_SIZE")
            {
                sym.eifMarketLotSize.present  = true;
                sym.eifMarketLotSize.minQty   = std::stod(filters["minQty"].GetString());
                sym.eifMarketLotSize.maxQty   = std::stod(filters["maxQty"].GetString());
                sym.eifMarketLotSize.stepSize = std::stod(filters["stepSize"].GetString());
            }
            else if (filterType == "MAX_NUM_ALGO_ORDERS")
            {
                sym.eifMaxNumAlgoOrders.present          = true;
                sym.eifMaxNumAlgoOrders.maxNumAlgoOrders = filters["maxNumAlgoOrders"].GetInt();
            }
            else if (filterType == "MAX_NUM_ORDERS")
            {
                sym.eifMaxNumOrders.present      = true;
                sym.eifMaxNumOrders.maxNumOrders = filters["maxNumOrders"].GetInt();
            }
            else if (filterType == "MAX_NUM_ICEBERG_ORDERS")
            {
                sym.eifMaxNumIcerbergOrders.present             = true;
                sym.eifMaxNumIcerbergOrders.maxNumIcebergOrders = filters["maxNumIcebergOrders"].GetInt();
            }
            else if (filterType == "MAX_POSITION")
            {
                sym.eifMaxPosition.present     = true;
                sym.eifMaxPosition.maxPosition = std::stod(filters["maxPosition"].GetString());
            }
        }
        // Permissions
        for (const auto &permissions : symbol["permissions"].GetArray())
        {
            std::string type { permissions.GetString(), permissions.GetStringLength() };
            if (type == "SPOT")
                sym.permissions.set(AccountPermissions::SPOT);
            else if (type == "MARGIN")
                sym.permissions.set(AccountPermissions::MARGIN);
            else if (type == "LEVERAGED")
                sym.permissions.set(AccountPermissions::LEVERAGED);
            else
            {
                // The schema forbids this scenario
            }
        }

        // Add to list
        exchangeInfo.symbols[sym.symbol] = sym;
    }

    return exchangeInfo;
}

std::string trader::api::BinanceAPI::apiAccountStatus()
{
    cpr::Parameters parameters = cpr::Parameters {
        { "timestamp", fmt::format_int(getTime()).c_str() },
        { "recvWindow", "6000" }
    };

    auto doc = apiRequest(ACCOUNT_STATUS, parameters, true);

    return doc["data"].GetString();
}

trader::api::AccountInformation trader::api::BinanceAPI::apiAccountInformation()
{
    cpr::Parameters parameters = cpr::Parameters {
        { "timestamp", fmt::format_int(getTime()).c_str() },
        { "recvWindow", "6000" }
    };

    auto doc = apiRequest(ACCOUNT_INFORMATION, parameters, true);

    AccountInformation accInfo;

    accInfo.makerCommission  = doc["makerCommission"].GetUint64();
    accInfo.takerCommission  = doc["takerCommission"].GetUint64();
    accInfo.buyerCommission  = doc["buyerCommission"].GetUint64();
    accInfo.sellerCommission = doc["sellerCommission"].GetUint64();
    accInfo.canTrade         = doc["canTrade"].GetBool();
    accInfo.canWithdraw      = doc["canWithdraw"].GetBool();
    accInfo.canDeposit       = doc["canDeposit"].GetBool();

    accInfo.accountType      = doc["accountType"].GetString();

    for (auto &permissions : doc["permissions"].GetArray())
    {
        if (strcmp(permissions.GetString(), "SPOT") == 0)
            accInfo.permissions.set(api::SPOT);
        else if (strcmp(permissions.GetString(), "MARGIN") == 0)
            accInfo.permissions.set(api::MARGIN);
        else
            accInfo.permissions.set(api::LEVERAGED);
    }

    for (auto &balances : doc["balances"].GetArray())
    {
        currency_t free   = std::stod(balances["free"].GetString());
        currency_t locked = std::stod(balances["locked"].GetString());

        std::string asset = balances["asset"].GetString();
        std::transform(asset.begin(), asset.end(), asset.begin(), ::toupper);
        accInfo.balances[asset] = { free, locked };
    }

    return accInfo;
}

trader::api::Orderbook trader::api::BinanceAPI::apiOrderbook(const std::string &symbol, const api::OrderBookLimits &limits)
{

    const std::pair<std::string, size_t> limitInfo = [&limits]() -> std::pair<std::string, size_t> {
        switch (limits)
        {
            case OrderBookLimits::l5:
                return { "5", ORDER_BOOK_1 };
            case OrderBookLimits::l10:
                return { "10", ORDER_BOOK_1 };
            case OrderBookLimits::l20:
                return { "20", ORDER_BOOK_1 };
            case OrderBookLimits::l50:
                return { "50", ORDER_BOOK_1 };
            case OrderBookLimits::l100:
                return { "100", ORDER_BOOK_1 };
            case OrderBookLimits::l500:
                return { "500", ORDER_BOOK_5 };
            case OrderBookLimits::l1000:
                return { "1000", ORDER_BOOK_10 };
            case OrderBookLimits::l5000:
                return { "5000", ORDER_BOOK_50 };
        }
    }();

    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol },
        { "limit", limitInfo.first }
    };

    auto doc = apiRequest(limitInfo.second, parameters);

    Orderbook ob;
    ob.lastUpdateId = doc["lastUpdateId"].GetUint64();

    for (const auto &bids : doc["bids"].GetArray())
    {
        ob.bids.push_back({ std::string { bids[0].GetString(), bids[0].GetStringLength() },
            std::string { bids[1].GetString(), bids[1].GetStringLength() } });
    }
    for (const auto &asks : doc["asks"].GetArray())
    {
        ob.asks.push_back({ std::string { asks[0].GetString(), asks[0].GetStringLength() },
            std::string { asks[1].GetString(), asks[1].GetStringLength() } });
    }

    return ob;
}

std::string trader::api::BinanceAPI::apiListenKey()
{
    auto doc = apiRequest(LISTEN_KEY, true);

    return std::string { doc["listenKey"].GetString(), doc["listenKey"].GetStringLength() };
}

void trader::api::BinanceAPI::apiKeepListenKeyAlive(const std::string &listenKey)
{
    cpr::Parameters parameter = cpr::Parameters {
        { "listenKey", listenKey }
    };

    apiRequest(KEEP_ALIVE_LISTEN_KEY, parameter, true);
}

trader::api::QueryOrder trader::api::BinanceAPI::apiQueryOrder(const std::string &symbol, const std::size_t &timeWindow, const std::size_t &orderId)
{
    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol },
        { "orderId", fmt::format_int(orderId).c_str() },
        { "recvWindow", fmt::format_int(timeWindow).c_str() },
        { "timestamp", fmt::format_int(getTime()).c_str() }
    };

    auto doc = apiRequest(QUERY_ORDER, parameters, true);

    trader::api::QueryOrder quo;

    quo.symbol               = std::string { doc["symbol"].GetString(), doc["symbol"].GetStringLength() };
    quo.orderId              = doc["orderId"].GetUint64();
    quo.orderListId          = doc["orderListId"].GetInt64();
    quo.clientOrderId        = std::string { doc["clientOrderId"].GetString(), doc["clientOrderId"].GetStringLength() };
    quo.price                = std::stod(std::string { doc["price"].GetString(), doc["price"].GetStringLength() });
    quo.origQty              = std::stod(std::string { doc["origQty"].GetString(), doc["origQty"].GetStringLength() });
    quo.executedQty          = std::stod(std::string { doc["executedQty"].GetString(), doc["executedQty"].GetStringLength() });
    quo.cumulativeQuoteQty   = std::stod(std::string { doc["cummulativeQuoteQty"].GetString(), doc["cummulativeQuoteQty"].GetStringLength() });

    const std::string status = std::string { doc["status"].GetString(), doc["status"].GetStringLength() };
    if (status == "NEW")
        quo.status = OrderStatus::newOrder;
    else if (status == "PARTIALLY_FILLED")
        quo.status = OrderStatus::partiallyFilled;
    else if (status == "FILLED")
        quo.status = OrderStatus::filled;
    else if (status == "CANCELED")
        quo.status = OrderStatus::canceled;
    else if (status == "PENDING_CANCEL")
        quo.status = OrderStatus::pendingCancel;
    else if (status == "REJECTED")
        quo.status = OrderStatus::rejected;
    else if (status == "EXPIRED")
        quo.status = OrderStatus::expired;

    const std::string tif = std::string { doc["timeInForce"].GetString(), doc["timeInForce"].GetStringLength() };
    if (tif == "GTC")
        quo.timeInForce = OrderTimeInForce::GTC;
    else if (tif == "IOC")
        quo.timeInForce = OrderTimeInForce::IOC;
    else if (tif == "FOK")
        quo.timeInForce = OrderTimeInForce::FOK;

    const std::string ot = std::string { doc["type"].GetString(), doc["type"].GetStringLength() };
    if (ot == "LIMIT")
        quo.type = OrderType::limit;
    else if (ot == "MARKET")
        quo.type = OrderType::market;
    else if (ot == "STOP_LOSS")
        quo.type = OrderType::stopLoss;
    else if (ot == "STOP_LOSS_LIMIT")
        quo.type = OrderType::stopLossLimit;
    else if (ot == "TAKE_PROFIT")
        quo.type = OrderType::takeProfit;
    else if (ot == "TAKE_PROFIT_LIMIT")
        quo.type = OrderType::takeProfitLimit;
    else if (ot == "LIMIT_MAKER")
        quo.type = OrderType::limitMaker;

    const std::string side = std::string { doc["side"].GetString(), doc["side"].GetStringLength() };
    if (side == "BUY")
        quo.side = OrderSide::buy;
    else if (side == "SELL")
        quo.side = OrderSide::sell;

    quo.stopPrice         = std::stod(std::string { doc["stopPrice"].GetString(), doc["stopPrice"].GetStringLength() });
    quo.icebergQty        = std::stod(std::string { doc["icebergQty"].GetString(), doc["icebergQty"].GetStringLength() });
    quo.time              = doc["time"].GetUint64();
    quo.updateTime        = doc["updateTime"].GetUint64();
    quo.isWorking         = doc["isWorking"].GetBool();
    quo.origQuoteOrderQty = std::stod(std::string { doc["origQuoteOrderQty"].GetString(), doc["origQuoteOrderQty"].GetStringLength() });

    return quo;
}

trader::api::CancelOrder trader::api::BinanceAPI::apiCancelOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &orderId, const std::string &cancelOrderId)
{

    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol },
        { "origClientOrderId", orderId },
        { "newClientOrderId", cancelOrderId },
        { "recvWindow", fmt::format("{}", timeWindow) },
        { "timestamp", fmt::format_int(getTime()).c_str() }
    };

    auto doc = apiRequest(CANCEL_ORDER, parameters, true);

    trader::api::CancelOrder cao;

    cao.symbol               = std::string { doc["symbol"].GetString(), doc["symbol"].GetStringLength() };
    cao.orderId              = doc["orderId"].GetUint64();
    cao.orderListId          = doc["orderListId"].GetInt64();
    cao.cancelClientOrderId  = std::string { doc["clientOrderId"].GetString(), doc["clientOrderId"].GetStringLength() };
    cao.clientOrderId        = std::string { doc["origClientOrderId"].GetString(), doc["origClientOrderId"].GetStringLength() };
    cao.price                = std::stod(std::string { doc["price"].GetString(), doc["price"].GetStringLength() });
    cao.origQty              = std::stod(std::string { doc["origQty"].GetString(), doc["origQty"].GetStringLength() });
    cao.executedQty          = std::stod(std::string { doc["executedQty"].GetString(), doc["executedQty"].GetStringLength() });
    cao.cumulativeQuoteQty   = std::stod(std::string { doc["cummulativeQuoteQty"].GetString(), doc["cummulativeQuoteQty"].GetStringLength() });

    const std::string status = std::string { doc["status"].GetString(), doc["status"].GetStringLength() };

    if (status == "NEW")
        cao.status = OrderStatus::newOrder;
    else if (status == "PARTIALLY_FILLED")
        cao.status = OrderStatus::partiallyFilled;
    else if (status == "FILLED")
        cao.status = OrderStatus::filled;
    else if (status == "CANCELED")
        cao.status = OrderStatus::canceled;
    else if (status == "PENDING_CANCEL")
        cao.status = OrderStatus::pendingCancel;
    else if (status == "REJECTED")
        cao.status = OrderStatus::rejected;
    else if (status == "EXPIRED")
        cao.status = OrderStatus::expired;

    const std::string tif = std::string { doc["timeInForce"].GetString(), doc["timeInForce"].GetStringLength() };
    if (tif == "GTC")
        cao.timeInForce = OrderTimeInForce::GTC;
    else if (tif == "IOC")
        cao.timeInForce = OrderTimeInForce::IOC;
    else if (tif == "FOK")
        cao.timeInForce = OrderTimeInForce::FOK;

    const std::string ot = std::string { doc["type"].GetString(), doc["type"].GetStringLength() };
    if (ot == "LIMIT")
        cao.type = OrderType::limit;
    else if (ot == "MARKET")
        cao.type = OrderType::market;
    else if (ot == "STOP_LOSS")
        cao.type = OrderType::stopLoss;
    else if (ot == "STOP_LOSS_LIMIT")
        cao.type = OrderType::stopLossLimit;
    else if (ot == "TAKE_PROFIT")
        cao.type = OrderType::takeProfit;
    else if (ot == "TAKE_PROFIT_LIMIT")
        cao.type = OrderType::takeProfitLimit;
    else if (ot == "LIMIT_MAKER")
        cao.type = OrderType::limitMaker;

    const std::string side = std::string { doc["side"].GetString(), doc["side"].GetStringLength() };
    if (side == "BUY")
        cao.side = OrderSide::buy;
    else if (side == "SELL")
        cao.side = OrderSide::sell;

    return cao;
}

trader::api::currency_t trader::api::BinanceAPI::apiAveragePrice(const std::string &symbol)
{

    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol }
    };

    auto doc = apiRequest(CURRENT_AVERAGE_PRICE, parameters, false);

    return std::stod(std::string { doc["price"].GetString(), doc["price"].GetStringLength() });
}

static auto getOrderSide(const trader::api::OrderSide &side) -> std::string
{
    switch (side)
    {
        case trader::api::OrderSide::buy:
            return "BUY";
        case trader::api::OrderSide::sell:
            return "SELL";
    }
}

trader::api::ServerSideOrder trader::api::BinanceAPI::apiCreateLimitOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &tradeId, const trader::api::OrderSide &side, const trader::api::currency_t &price, const trader::api::currency_t &quantity, const trader::api::OrderTimeInForce &tif, const ExchangeInformationSymbol &symInfo, const bool &testOnly)
{
    const std::string orderSide = getOrderSide(side);
    const std::string orderTIF  = [&tif]() -> std::string {
        switch (tif)
        {
            case OrderTimeInForce::GTC:
                return "GTC";
            case OrderTimeInForce::IOC:
                return "IOC";
            case OrderTimeInForce::FOK:
                return "FOK";
            case OrderTimeInForce::GTX:
                return ""; // GTX is not supported in SPOT
        }
    }();

    ServerSideOrder sst;

    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol },
        { "side", orderSide },
        { "type", "LIMIT" },
        { "newOrderRespType", "ACK" },
        { "newClientOrderId", tradeId },
        { "recvWindow", fmt::to_string(timeWindow) },
        { "timeInForce", orderTIF },
        { "quantity", fmt::format("{:.{}f}", quantity, symInfo.baseAssetPrecision) },
        { "price", fmt::format("{:.{}f}", price, symInfo.baseAssetPrecision) },
        { "timestamp", fmt::format_int(getTime()).c_str() }
    };

    auto doc = apiRequest(
        testOnly ? TEST_NEW_ORDER : NEW_ORDER,
        parameters,
        true);

    if (!testOnly)
    {
        sst.symbol          = symbol;
        sst.orderId         = doc["orderId"].GetUint64();
        sst.clientOrderId   = std::string { doc["clientOrderId"].GetString(), doc["clientOrderId"].GetStringLength() };
        sst.transactionTime = doc["transactTime"].GetUint64();
    }
    else
    {
        sst.symbol          = symbol;
        sst.orderId         = 0;
        sst.clientOrderId   = tradeId;
        sst.transactionTime = getTime();
    }

    return sst;
}

trader::api::ServerSideOrder trader::api::BinanceAPI::apiCreateMarketOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &tradeId, const trader::api::OrderSide &side, const trader::api::currency_t &quantity, const ExchangeInformationSymbol &symInfo, const bool &testOnly)
{
    const std::string orderSide = getOrderSide(side);
    ServerSideOrder sst;

    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol },
        { "side", orderSide },
        { "type", "MARKET" },
        { "newOrderRespType", "ACK" },
        { "newClientOrderId", tradeId },
        { "recvWindow", fmt::to_string(timeWindow) },
        { "quantity", fmt::format("{:.{}f}", quantity, symInfo.baseAssetPrecision) },
        { "timestamp", fmt::format_int(getTime()).c_str() }
    };

    auto doc = apiRequest(
        testOnly ? TEST_NEW_ORDER : NEW_ORDER,
        parameters,
        true);

    if (!testOnly)
    {
        sst.symbol          = symbol;
        sst.orderId         = doc["orderId"].GetUint64();
        sst.clientOrderId   = std::string { doc["clientOrderId"].GetString(), doc["clientOrderId"].GetStringLength() };
        sst.transactionTime = doc["transactTime"].GetUint64();
    }
    else
    {
        sst.symbol          = symbol;
        sst.orderId         = 0;
        sst.clientOrderId   = tradeId;
        sst.transactionTime = getTime();
    }

    return sst;
}

trader::api::ServerSideOrder trader::api::BinanceAPI::apiCreateLimitMakerOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &tradeId, const trader::api::currency_t &price, const trader::api::currency_t &quantity, const trader::api::OrderSide &side, const ExchangeInformationSymbol &symInfo, const bool &testOnly)
{
    const std::string orderSide = getOrderSide(side);

    ServerSideOrder sst;

    cpr::CurlHolder curlHolder;
    cpr::Parameters parameters = cpr::Parameters {
        { "symbol", symbol },
        { "side", orderSide },
        { "type", "LIMIT_MAKER" },
        { "newOrderRespType", "ACK" },
        { "newClientOrderId", tradeId },
        { "recvWindow", fmt::to_string(timeWindow) },
        { "timestamp", fmt::format_int(getTime()).c_str() },
        { "quantity", fmt::format("{:.{}f}", quantity, symInfo.baseAssetPrecision) },
        { "price", fmt::format("{:.{}f}", price, symInfo.baseAssetPrecision) }
    };

    auto doc = apiRequest(
        testOnly ? TEST_NEW_ORDER : NEW_ORDER,
        parameters,
        true);

    if (!testOnly)
    {
        sst.symbol          = symbol;
        sst.orderId         = doc["orderId"].GetUint64();
        sst.clientOrderId   = std::string { doc["clientOrderId"].GetString(), doc["clientOrderId"].GetStringLength() };
        sst.transactionTime = doc["transactTime"].GetUint64();
    }
    else
    {
        sst.symbol          = symbol;
        sst.orderId         = 0;
        sst.clientOrderId   = tradeId;
        sst.transactionTime = getTime();
    }

    return sst;
}

void trader::api::BinanceAPI::fapiPing()
{
    fapiRequest(F_PING);
}

uint64_t trader::api::BinanceAPI::fapiGetServerTime()
{
    auto time = fapiRequest(F_SERVER_TIME);
    return time["serverTime"].GetUint64();
}

trader::api::FuturesExchangeInformation trader::api::BinanceAPI::fapiGetExchangeInformation()
{
    auto exchInfo = fapiRequest(F_EXCHANGE_INFORMATION);
    FuturesExchangeInformation fei;

    fei.serverTime  = exchInfo["serverTime"].GetUint64();
    fei.futuresType = exchInfo["futuresType"].GetString();

    // Extract Limits
    for (const auto &limit : exchInfo["rateLimits"].GetArray())
    {
        std::string type { limit["rateLimitType"].GetString(), limit["rateLimitType"].GetStringLength() };
        std::string interval { limit["interval"].GetString(), limit["interval"].GetStringLength() };
        uint32_t number         = limit["intervalNum"].GetUint();
        uint32_t rateLimit      = limit["limit"].GetUint();

        uint32_t intervalNumber = 0;

        if (interval == "MINUTE")
            intervalNumber = 60;
        else if (interval == "SECOND")
            intervalNumber = 1;
        else if (interval == "DAY")
            intervalNumber = 24 * 60 * 60;

        if (type == "REQUEST_WEIGHT")
        {
            fei.limitWeight.seconds = intervalNumber * number;
            fei.limitWeight.limit   = rateLimit;
        }
        else if (type == "ORDERS")
        {
            if (intervalNumber == 1)
            {
                // SECOND
                fei.limitOrderSecond.seconds = intervalNumber * number;
                fei.limitOrderSecond.limit   = rateLimit;
            }
            else
            {
                fei.limitOrderMinute.seconds = intervalNumber * number;
                fei.limitOrderMinute.limit   = rateLimit;
            }
        }
        else
        {
            // POSSIBLE BUG IF BINANCE CHANGE THE FORMAT OF THE OUTPUT DATA
        }
    }

    for (const auto &assets : exchInfo["assets"].GetArray())
    {
        FutureAssets fAssets;

        fAssets.asset             = std::string { assets["asset"].GetString(), assets["asset"].GetStringLength() };
        fAssets.marginAvailable   = assets["marginAvailable"].GetBool();
        fAssets.autoAssetExchange = std::stod(std::string { assets["autoAssetExchange"].GetString(), assets["autoAssetExchange"].GetStringLength() });
        fei.assets[fAssets.asset] = fAssets;
    }

    for (const auto &symbol : exchInfo["symbols"].GetArray())
    {
        FuturesExchangeInformationSymbol feis;

        feis.symbol       = JTO_STRING(symbol, "symbol");
        feis.pair         = JTO_STRING(symbol, "pair");

        auto contractType = JTO_STRING(symbol, "contractType");
        if (contractType == "PERPETUAL")
            feis.contractType = ContractType::PERPETUAL;
        else if (contractType == "CURRENT_QUARTER")
            feis.contractType = ContractType::CURRENT_QUARTER;
        else
            feis.contractType = ContractType::none;

        feis.deliveryDate = symbol["deliveryDate"].GetUint64();
        feis.onboardDate  = symbol["onboardDate"].GetUint64();

        auto status       = JTO_STRING(symbol, "status");
        if (status == "TRADING")
            feis.status = FutureSymbolStatus::TRADING;
        else if (status == "PENDING_TRADING")
            feis.status = FutureSymbolStatus::PENDING_TRADING;

        feis.maintMarginPercent    = std::stod(JTO_STRING(symbol, "maintMarginPercent"));
        feis.requiredMarginPercent = std::stod(JTO_STRING(symbol, "requiredMarginPercent"));

        feis.baseAsset             = JTO_STRING(symbol, "baseAsset");
        feis.quoteAsset            = JTO_STRING(symbol, "quoteAsset");
        feis.marginAsset           = JTO_STRING(symbol, "marginAsset");

        feis.pricePrecision        = symbol["pricePrecision"].GetInt64();
        feis.quantityPrecision     = symbol["quantityPrecision"].GetInt64();
        feis.baseAssetPrecision    = symbol["baseAssetPrecision"].GetInt64();
        feis.quotePrecision        = symbol["quotePrecision"].GetInt64();

        auto underlyingType        = JTO_STRING(symbol, "underlyingType");

        if (underlyingType == "COIN")
            feis.underlyingType = FutureUnderlyingType::COIN;
        else if (underlyingType == "INDEX")
            feis.underlyingType = FutureUnderlyingType::INDEX;

        for (const auto &subtype : symbol["underlyingSubType"].GetArray())
        {
            auto st = std::string { subtype.GetString(), subtype.GetStringLength() };
            if (st == "DEFI")
                feis.underlyingSubType.push_back(FutureUnderlyingSubType::DEFI);
            else if (st == "HOT")
                feis.underlyingSubType.push_back(FutureUnderlyingSubType::HOT);
            else if (st == "BSC")
                feis.underlyingSubType.push_back(FutureUnderlyingSubType::BSC);
            else if (st == "NFT")
                feis.underlyingSubType.push_back(FutureUnderlyingSubType::NFT);
            else if (st == "INDEX")
                feis.underlyingSubType.push_back(FutureUnderlyingSubType::INDEX);
        }

        feis.settlePlan      = symbol["settlePlan"].GetInt64();
        feis.triggerProtect  = std::stod(JTO_STRING(symbol, "triggerProtect"));
        feis.liquidationFee  = std::stod(JTO_STRING(symbol, "liquidationFee"));
        feis.marketTakeBound = std::stod(JTO_STRING(symbol, "marketTakeBound"));

        for (const auto &filters : symbol["filters"].GetArray())
        {
            std::string filterType = filters["filterType"].GetString();

            if (filterType == "PRICE_FILTER")
            {
                feis.eifPrice.present  = true;
                feis.eifPrice.minPrice = std::stod(filters["minPrice"].GetString());
                feis.eifPrice.maxPrice = std::stod(filters["maxPrice"].GetString());
                feis.eifPrice.tickSize = std::stod(filters["tickSize"].GetString());
            }
            else if (filterType == "PERCENT_PRICE")
            {
                feis.eifPercentPrice.present           = true;
                feis.eifPercentPrice.multiplierUp      = std::stod(filters["multiplierUp"].GetString());
                feis.eifPercentPrice.multiplierDown    = std::stod(filters["multiplierDown"].GetString());
                feis.eifPercentPrice.multiplierDecimal = std::stod(filters["multiplierDecimal"].GetString());
            }
            else if (filterType == "LOT_SIZE")
            {
                feis.eifLotSize.present  = true;
                feis.eifLotSize.minQty   = std::stod(filters["minQty"].GetString());
                feis.eifLotSize.maxQty   = std::stod(filters["maxQty"].GetString());
                feis.eifLotSize.stepSize = std::stod(filters["stepSize"].GetString());
            }
            else if (filterType == "MIN_NOTIONAL")
            {
                feis.eifMinNotional.present     = true;
                feis.eifMinNotional.minNotional = std::stod(filters["notional"].GetString());
            }

            else if (filterType == "MARKET_LOT_SIZE")
            {
                feis.eifMarketLotSize.present  = true;
                feis.eifMarketLotSize.minQty   = std::stod(filters["minQty"].GetString());
                feis.eifMarketLotSize.maxQty   = std::stod(filters["maxQty"].GetString());
                feis.eifMarketLotSize.stepSize = std::stod(filters["stepSize"].GetString());
            }
            else if (filterType == "MAX_NUM_ALGO_ORDERS")
            {
                feis.eifMaxNumAlgoOrders.present          = true;
                feis.eifMaxNumAlgoOrders.maxNumAlgoOrders = filters["limit"].GetInt();
            }
            else if (filterType == "MAX_NUM_ORDERS")
            {
                feis.eifMaxNumOrders.present      = true;
                feis.eifMaxNumOrders.maxNumOrders = filters["limit"].GetInt();
            }
        }

        for (const auto &jot : symbol["orderTypes"].GetArray())
        {
            auto ot = std::string { jot.GetString(), jot.GetStringLength() };
            if (ot == "LIMIT")
                feis.orderTypes.insert(OrderType::limit);
            else if (ot == "MARKET")
                feis.orderTypes.insert(OrderType::market);
            else if (ot == "STOP")
                feis.orderTypes.insert(OrderType::stop);
            else if (ot == "STOP_MARKET")
                feis.orderTypes.insert(OrderType::stopMarket);
            else if (ot == "TAKE_PROFIT")
                feis.orderTypes.insert(OrderType::takeProfit);
            else if (ot == "TAKE_PROFIT_MARKET")
                feis.orderTypes.insert(OrderType::takeProfitMarket);
            else if (ot == "TRAILING_STOP_MARKET")
                feis.orderTypes.insert(OrderType::trailingStopMarket);
        }

        for (const auto &jtif : symbol["timeInForce"].GetArray())
        {
            auto tif = std::string { jtif.GetString(), jtif.GetStringLength() };
            if (tif == "GTC")
                feis.orderTimeInForce.insert(OrderTimeInForce::GTC);
            else if (tif == "IOC")
                feis.orderTimeInForce.insert(OrderTimeInForce::IOC);
            else if (tif == "FOK")
                feis.orderTimeInForce.insert(OrderTimeInForce::FOK);
            else if (tif == "GTX")
                feis.orderTimeInForce.insert(OrderTimeInForce::GTX);
        }
        fei.symbols[feis.symbol] = feis;
    }

    return fei;
}

trader::api::FuturesOrderBook trader::api::BinanceAPI::fapiGetOrderBook(const std::string &symbol, const OrderBookLimits &limit)
{
    assert(limit != OrderBookLimits::l5000);

    const std::pair<std::string, std::size_t> limitInfo = [&limit]() -> std::pair<std::string, std::size_t> {
        switch (limit)
        {
            case OrderBookLimits::l5:
                return { "5", F_ORDER_BOOK_2 };
            case OrderBookLimits::l10:
                return { "10", F_ORDER_BOOK_2 };
            case OrderBookLimits::l20:
                return { "20", F_ORDER_BOOK_2 };
            case OrderBookLimits::l50:
                return { "50", F_ORDER_BOOK_2 };
            case OrderBookLimits::l100:
                return { "100", F_ORDER_BOOK_5 };
            case OrderBookLimits::l500:
                return { "500", F_ORDER_BOOK_10 };
            case OrderBookLimits::l1000:
                return { "1000", F_ORDER_BOOK_20 };
            case OrderBookLimits::l5000:
                return { "", 0 }; // Not supported under the Futures API
        }
    }();

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", limitInfo.first }
    };

    auto doc = fapiRequest(limitInfo.second, cpr);

    trader::api::FuturesOrderBook ob;

    ob.lastTimeUpdate    = doc["lastUpdateId"].GetUint64();
    ob.messageOutputTime = doc["E"].GetUint64();
    ob.transactionTime   = doc["T"].GetUint64();

    for (const auto &bids : doc["bids"].GetArray())
    {
        ob.bids.push_back({ std::string { bids[0].GetString(), bids[0].GetStringLength() },
            std::string { bids[1].GetString(), bids[1].GetStringLength() } });
    }
    for (const auto &asks : doc["asks"].GetArray())
    {
        ob.asks.push_back({ std::string { asks[0].GetString(), asks[0].GetStringLength() },
            std::string { asks[1].GetString(), asks[1].GetStringLength() } });
    }

    return ob;
}

std::vector<trader::api::FuturesRecentTradesList> trader::api::BinanceAPI::fapiRecentTradesList(const std::string &symbol, const int64_t &limit)
{
    std::vector<trader::api::FuturesRecentTradesList> frtl;
    const std::string cLimit = [&limit]() -> std::string {
        if (limit > 1000)
            return "1000";
        else if (limit <= 0)
            return "500";
        else
            return fmt::to_string(limit);
    }();

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", cLimit }
    };

    auto doc = fapiRequest(F_RECENT_TRADES_LIST, cpr);

    for (const auto &list : doc.GetArray())
    {
        trader::api::FuturesRecentTradesList fr {
            .id            = list["id"].GetInt64(),
            .price         = std::stod(JTO_STRING(list, "price")),
            .quantity      = std::stod(JTO_STRING(list, "qty")),
            .quoteQuantity = std::stod(JTO_STRING(list, "quoteQty")),
            .time          = list["time"].GetUint64(),
            .isBuyerMaker  = list["isBuyerMaker"].GetBool()
        };
        frtl.push_back(fr);
    }

    return frtl;
}

std::vector<trader::api::FuturesRecentTradesList> trader::api::BinanceAPI::fapiOldTradesLookup(const std::string &symbol, const int &limit)
{
    std::vector<trader::api::FuturesRecentTradesList> frtl;
    const std::string cLimit = [&limit]() -> std::string {
        if (limit > 1000)
            return "1000";
        else if (limit <= 0)
            return "500";
        else
            return fmt::to_string(limit);
    }();

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", cLimit }
    };

    auto doc = fapiRequest(F_OLD_TRADES_LOOKUP, cpr, true);

    for (const auto &list : doc.GetArray())
    {
        trader::api::FuturesRecentTradesList fr {
            .id            = list["id"].GetInt64(),
            .price         = std::stod(JTO_STRING(list, "price")),
            .quantity      = std::stod(JTO_STRING(list, "qty")),
            .quoteQuantity = std::stod(JTO_STRING(list, "quoteQty")),
            .time          = list["time"].GetUint64(),
            .isBuyerMaker  = list["isBuyerMaker"].GetBool()
        };
        frtl.push_back(fr);
    }

    return frtl;
}

std::vector<trader::api::FuturesRecentTradesList> trader::api::BinanceAPI::fapiOldTradesLookup(const std::string &symbol, const uint64_t &id)
{
    std::vector<trader::api::FuturesRecentTradesList> frtl;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "fromId", fmt::to_string(id) }
    };

    auto doc = fapiRequest(F_OLD_TRADES_LOOKUP, cpr, true);

    for (const auto &list : doc.GetArray())
    {
        trader::api::FuturesRecentTradesList fr {
            .id            = list["id"].GetInt64(),
            .price         = std::stod(JTO_STRING(list, "price")),
            .quantity      = std::stod(JTO_STRING(list, "qty")),
            .quoteQuantity = std::stod(JTO_STRING(list, "quoteQty")),
            .time          = list["time"].GetUint64(),
            .isBuyerMaker  = list["isBuyerMaker"].GetBool()
        };
        frtl.push_back(fr);
    }

    return frtl;
}

std::vector<trader::api::FuturesCompressedAggregateTradeLists> trader::api::BinanceAPI::fapiCompressedAggregateTradesLists(const std::string &symbol, const int &limit, const uint64_t &startTime, const uint64_t &endTime, const int64_t &fromId)
{
    std::vector<FuturesCompressedAggregateTradeLists> fcatl;

    const std::string cLimit = [&limit]() -> std::string {
        if (limit > 1000)
            return "1000";
        else if (limit <= 0)
            return "500";
        else
            return fmt::to_string(limit);
    }();

    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", cLimit }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);

    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    if (fromId > 0)
        cpr.AddParameter({ "fromId", fmt::to_string(fromId) }, holder);

    auto doc = fapiRequest(F_COMPRESSED_AGGREGATE_TRADES_LIST, cpr);

    for (const auto &list : doc.GetArray())
    {
        trader::api::FuturesCompressedAggregateTradeLists fr {
            .id           = list["a"].GetInt64(),
            .price        = std::stod(JTO_STRING(list, "p")),
            .quantity     = std::stod(JTO_STRING(list, "q")),
            .firstTradeId = list["f"].GetInt64(),
            .lastTradeId  = list["l"].GetInt64(),
            .timestamp    = list["T"].GetUint64(),
            .isBuyerMaker = list["m"].GetBool()
        };
        fcatl.push_back(fr);
    }

    return fcatl;
}

std::vector<trader::api::Candlestick> trader::api::BinanceAPI::fapiCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit, const uint64_t startTime, const uint64_t endTime)
{
    const std::pair<std::string, size_t> limitInfo = [&limit]() -> std::pair<std::string, size_t> {
        if (limit >= 1 && limit < 100)
            return { fmt::to_string(limit), F_KLINE_1 };
        if (limit >= 100 && limit < 500)
            return { fmt::to_string(limit), F_KLINE_2 };
        if (limit >= 500 && limit <= 1000)
            return { fmt::to_string(limit), F_KLINE_5 };
        if (limit > 1000)
            return { fmt::to_string(limit), F_KLINE_10 };
        return { "", 0 };
    }();

    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", limitInfo.first },
        { "interval", std::string(fromIntervalToString(interval)) }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);

    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(limitInfo.second, cpr);

    std::vector<trader::api::Candlestick> cdl;
    for (const auto &candleData : doc.GetArray())
    {
        std::size_t i = 0;
        Candlestick csd {};
        for (const auto &candle : candleData.GetArray())
        {
            switch (i)
            {
                case 0:
                    csd.openTime = candle.GetUint64();
                    break;
                case 1:
                    csd.open = std::stod(candle.GetString());
                    break;
                case 2:
                    csd.high = std::stod(candle.GetString());
                    break;
                case 3:
                    csd.low = std::stod(candle.GetString());
                    break;
                case 4:
                    csd.close = std::stod(candle.GetString());
                    break;
                case 5:
                    csd.volume = std::stod(candle.GetString());
                    break;
                case 6:
                    csd.closeTime = candle.GetUint64();
                    break;
                case 7:
                    csd.quoteAssetVolume = std::stod(candle.GetString());
                    break;
                case 8:
                    csd.numberOfTrades = candle.GetUint64();
                    break;
                case 9:
                    csd.takerBaseAssetVolume = std::stod(candle.GetString());
                    break;
                case 10:
                    csd.takerQuoteAssetVolume = std::stod(candle.GetString());
                    break;
                default:
                    break;
            }
            ++i;
        }
        cdl.push_back(csd);
    }

    return cdl;
}

std::vector<trader::api::Candlestick> trader::api::BinanceAPI::fapiContinuesContractCandlestick(const std::string &symbol, const FuturesContractType &contractType, const BinanceTimeIntervals &interval, const int &limit, const uint64_t startTime, const uint64_t endTime)
{
    const std::pair<std::string, size_t> limitInfo = [&limit]() -> std::pair<std::string, size_t> {
        if (limit >= 1 && limit < 100)
            return { fmt::to_string(limit), F_CONTINUES_CONTRACT_KLINE_1 };
        if (limit >= 100 && limit < 500)
            return { fmt::to_string(limit), F_CONTINUES_CONTRACT_KLINE_2 };
        if (limit >= 500 && limit <= 1000)
            return { fmt::to_string(limit), F_CONTINUES_CONTRACT_KLINE_5 };
        if (limit > 1000)
            return { fmt::to_string(limit), F_CONTINUES_CONTRACT_KLINE_10 };
        return { "", 0 };
    }();

    const std::string contractTypeString = [&contractType]() -> std::string {
        switch (contractType)
        {
            case FuturesContractType::perpetual:
                return "PERPETUAL";
            case FuturesContractType::currentMonth:
                return "CURRENT_MONTH";
            case FuturesContractType::nextMonth:
                return "NEXT_MONTH";
            case FuturesContractType::currentQuarter:
                return "CURRENT_QUARTER";
            case FuturesContractType::nextQuarter:
                return "NEXT_QUARTER";
        }
    }();

    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", limitInfo.first },
        { "contractType", contractTypeString },
        { "interval", std::string(fromIntervalToString(interval)) }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);

    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(limitInfo.second, cpr);

    std::vector<trader::api::Candlestick> cdl;
    for (const auto &candleData : doc.GetArray())
    {
        std::size_t i = 0;
        Candlestick csd {};
        for (const auto &candle : candleData.GetArray())
        {
            switch (i)
            {
                case 0:
                    csd.openTime = candle.GetUint64();
                    break;
                case 1:
                    csd.open = std::stod(candle.GetString());
                    break;
                case 2:
                    csd.high = std::stod(candle.GetString());
                    break;
                case 3:
                    csd.low = std::stod(candle.GetString());
                    break;
                case 4:
                    csd.close = std::stod(candle.GetString());
                    break;
                case 5:
                    csd.volume = std::stod(candle.GetString());
                    break;
                case 6:
                    csd.closeTime = candle.GetUint64();
                    break;
                case 7:
                    csd.quoteAssetVolume = std::stod(candle.GetString());
                    break;
                case 8:
                    csd.numberOfTrades = candle.GetUint64();
                    break;
                case 9:
                    csd.takerBaseAssetVolume = std::stod(candle.GetString());
                    break;
                case 10:
                    csd.takerQuoteAssetVolume = std::stod(candle.GetString());
                    break;
                default:
                    break;
            }
            ++i;
        }
        cdl.push_back(csd);
    }

    return cdl;
}

std::vector<trader::api::Candlestick> trader::api::BinanceAPI::fapiIndexPriceCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit, const uint64_t startTime, const uint64_t endTime)
{
    const std::pair<std::string, size_t> limitInfo = [&limit]() -> std::pair<std::string, size_t> {
        if (limit >= 1 && limit < 100)
            return { fmt::to_string(limit), F_INDEX_PRICE_KLINE_1 };
        if (limit >= 100 && limit < 500)
            return { fmt::to_string(limit), F_INDEX_PRICE_KLINE_2 };
        if (limit >= 500 && limit <= 1000)
            return { fmt::to_string(limit), F_INDEX_PRICE_KLINE_5 };
        if (limit > 1000)
            return { fmt::to_string(limit), F_INDEX_PRICE_KLINE_10 };
        return { "", 0 };
    }();

    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", limitInfo.first },
        { "interval", std::string(fromIntervalToString(interval)) }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);

    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(limitInfo.second, cpr);

    std::vector<trader::api::Candlestick> cdl;
    for (const auto &candleData : doc.GetArray())
    {
        std::size_t i = 0;
        Candlestick csd {};
        for (const auto &candle : candleData.GetArray())
        {
            switch (i)
            {
                case 0:
                    csd.openTime = candle.GetUint64();
                    break;
                case 1:
                    csd.open = std::stod(candle.GetString());
                    break;
                case 2:
                    csd.high = std::stod(candle.GetString());
                    break;
                case 3:
                    csd.low = std::stod(candle.GetString());
                    break;
                case 4:
                    csd.close = std::stod(candle.GetString());
                    break;
                case 6:
                    csd.closeTime = candle.GetUint64();
                    break;
                case 8:
                    csd.numberOfTrades = candle.GetUint64();
                    break;
                case 5:
                    [[fallthrough]];
                case 7:
                    [[fallthrough]];
                case 9:
                    [[fallthrough]];
                case 10:
                    [[fallthrough]];
                default:
                    break;
            }
            ++i;
        }
        cdl.push_back(csd);
    }

    return cdl;
}

std::vector<trader::api::Candlestick> trader::api::BinanceAPI::fapiMarkPriceCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit, const uint64_t startTime, const uint64_t endTime)
{
    const std::pair<std::string, size_t> limitInfo = [&limit]() -> std::pair<std::string, size_t> {
        if (limit >= 1 && limit < 100)
            return { fmt::to_string(limit), F_MARK_PRICE_KLINE_1 };
        if (limit >= 100 && limit < 500)
            return { fmt::to_string(limit), F_MARK_PRICE_KLINE_2 };
        if (limit >= 500 && limit <= 1000)
            return { fmt::to_string(limit), F_MARK_PRICE_KLINE_5 };
        if (limit > 1000)
            return { fmt::to_string(limit), F_MARK_PRICE_KLINE_10 };
        return { "", 0 };
    }();

    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", limitInfo.first },
        { "interval", std::string(fromIntervalToString(interval)) }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);

    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(limitInfo.second, cpr);

    std::vector<trader::api::Candlestick> cdl;
    for (const auto &candleData : doc.GetArray())
    {
        std::size_t i = 0;
        Candlestick csd {};
        for (const auto &candle : candleData.GetArray())
        {
            switch (i)
            {
                case 0:
                    csd.openTime = candle.GetUint64();
                    break;
                case 1:
                    csd.open = std::stod(candle.GetString());
                    break;
                case 2:
                    csd.high = std::stod(candle.GetString());
                    break;
                case 3:
                    csd.low = std::stod(candle.GetString());
                    break;
                case 4:
                    csd.close = std::stod(candle.GetString());
                    break;
                case 6:
                    csd.closeTime = candle.GetUint64();
                    break;
                case 8:
                    csd.numberOfTrades = candle.GetUint64();
                    break;
                case 5:
                    [[fallthrough]];
                case 7:
                    [[fallthrough]];
                case 9:
                    [[fallthrough]];
                case 10:
                    [[fallthrough]];
                default:
                    break;
            }
            ++i;
        }
        cdl.push_back(csd);
    }

    return cdl;
}

trader::api::MarkPrice trader::api::BinanceAPI::fapiMarkPrice(const std::string &symbol)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol }
    };

    auto doc = fapiRequest(F_MARK_PRICE, cpr);

    trader::api::MarkPrice mp {
        .symbol          = JTO_STRING(doc, "symbol"),
        .markPrice       = std::stod(JTO_STRING(doc, "markPrice")),
        .indexPrice      = std::stod(JTO_STRING(doc, "indexPrice")),
        .lastFundingRate = std::stod(JTO_STRING(doc, "lastFundingRate")),
        .nextFundingTime = doc["nextFundingTime"].GetUint64(),
        .interestRate    = std::stod(JTO_STRING(doc, "interestRate")),
        .time            = doc["time"].GetUint64()
    };

    return mp;
}

std::vector<trader::api::FundingRate> trader::api::BinanceAPI::fapiGetFundingRateHistory(const std::string &symbol, const int &limit, const uint64_t &startTime, const uint64_t &endTime)
{
    assert(limit <= 1000);

    std::vector<FundingRate> vfr;

    bool parameters = false;
    cpr::CurlHolder holder;
    cpr::Parameters cpr;

    if (!symbol.empty())
    {
        parameters = true;
        cpr.AddParameter({ "symbol", symbol }, holder);
    }

    if (limit > 0)
    {
        parameters = true;
        cpr.AddParameter({ "limit", fmt::to_string(limit) }, holder);
    }

    if (startTime > 0)
    {
        parameters = true;
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    }

    if (endTime > 0)
    {
        parameters = true;
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);
    }

    auto doc = [&]() {
        return !parameters ? fapiRequest(F_FUNDING_RATE_HISTORY) : fapiRequest(F_FUNDING_RATE_HISTORY, cpr);
    }();

    for (const auto &list : doc.GetArray())
    {
        FundingRate fr {
            .symbol      = JTO_STRING(list, "symbol"),
            .fundingRate = std::stod(JTO_STRING(list, "fundingRate")),
            .fundingTime = list["fundingTime"].GetUint64()
        };
        vfr.push_back(fr);
    }

    return vfr;
}

trader::api::TickerPrice24hr trader::api::BinanceAPI::fapi24hrTickerPriceChangeStatistics(const std::string &symbol)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol }
    };

    auto doc = fapiRequest(F_24HR_TICKER_PRICE_1, cpr);

    trader::api::TickerPrice24hr tp {
        .symbol             = JTO_STRING(doc, "symbol"),
        .priceChange        = std::stod(JTO_STRING(doc, "priceChange")),
        .priceChangePercent = std::stod(JTO_STRING(doc, "priceChangePercent")),
        .weightedAvgPrice   = std::stod(JTO_STRING(doc, "weightedAvgPrice")),
        .lastPrice          = std::stod(JTO_STRING(doc, "lastPrice")),
        .lastQty            = std::stod(JTO_STRING(doc, "lastQty")),
        .openPrice          = std::stod(JTO_STRING(doc, "openPrice")),
        .highPrice          = std::stod(JTO_STRING(doc, "highPrice")),
        .lowPrice           = std::stod(JTO_STRING(doc, "lowPrice")),
        .volume             = std::stod(JTO_STRING(doc, "volume")),
        .quoteVolume        = std::stod(JTO_STRING(doc, "quoteVolume")),
        .openTime           = doc["openTime"].GetUint64(),
        .closeTime          = doc["closeTime"].GetUint64(),
        .firstId            = doc["firstId"].GetInt64(),
        .lastId             = doc["lastId"].GetInt64(),
        .count              = doc["count"].GetInt64()
    };

    return tp;
}

std::vector<trader::api::TickerPrice24hr> trader::api::BinanceAPI::fapi24hrTickerPriceChangeStatistics()
{
    std::vector<trader::api::TickerPrice24hr> vtp;

    auto doc = fapiRequest(F_24HR_TICKER_PRICE_40);

    for (const auto &ticker : doc.GetArray())
    {
        trader::api::TickerPrice24hr tp {
            .symbol             = JTO_STRING(ticker, "symbol"),
            .priceChange        = std::stod(JTO_STRING(ticker, "priceChange")),
            .priceChangePercent = std::stod(JTO_STRING(ticker, "priceChangePercent")),
            .weightedAvgPrice   = std::stod(JTO_STRING(ticker, "weightedAvgPrice")),
            .lastPrice          = std::stod(JTO_STRING(ticker, "lastPrice")),
            .lastQty            = std::stod(JTO_STRING(ticker, "lastQty")),
            .openPrice          = std::stod(JTO_STRING(ticker, "openPrice")),
            .highPrice          = std::stod(JTO_STRING(ticker, "highPrice")),
            .lowPrice           = std::stod(JTO_STRING(ticker, "lowPrice")),
            .volume             = std::stod(JTO_STRING(ticker, "volume")),
            .quoteVolume        = std::stod(JTO_STRING(ticker, "quoteVolume")),
            .openTime           = ticker["openTime"].GetUint64(),
            .closeTime          = ticker["closeTime"].GetUint64(),
            .firstId            = ticker["firstId"].GetInt64(),
            .lastId             = ticker["lastId"].GetInt64(),
            .count              = ticker["count"].GetInt64()
        };
        vtp.push_back(tp);
    }

    return vtp;
}

trader::api::FuturesSymbolPriceTicker trader::api::BinanceAPI::fapiSymbolPriceTicker(const std::string &symbol)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol }
    };

    auto doc = fapiRequest(F_SYMBOL_PRICE_TICKER_1, cpr);

    trader::api::FuturesSymbolPriceTicker spt {
        .symbol = JTO_STRING(doc, "symbol"),
        .price  = std::stod(JTO_STRING(doc, "priceChange")),
        .time   = doc["closeTime"].GetUint64()
    };

    return spt;
}

std::vector<trader::api::FuturesSymbolPriceTicker> trader::api::BinanceAPI::fapiSymbolPriceTicker()
{
    std::vector<trader::api::FuturesSymbolPriceTicker> vspt;

    auto doc = fapiRequest(F_SYMBOL_PRICE_TICKER_2);

    for (const auto &ticker : doc.GetArray())
    {
        trader::api::FuturesSymbolPriceTicker spt {
            .symbol = JTO_STRING(ticker, "symbol"),
            .price  = std::stod(JTO_STRING(ticker, "priceChange")),
            .time   = ticker["closeTime"].GetUint64()
        };
        vspt.push_back(spt);
    }

    return vspt;
}

trader::api::FuturesOrderBookTicker trader::api::BinanceAPI::fapiSymbolOrderBookTicker(const std::string &symbol)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol }
    };

    auto doc = fapiRequest(F_SYMBOL_ORDER_BOOK_TICKER_1, cpr);

    trader::api::FuturesOrderBookTicker fobt {
        .symbol   = JTO_STRING(doc, "symbol"),
        .bidPrice = std::stod(JTO_STRING(doc, "bidPrice")),
        .bidQty   = std::stod(JTO_STRING(doc, "bidQty")),
        .askPrice = std::stod(JTO_STRING(doc, "askPrice")),
        .askQty   = std::stod(JTO_STRING(doc, "askQty")),
        .time     = doc["time"].GetUint64()
    };

    return fobt;
}

std::vector<trader::api::FuturesOrderBookTicker> trader::api::BinanceAPI::fapiSymbolOrderBookTicker()
{
    std::vector<trader::api::FuturesOrderBookTicker> vfobt;
    auto doc = fapiRequest(F_SYMBOL_ORDER_BOOK_TICKER_2);

    for (const auto &ticker : doc.GetArray())
    {
        trader::api::FuturesOrderBookTicker fobt {
            .symbol   = JTO_STRING(ticker, "symbol"),
            .bidPrice = std::stod(JTO_STRING(ticker, "bidPrice")),
            .bidQty   = std::stod(JTO_STRING(ticker, "bidQty")),
            .askPrice = std::stod(JTO_STRING(ticker, "askPrice")),
            .askQty   = std::stod(JTO_STRING(ticker, "askQty")),
            .time     = ticker["time"].GetUint64()
        };
        vfobt.push_back(fobt);
    }

    return vfobt;
}

trader::api::OpenInterest trader::api::BinanceAPI::fapiOpenInterest(const std::string &symbol)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol }
    };

    auto doc = fapiRequest(F_OPEN_INTEREST, cpr);

    OpenInterest oi {
        .openInterest = std::stod(JTO_STRING(doc, "openInterest")),
        .symbol       = JTO_STRING(doc, "symbol"),
        .time         = doc["time"].GetUint64()
    };

    return oi;
}

std::vector<trader::api::OpenInterestStatistics> trader::api::BinanceAPI::fapiOpenInterestStatistics(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit, const uint64_t &startTime, const uint64_t &endTime)
{
    assert(limit < 500);

    cpr::CurlHolder holder;
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "period", std::string(fromIntervalToString(period)) }
    };

    if (limit > 0)
        cpr.AddParameter({ "limit", fmt::to_string(limit) }, holder);
    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_OPEN_INTEREST_STATISTICS, cpr);

    std::vector<trader::api::OpenInterestStatistics> vois;
    for (const auto &data : doc.GetArray())
    {
        OpenInterestStatistics ois {
            .symbol               = JTO_STRING(data, "symbol"),
            .sumOpenInterest      = std::stod(JTO_STRING(data, "sumOpenInterest")),
            .sumOpenInterestValue = std::stod(JTO_STRING(data, "sumOpenInterestValue")),
            .timestamp            = data["timestamp"].GetUint64()
        };
        vois.push_back(ois);
    }
    return vois;
}

std::vector<trader::api::TopTraderLongShortRatioAccounts> trader::api::BinanceAPI::fapiTopTraderLongShortRatioAccounts(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit, const uint64_t &startTime, const uint64_t &endTime)
{
    assert(limit < 500);

    cpr::CurlHolder holder;
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "period", std::string(fromIntervalToString(period)) }
    };

    if (limit > 0)
        cpr.AddParameter({ "limit", fmt::to_string(limit) }, holder);
    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_TOP_TRADER_LONG_SHORT_ACCOUNT, cpr);

    std::vector<TopTraderLongShortRatioAccounts> ttlsra;
    for (const auto &data : doc.GetArray())
    {
        TopTraderLongShortRatio ttlsr {
            .symbol         = JTO_STRING(data, "symbol"),
            .longShortRatio = std::stod(JTO_STRING(data, "longShortRatio")),
            .longAccount    = std::stod(JTO_STRING(data, "longAccount")),
            .shortAccount   = std::stod(JTO_STRING(data, "shortAccount")),
            .timestamp      = data["timestamp"].GetUint64()
        };
        ttlsra.push_back(ttlsr);
    }

    return ttlsra;
}

std::vector<trader::api::TopTraderLongShortRatioPositions> trader::api::BinanceAPI::fapiTopTraderLongShortRatioPositions(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit, const uint64_t &startTime, const uint64_t &endTime)
{
    assert(limit < 500);

    cpr::CurlHolder holder;
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "period", std::string(fromIntervalToString(period)) }
    };

    if (limit > 0)
        cpr.AddParameter({ "limit", fmt::to_string(limit) }, holder);
    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_TOP_TRADER_LONG_SHORT_POSITION, cpr);

    std::vector<trader::api::TopTraderLongShortRatioPositions> ttlsrp;

    for (const auto &data : doc.GetArray())
    {
        TopTraderLongShortRatio ttlsr {
            .symbol         = JTO_STRING(data, "symbol"),
            .longShortRatio = std::stod(JTO_STRING(data, "longShortRatio")),
            .longAccount    = std::stod(JTO_STRING(data, "longAccount")),
            .shortAccount   = std::stod(JTO_STRING(data, "shortAccount")),
            .timestamp      = data["timestamp"].GetUint64()
        };
        ttlsrp.push_back(ttlsr);
    }

    return ttlsrp;
}

std::vector<trader::api::TopTraderLongShortRatio> trader::api::BinanceAPI::fapiLongShortRatio(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit, const uint64_t &startTime, const uint64_t &endTime)
{
    assert(limit < 500);

    cpr::CurlHolder holder;
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "period", std::string(fromIntervalToString(period)) }
    };

    if (limit > 0)
        cpr.AddParameter({ "limit", fmt::to_string(limit) }, holder);
    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_LONG_SHORT_RATIO, cpr);

    std::vector<trader::api::TopTraderLongShortRatio> vttlsr;

    for (const auto &data : doc.GetArray())
    {
        TopTraderLongShortRatio ttlsr {
            .symbol         = JTO_STRING(data, "symbol"),
            .longShortRatio = std::stod(JTO_STRING(data, "longShortRatio")),
            .longAccount    = std::stod(JTO_STRING(data, "longAccount")),
            .shortAccount   = std::stod(JTO_STRING(data, "shortAccount")),
            .timestamp      = data["timestamp"].GetUint64()
        };
        vttlsr.push_back(ttlsr);
    }

    return vttlsr;
}

std::vector<trader::api::FuturesTakerBuySellVolume> trader::api::BinanceAPI::fapiTakerBuySellVolume(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit, const uint64_t &startTime, const uint64_t &endTime)
{
    assert(limit < 500);

    cpr::CurlHolder holder;
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "period", std::string(fromIntervalToString(period)) }
    };

    if (limit > 0)
        cpr.AddParameter({ "limit", fmt::to_string(limit) }, holder);
    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_TAKER_BUY_SELL_VOLUME, cpr);

    std::vector<trader::api::FuturesTakerBuySellVolume> vftbsv;

    for (const auto &data : doc.GetArray())
    {
        FuturesTakerBuySellVolume ftbsv {
            .buySellRatio = std::stod(JTO_STRING(data, "buySellRatio")),
            .buyVol       = std::stod(JTO_STRING(data, "buyVol")),
            .sellVol      = std::stod(JTO_STRING(data, "sellVol")),
            .timestamp    = data["timestamp"].GetUint64()
        };
        vftbsv.push_back(ftbsv);
    }

    return vftbsv;
}

std::vector<trader::api::Candlestick> trader::api::BinanceAPI::fapiHistoricalBLVTNAVCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit, const uint64_t startTime, const uint64_t endTime)
{

    assert(limit < 1000);

    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", fmt::to_string(limit) },
        { "interval", std::string(fromIntervalToString(interval)) }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);

    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_HISTORICAL_BLVT, cpr);

    std::vector<trader::api::Candlestick> cdl;
    for (const auto &candleData : doc.GetArray())
    {
        std::size_t i = 0;
        Candlestick csd {};
        for (const auto &candle : candleData.GetArray())
        {
            switch (i)
            {
                case 0:
                    csd.openTime = candle.GetUint64();
                    break;
                case 1:
                    csd.open = std::stod(candle.GetString());
                    break;
                case 2:
                    csd.high = std::stod(candle.GetString());
                    break;
                case 3:
                    csd.low = std::stod(candle.GetString());
                    break;
                case 4:
                    csd.close = std::stod(candle.GetString());
                    break;
                case 6:
                    csd.closeTime = candle.GetUint64();
                    break;
                case 8:
                    csd.numberOfTrades = candle.GetUint64();
                    break;
                case 5:
                    [[fallthrough]];
                case 7:
                    [[fallthrough]];
                case 9:
                    [[fallthrough]];
                case 10:
                    [[fallthrough]];
                default:
                    break;
            }
            ++i;
        }
        cdl.push_back(csd);
    }

    return cdl;
}

trader::api::CompositeIndexSymbolInformation trader::api::BinanceAPI::fapiCompositeIndexSymbolInformation(const std::string &symbol)
{
    trader::api::CompositeIndexSymbolInformation cis;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol }
    };

    auto doc      = fapiRequest(F_COMPOSITE_INDEX_SYMBOL, cpr);

    cis.symbol    = JTO_STRING(doc, "symbol");
    cis.time      = doc["time"].GetUint64();
    cis.component = JTO_STRING(doc, "component");

    for (const auto &list : doc["baseAssetList"].GetArray())
    {
        CompositeBaseAsset cba {
            .baseAsset          = JTO_STRING(list, "baseAsset"),
            .quoteAsset         = JTO_STRING(list, "quoteAsset"),
            .weightInQuantity   = std::stod(JTO_STRING(list, "weightInQuantity")),
            .weightInPercentage = std::stod(JTO_STRING(list, "weightInPercentage"))
        };

        cis.baseAssetList.push_back(cba);
    }

    return cis;
}

std::pair<int, std::string> trader::api::BinanceAPI::fapiChangePositionMode(const bool &dualSidePosition, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "dualSidePosition", fmt::to_string(dualSidePosition) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_CHANGE_POSITION_MODE, cpr, true);

    return { doc["code"].GetInt(), JTO_STRING(doc, "msg") };
}

bool trader::api::BinanceAPI::fapiGetCurrentPositionMode(const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_GET_POSITION_MODE, cpr, true);

    return doc["dualSidePosition"].GetBool();
}

std::pair<int, std::string> trader::api::BinanceAPI::fapiChangeMultiAssetsMode(const bool &multiAssetsMargin, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "multiAssetsMargin", fmt::to_string(multiAssetsMargin) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_CHANGE_MULTI_ASSET_MODE, cpr, true);

    return { doc["code"].GetInt(), JTO_STRING(doc, "msg") };
}

bool trader::api::BinanceAPI::fapiGetMultiAssetsMode(const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_GET_MULTI_ASSET_MODE, cpr, true);

    return doc["multiAssetsMargin"].GetBool();
}

std::vector<trader::api::FuturesAccountBalance> trader::api::BinanceAPI::fapiGetAccountBalance(const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_ACCOUNT_BALANCE, cpr, true);

    std::vector<FuturesAccountBalance> vfab;

    for (const auto &bal : doc.GetArray())
    {
        FuturesAccountBalance fab {
            .accountAlias         = JTO_STRING(bal, "accountAlias"),
            .assetName            = JTO_STRING(bal, "asset"),
            .walletBalance        = std::stod(JTO_STRING(bal, "balance")),
            .crossedWalletBalance = std::stod(JTO_STRING(bal, "crossWalletBalance")),
            .crossUnPnL           = std::stod(JTO_STRING(bal, "crossUnPnl")),
            .availableBalance     = std::stod(JTO_STRING(bal, "availableBalance")),
            .maxWidrawAmount      = std::stod(JTO_STRING(bal, "maxWithdrawAmount")),
            .marginAvailable      = bal["marginAvailable"].GetBool(),
            .updateTime           = bal["updateTime"].GetUint64()
        };

        vfab.push_back(fab);
    }

    return vfab;
}

trader::api::FuturesAccountInformation trader::api::BinanceAPI::fapiGetAccountInformation(const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_ACCOUNT_INFORMATION, cpr, true);

    FuturesAccountInformation fai;

    fai.feeTier                     = doc["feeTier"].GetInt64();
    fai.canTrade                    = doc["canTrade"].GetBool();
    fai.canDeposit                  = doc["canDeposit"].GetBool();
    fai.canWithdraw                 = doc["canWithdraw"].GetBool();
    fai.updateTime                  = doc["updateTime"].GetUint64();
    fai.totalInitialMargin          = std::stod(JTO_STRING(doc, "totalInitialMargin"));
    fai.totalMaintMargin            = std::stod(JTO_STRING(doc, "totalMaintMargin"));
    fai.totalWalletBalance          = std::stod(JTO_STRING(doc, "totalWalletBalance"));
    fai.totalUnrealizedProfit       = std::stod(JTO_STRING(doc, "totalUnrealizedProfit"));
    fai.totalMarginBalance          = std::stod(JTO_STRING(doc, "totalMarginBalance"));
    fai.totalPositionInitialMargin  = std::stod(JTO_STRING(doc, "totalPositionInitialMargin"));
    fai.totalOpenOrderInitialMargin = std::stod(JTO_STRING(doc, "totalOpenOrderInitialMargin"));
    fai.totalCrossWalletBalance     = std::stod(JTO_STRING(doc, "totalCrossWalletBalance"));
    fai.totalCrossUnPnl             = std::stod(JTO_STRING(doc, "totalCrossUnPnl"));
    fai.availableBalance            = std::stod(JTO_STRING(doc, "availableBalance"));
    fai.maxWithdrawAmount           = std::stod(JTO_STRING(doc, "maxWithdrawAmount"));

    for (const auto &assets : doc["assets"].GetArray())
    {
        FuturesAccountInformationAsset faia {
            .asset                  = JTO_STRING(assets, "asset"),
            .walletBalance          = std::stod(JTO_STRING(assets, "walletBalance")),
            .unrealizedProfit       = std::stod(JTO_STRING(assets, "unrealizedProfit")),
            .marginBalance          = std::stod(JTO_STRING(assets, "marginBalance")),
            .maintMargin            = std::stod(JTO_STRING(assets, "maintMargin")),
            .initialMargin          = std::stod(JTO_STRING(assets, "initialMargin")),
            .positionInitialMargin  = std::stod(JTO_STRING(assets, "positionInitialMargin")),
            .openOrderInitialMargin = std::stod(JTO_STRING(assets, "openOrderInitialMargin")),
            .crossWalletBalance     = std::stod(JTO_STRING(assets, "crossWalletBalance")),
            .crossUnPnl             = std::stod(JTO_STRING(assets, "crossUnPnl")),
            .availableBalance       = std::stod(JTO_STRING(assets, "availableBalance")),
            .maxWithdrawAmount      = std::stod(JTO_STRING(assets, "maxWithdrawAmount")),
            .marginAvailable        = assets["marginAvailable"].GetBool(),
            .updateTime             = assets["updateTime"].GetUint64()
        };

        fai.assets[faia.asset] = faia;
    }

    for (const auto &positions : doc["positions"].GetArray())
    {
        FuturesAccountInformationPosition faip {
            .symbol                 = JTO_STRING(positions, "symbol"),
            .initialMargin          = std::stod(JTO_STRING(positions, "initialMargin")),
            .maintMargin            = std::stod(JTO_STRING(positions, "maintMargin")),
            .unrealizedProfit       = std::stod(JTO_STRING(positions, "unrealizedProfit")),
            .positionInitialMargin  = std::stod(JTO_STRING(positions, "positionInitialMargin")),
            .openOrderInitialMargin = std::stod(JTO_STRING(positions, "openOrderInitialMargin")),
            .leverage               = std::stod(JTO_STRING(positions, "leverage")),
            .isolated               = positions["isolated"].GetBool(),
            .entryPrice             = std::stod(JTO_STRING(positions, "entryPrice")),
            .maxNotional            = std::stod(JTO_STRING(positions, "maxNotional")),
            .positionSide           = JTO_STRING(positions, "positionSide"),
            .poisitionAmount        = std::stod(JTO_STRING(positions, "positionAmt")),
            .notional               = std::stod(JTO_STRING(positions, "notional")),
            .isolatedWallet         = std::stod(JTO_STRING(positions, "isolatedWallet")),
            .updateTime             = positions["updateTime"].GetUint64()
        };
        fai.positions[faip.symbol] = faip;
    }

    return fai;
}

trader::api::InitialLeverage trader::api::BinanceAPI::fapiChangeInitialLeverage(const std::string &symbol, const int64_t &leverage, const uint64_t &recvWindow)
{
    assert(leverage >= 1 && leverage <= 125);

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "leverage", fmt::to_string(leverage) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_CHANGE_INITIAL_LEVERAGE, cpr, true);

    trader::api::InitialLeverage il {
        .symbol           = JTO_STRING(doc, "symbol"),
        .maxNotionalValue = std::stod(JTO_STRING(doc, "maxNotionalValue")),
        .leverage         = doc["leverage"].GetInt64()
    };
    return il;
}

std::pair<int, std::string> trader::api::BinanceAPI::fapiChangeMarginTypeIsolated(const std::string &symbol, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "marginType", "ISOLATED" },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_CHANGE_MARGIN_TYPE, cpr, true);

    return { doc["code"].GetInt(), JTO_STRING(doc, "msg") };
}

std::pair<int, std::string> trader::api::BinanceAPI::fapiChangeMarginTypeCrossed(const std::string &symbol, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "marginType", "CROSSED" },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_CHANGE_MARGIN_TYPE, cpr, true);

    return { doc["code"].GetInt(), JTO_STRING(doc, "msg") };
}

std::tuple<double, int, std::string, trader::api::PositionMargin> trader::api::BinanceAPI::fapiModifyIsolatedPositionMarginOneWayMode(const std::string &symbol, const quantity_t &amount, const PositionMargin &pos, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "positionSide", "BOTH" },
        { "amount", fmt::to_string(amount) },
        { "type", fmt::to_string(static_cast<int>(pos)) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_MODIFY_ISOLATED_POSITION, cpr, true);

    return {
        doc["amount"].GetDouble(),
        doc["code"].GetInt64(),
        JTO_STRING(doc, "msg"),
        static_cast<PositionMargin>(doc["type"].GetInt())
    };
}

std::tuple<double, int, std::string, trader::api::PositionMargin> trader::api::BinanceAPI::fapiModifyIsolatedPositionMarginHedgeModeLong(const std::string &symbol, const quantity_t &amount, const PositionMargin &pos, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "positionSide", "LONG" },
        { "amount", fmt::to_string(amount) },
        { "type", fmt::to_string(static_cast<int>(pos)) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_MODIFY_ISOLATED_POSITION, cpr, true);

    return {
        doc["amount"].GetDouble(),
        doc["code"].GetInt64(),
        JTO_STRING(doc, "msg"),
        static_cast<PositionMargin>(doc["type"].GetInt())
    };
}

std::tuple<double, int, std::string, trader::api::PositionMargin> trader::api::BinanceAPI::fapiModifyIsolatedPositionMarginHedgeModeShort(const std::string &symbol, const quantity_t &amount, const PositionMargin &pos, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "positionSide", "SHORT" },
        { "amount", fmt::to_string(amount) },
        { "type", fmt::to_string(static_cast<int>(pos)) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_MODIFY_ISOLATED_POSITION, cpr, true);

    return {
        doc["amount"].GetDouble(),
        doc["code"].GetInt64(),
        JTO_STRING(doc, "msg"),
        static_cast<PositionMargin>(doc["type"].GetInt())
    };
}

std::vector<trader::api::PositionMarginChange> trader::api::BinanceAPI::fapiGetPositionMarginChangeHistory(const std::string &symbol, const PositionMargin &pos, const uint64_t &limit, const uint64_t &startTime, const uint64_t endTime, const uint64_t &recvWindow)
{
    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "type", fmt::to_string(static_cast<int>(pos)) },
        { "limit", fmt::to_string(limit) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_GET_POSITION_MARGIN_CHANGE_HISTORY, cpr, true);

    std::vector<trader::api::PositionMarginChange> vpsmc;
    for (const auto &hist : doc.GetArray())
    {
        PositionMarginChange pms {
            .amount       = std::stod(JTO_STRING(hist, "amount")),
            .asset        = JTO_STRING(hist, "asset"),
            .symbol       = JTO_STRING(hist, "symbol"),
            .time         = hist["time"].GetUint64(),
            .type         = static_cast<PositionMargin>(hist["type"].GetInt()),
            .positionSide = JTO_STRING(hist, "positionSide")
        };
        vpsmc.push_back(pms);
    }
    return vpsmc;
}

std::vector<trader::api::PositionInformation> trader::api::BinanceAPI::fapiGetPositionInformation(const std::string &symbol, const uint64_t &recvWindow)
{
    cpr::CurlHolder holder;
    cpr::Parameters cpr = cpr::Parameters {
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    if (!symbol.empty())
        cpr.AddParameter({ "symbol", symbol }, holder);

    auto doc = fapiRequest(F_POSITION_INFORMATION, cpr, true);

    std::vector<trader::api::PositionInformation> vpi;
    for (const auto &pos : doc.GetArray())
    {
        PositionInformation pi {
            .entryPrice       = std::stod(JTO_STRING(pos, "entryPrice")),
            .marginType       = JTO_STRING(pos, "marginType") == "isolated" ? MarginType::isolated : MarginType::crossed,
            .isAutoAddMargin  = JTO_STRING(pos, "isAutoAddMargin") == "false" ? false : true,
            .isolatedMargin   = std::stod(JTO_STRING(pos, "isolatedMargin")),
            .leverage         = std::stod(JTO_STRING(pos, "leverage")),
            .liquidationPrice = std::stod(JTO_STRING(pos, "liquidationPrice")),
            .markPrice        = std::stod(JTO_STRING(pos, "markPrice")),
            .maxNotionalVal   = std::stod(JTO_STRING(pos, "maxNotionalValue")),
            .positionAmt      = std::stod(JTO_STRING(pos, "positionAmt")),
            .symbol           = JTO_STRING(pos, "symbol"),
            .unrealizedProfit = std::stod(JTO_STRING(pos, "unRealizedProfit")),
            .positionSide     = [&pos]() {
                auto side = JTO_STRING(pos, "positionSide");
                if (side == "BOTH")
                    return PositionSide::BOTH;
                else if (side == "LONG")
                    return PositionSide::LONG;
                else
                    return PositionSide::SHORT;
            }(),
            .updateTime = pos["updateTime"].GetUint64()
        };

        vpi.push_back(pi);
    }

    return vpi;
}

std::vector<trader::api::AccountTradeList> trader::api::BinanceAPI::fapiGetAccountTradeList(const std::string &symbol, const uint64_t &limit, const uint64_t &startTime, const uint64_t endTime, const uint64_t &recvWindow)
{
    cpr::CurlHolder holder;

    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "limit", fmt::to_string(limit) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    if (startTime > 0)
        cpr.AddParameter({ "startTime", fmt::to_string(startTime) }, holder);
    if (endTime > 0)
        cpr.AddParameter({ "endTime", fmt::to_string(endTime) }, holder);

    auto doc = fapiRequest(F_ACCOUNT_TRADE_LIST, cpr, true);

    std::vector<trader::api::AccountTradeList> vatl;
    for (const auto &list : doc.GetArray())
    {
        AccountTradeList atl {
            .buyer           = list["buyer"].GetBool(),
            .commission      = std::stod(JTO_STRING(list, "commission")),
            .commissionAsset = JTO_STRING(list, "commissionAsset"),
            .id              = list["id"].GetInt64(),
            .maker           = list["maker"].GetBool(),
            .orderId         = list["orderId"].GetInt64(),
            .price           = std::stod(JTO_STRING(list, "price")),
            .qty             = std::stod(JTO_STRING(list, "qty")),
            .quoteQty        = std::stod(JTO_STRING(list, "quoteQty")),
            .realizePnL      = std::stod(JTO_STRING(list, "realizedPnl")),
            .side            = JTO_STRING(list, "side") == "SELL" ? OrderSide::sell : OrderSide::buy,
            .posSide         = [&list]() {
                auto side = JTO_STRING(list, "positionSide");
                if (list == "BOTH")
                    return PositionSide::BOTH;
                else if (list == "LONG")
                    return PositionSide::LONG;
                else
                    return PositionSide::SHORT;
            }(),
            .symbol     = JTO_STRING(list, "symbol"),
            .updateTime = list["time"].GetUint64()
        };

        vatl.push_back(atl);
    }
    return vatl;
}

std::vector<trader::api::AccountTradeList> trader::api::BinanceAPI::fapiGetAccountTradeList(const std::string &symbol, const uint64_t &fromId, const uint64_t &limit, const uint64_t &recvWindow)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "fromId", fmt::to_string(fromId) },
        { "limit", fmt::to_string(limit) },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) }
    };

    auto doc = fapiRequest(F_ACCOUNT_TRADE_LIST, cpr, true);

    std::vector<trader::api::AccountTradeList> vatl;
    for (const auto &list : doc.GetArray())
    {
        AccountTradeList atl {
            .buyer           = list["buyer"].GetBool(),
            .commission      = std::stod(JTO_STRING(list, "commission")),
            .commissionAsset = JTO_STRING(list, "commissionAsset"),
            .id              = list["id"].GetInt64(),
            .maker           = list["maker"].GetBool(),
            .orderId         = list["orderId"].GetInt64(),
            .price           = std::stod(JTO_STRING(list, "price")),
            .qty             = std::stod(JTO_STRING(list, "qty")),
            .quoteQty        = std::stod(JTO_STRING(list, "quoteQty")),
            .realizePnL      = std::stod(JTO_STRING(list, "realizedPnl")),
            .side            = JTO_STRING(list, "side") == "SELL" ? OrderSide::sell : OrderSide::buy,
            .posSide         = [&list]() {
                auto side = JTO_STRING(list, "positionSide");
                if (list == "BOTH")
                    return PositionSide::BOTH;
                else if (list == "LONG")
                    return PositionSide::LONG;
                else
                    return PositionSide::SHORT;
            }(),
            .symbol     = JTO_STRING(list, "symbol"),
            .updateTime = list["time"].GetUint64()
        };
    }

    return vatl;
}

std::string trader::api::BinanceAPI::fapiStartUserDataStream()
{
    auto doc = fapiRequest(F_START_USER_STREAM, true);
    return JTO_STRING(doc, "listenKey");
}

void trader::api::BinanceAPI::fapiKeepAliveUserDataStream()
{
    auto doc = fapiRequest(F_KEEP_ALIVE_USER_STREAM, true);
}

void trader::api::BinanceAPI::fapiCloseUserDataStream()
{
    auto doc = fapiRequest(F_CLOSE_USER_STREAM, true);
}

trader::api::OrderAcknowledge trader::api::BinanceAPI::fapiNewOrderOneWayLimit(const std::string &symbol,
    const uint64_t &recvWindow,
    const std::string &userId,
    const OrderSide &side,
    const OrderTimeInForce &otif,
    const quantity_t &qty,
    const currency_t &price,
    const trader::api::FuturesExchangeInformationSymbol &symInfo,
    const bool &reduceOnly)
{

    using namespace std::string_literals;
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) },
        { "newClientOrderId", userId },
        { "reduceOnly", reduceOnly ? "true"s : "false"s },
        { "type", "LIMIT"s },
        { "side", [&side]() -> std::string {
             switch (side)
             {
                 case OrderSide::sell:
                     return "SELL"s;
                 case OrderSide::buy:
                     return "BUY"s;
             }
         }() },
        { "timeInForce", [&otif]() -> std::string {
             switch (otif)
             {
                 case OrderTimeInForce::GTC:
                     return "GTC"s;
                 case OrderTimeInForce::IOC:
                     return "IOC"s;
                 case OrderTimeInForce::FOK:
                     return "FOK"s;
                 case OrderTimeInForce::GTX:
                     return "GTX"s;
             }
         }() },
        { "quantity", [&qty, &symInfo]() -> std::string {
             return fmt::format("{:.{}f}", qty, symInfo.quantityPrecision);
         }() },
        { "price", [&price, &symInfo]() -> std::string {
             return fmt::format("{:.{}f}", price, symInfo.pricePrecision);
         }() },
    };

    auto doc = fapiRequest(F_NEW_LIMIT_ORDER, cpr, true);

    OrderAcknowledge oa {
        .symbol        = JTO_STRING(doc, "symbol"),
        .clientOrderId = JTO_STRING(doc, "clientOrderId"),
        .reduceOnly    = doc["reduceOnly"].GetBool(),
        .closePosition = doc["closePosition"].GetBool(),
        .side          = [&doc]() -> OrderSide {
            const std::string side = JTO_STRING(doc, "side");
            if (side == "BUY")
                return OrderSide::buy;
            else
                return OrderSide::sell;
        }(),
        .position = [&doc]() -> PositionSide {
            const std::string side = JTO_STRING(doc, "side");
            if (side == "SHORT")
                return PositionSide::SHORT;
            else if (side == "LONG")
                return PositionSide::LONG;
            else
                return PositionSide::BOTH;
        }(),
        .orderId = doc["orderId"].GetUint64()
    };

    return oa;
}
/*
trader::api::OrderAcknowledge trader::api::BinanceAPI::fapiNewOrderOneWayLimitClosePosition(const std::string &symbol,
    const uint64_t &recvWindow,
    const std::string &userId,
    const OrderSide &side,
    const OrderTimeInForce &otif,
    const currency_t &price,
    const trader::api::FuturesExchangeInformationSymbol &symInfo,
    const WorkingType &wt)
{
    auto doc = fapiRequest(F_NEW_LIMIT_ORDER, true);

    OrderAcknowledge oa {
        .symbol        = JTO_STRING(doc, "symbol"),
        .clientOrderId = JTO_STRING(doc, "clientOrderId"),
        .reduceOnly    = doc["reduceOnly"].GetBool(),
        .closePosition = doc["closePosition"].GetBool(),
        .side          = [&doc]() -> OrderSide {
            const std::string side = JTO_STRING(doc, "side");
            if (side == "BUY")
                return OrderSide::buy;
            else
                return OrderSide::sell;
        }(),
        .position = [&doc]() -> PositionSide {
            const std::string side = JTO_STRING(doc, "side");
            if (side == "SHORT")
                return PositionSide::SHORT;
            else if (side == "LONG")
                return PositionSide::LONG;
            else
                return PositionSide::BOTH;
        }(),
        .orderId = doc["orderId"].GetUint64()
    };

    return oa;
}*/

trader::api::OrderAcknowledge trader::api::BinanceAPI::fapiCancelOrder(const std::string &symbol, const uint64_t &recvWindow, const std::string &userOrderId)
{
    cpr::Parameters cpr = cpr::Parameters {
        { "symbol", symbol },
        { "recvWindow", fmt::to_string(recvWindow) },
        { "timestamp", fmt::to_string(getTime()) },
        { "origClientOrderId", userOrderId }
    };

    auto doc = fapiRequest(F_CANCEL_ORDER, cpr, true);

    OrderAcknowledge oa {
        .symbol        = JTO_STRING(doc, "symbol"),
        .clientOrderId = JTO_STRING(doc, "clientOrderId"),
        .reduceOnly    = doc["reduceOnly"].GetBool(),
        .closePosition = doc["closePosition"].GetBool(),
        .side          = [&doc]() -> OrderSide {
            const std::string side = JTO_STRING(doc, "side");
            if (side == "BUY")
                return OrderSide::buy;
            else
                return OrderSide::sell;
        }(),
        .position = [&doc]() -> PositionSide {
            const std::string side = JTO_STRING(doc, "side");
            if (side == "SHORT")
                return PositionSide::SHORT;
            else if (side == "LONG")
                return PositionSide::LONG;
            else
                return PositionSide::BOTH;
        }(),
        .orderId = doc["orderId"].GetUint64()
    };

    return oa;
}
