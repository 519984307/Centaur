////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// binapi.hpp

/**
 * @file binapi.hpp
 * @brief Binance limits
 *
 * @author Ricardo Romero
 * @date 2021-07-07
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#ifdef __cplusplus
#pragma once
#endif /* __cplusplus*/

#ifndef BINAPI_HPP_
#define BINAPI_HPP_

#include "BinanceAPIDefs.hpp"
#include "BinanceAPIGlobal.hpp"
#include "BinanceAPILimits.hpp"

#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

/// Default value for parameter recvWindow of the Binance API
#define BINANCEAPI_DEFAULT_RECVWINDOW 4000 /// Change according to personal needs

namespace trader::api
{
    namespace local
    {
        struct BinanceAPIRequest;
    }

    /// \brief Binance user keys
    struct BINAPI_EXPORT BinanceKeys
    {
        std::string apiKey;
        std::string secretKey;
    };

    class BINAPI_EXPORT BinanceAPI
    {
    public:
        BinanceAPI(BinanceKeys *keys, BinanceLimits *exl);

        virtual ~BinanceAPI();

    private:
        /// \brief Compute HMAC 256
        ///
        /// \param key API Key
        /// \param plainText Plain text to cipher
        /// \return std::string Ciphered text
        static std::string HMAC256(const std::string &key, const std::string &plainText) noexcept;

        /// \brief Make an API rest request
        ///
        /// \param request  Request parameters
        /// \param parameters Parameters of the call. There is no guarantee this remains intact
        /// \param secure Will call ExchangeAPI::secure to ensure a secure request
        /// \param timeLog On success a JSON Document otherwise will throw
        /// \return rapidjson::Document ExchangeAPIError on error
        rapidjson::Document request(local::BinanceAPIRequest *request, cpr::Parameters &parameters, const bool &secure = false, double *const timeLog = nullptr);

    public:
        /// \brief Converts BinanceTimeIntervals to a string that the BinanceAPI understands
        ///
        /// \param interval Time interval
        /// \return constexpr std::string_view The string. e.g. fromIntervalToString(BinanceTimeIntervals::i12h) returns std::string_view{"12h"}
        static constexpr std::string_view fromIntervalToString(const BinanceTimeIntervals &interval) noexcept
        {
            switch (interval)
            {
                case trader::api::BinanceTimeIntervals::i1m:
                    return "1m";
                case trader::api::BinanceTimeIntervals::i3m:
                    return "3m";
                case trader::api::BinanceTimeIntervals::i5m:
                    return "5m";
                case trader::api::BinanceTimeIntervals::i15m:
                    return "15m";
                case trader::api::BinanceTimeIntervals::i30m:
                    return "30m";
                case trader::api::BinanceTimeIntervals::i1h:
                    return "1h";
                case trader::api::BinanceTimeIntervals::i2h:
                    return "2h";
                case trader::api::BinanceTimeIntervals::i4h:
                    return "4h";
                case trader::api::BinanceTimeIntervals::i6h:
                    return "6h";
                case trader::api::BinanceTimeIntervals::i8h:
                    return "8h";
                case trader::api::BinanceTimeIntervals::i12h:
                    return "12h";
                case trader::api::BinanceTimeIntervals::i1d:
                    return "1d";
                case trader::api::BinanceTimeIntervals::i3d:
                    return "3d";
                case trader::api::BinanceTimeIntervals::i1w:
                    return "1w";
                case trader::api::BinanceTimeIntervals::i1M:
                    return "1M";
            }
        }

        /// \brief Returns a conversion of BinanceTimeIntervals to milliseconds
        /// \note This function is intended to be use in conjuction with trader::api::BinanceAPI::getCandlesTimesAndLimits
        ///
        /// \param interval Time interval
        /// \return constexpr uint64_t The milliseconds of the interval. e.g. fromIntervalToMilliseconds(BinanceTimeIntervals::i6h) returns uint64_t{60'000ull * 60 * 6}
        static inline constexpr uint64_t fromIntervalToMilliseconds(const BinanceTimeIntervals &interval) noexcept
        {
            switch (interval)
            {
                case trader::api::BinanceTimeIntervals::i1m:
                    return 60'000ull; // 60,000 milliseconds in a minute
                case trader::api::BinanceTimeIntervals::i3m:
                    return 60'000ull * 3; // * minutes
                case trader::api::BinanceTimeIntervals::i5m:
                    return 60'000ull * 5;
                case trader::api::BinanceTimeIntervals::i15m:
                    return 60'000ull * 15;
                case trader::api::BinanceTimeIntervals::i30m:
                    return 60'000ull * 30;
                case trader::api::BinanceTimeIntervals::i1h:
                    return 60'000ull * 60; // * 60 minutes
                case trader::api::BinanceTimeIntervals::i2h:
                    return 60'000ull * 60 * 2; // * 60 minutes * 2 hours
                case trader::api::BinanceTimeIntervals::i4h:
                    return 60'000ull * 60 * 4;
                case trader::api::BinanceTimeIntervals::i6h:
                    return 60'000ull * 60 * 6;
                case trader::api::BinanceTimeIntervals::i8h:
                    return 60'000ull * 60 * 8;
                case trader::api::BinanceTimeIntervals::i12h:
                    return 60'000ull * 60 * 12;
                case trader::api::BinanceTimeIntervals::i1d:
                    return 60'000ull * 60 * 24;
                case trader::api::BinanceTimeIntervals::i3d:
                    return 60'000ull * 60 * 24 * 3; // *60 minutes * 24 hours (24 hours in a day) * 3 days
                case trader::api::BinanceTimeIntervals::i1w:
                    return 60'000ull * 60 * 24 * 7;
                case trader::api::BinanceTimeIntervals::i1M:
                    return 60'000ull * 60 * 24 * 30; // Lets default a month as a 30 day period
            }
        }

    protected:
        void secureRequest(cpr::Session &session, cpr::Parameters &parameters, const bool &preventSignature) noexcept;

        /// \brief Handle an API Error request
        ///
        /// \param data The user defined by the user
        /// \param value This will set to the APIError (apiCode) in the exception
        /// \param code This will set to the APIError (apiMessage) in the exception
        /// \param message nor code not message are initialized
        /// \return true No error
        /// \return false Error
        bool handleAPIRequestError(const bool &data, const rapidjson::Value &value, int &code, std::string &message);

        /// \brief When an HTTP call returns a code different of 200 and the response body is a JSON object this function needs to handle
        ///
        /// \param jsonObject The JSON object string
        /// \param code This will set to the httpAPIRequestError2 (apiCode) in the exception
        /// \param message This will set to the httpAPIRequestError2 (apiMessage) in the exception
        void handleHTTPErrorResponse(const std::string &jsonObject, int &code, std::string &message);

    protected:
        /// \brief Wrapper for a request with no parameters
        ///
        /// \param apiCall Call
        /// \param secure Secure or not
        /// \return rapidjson::Document The JSON Document
        rapidjson::Document apiRequest(const size_t &apiCall, const bool &secure = false);

        /// \brief Wrapper for a request with parameters
        ///
        /// \param apiCall Call
        /// \param parameters Parameters to pass
        /// \param secure Secure or not
        /// \return rapidjson::Document The JSON Document
        rapidjson::Document apiRequest(const size_t &apiCall, cpr::Parameters &parameters, const bool &secure = false);

        /// \brief Wrapper for a futures request with no parameters
        ///
        /// \param apiCall Call
        /// \param secure Secure or not
        /// \return rapidjson::Document The JSON Document
        rapidjson::Document fapiRequest(const size_t &apiCall, const bool &secure = false);

        /// \brief Wrapper for a futures request with parameters
        ///
        /// \param apiCall Call
        /// \param parameters Parameters to pass
        /// \param secure Secure or not
        /// \return rapidjson::Document The JSON Document
        rapidjson::Document fapiRequest(const size_t &apiCall, cpr::Parameters &parameters, const bool &secure = false);

    public:
        /// \brief Get the last api call the time to retrieve the data from the internet
        ///
        /// \return double A double with the seconds
        /// \remarks This time does not include the time taken to parse the actual JSON data
        T_NODISCARD double getLastCallTime() const noexcept;

        /// \brief Retrieve UTC time
        ///
        /// \return uint64_t Time in milliseconds
        /// \note This uses internal clock. Does not make any API request so does not go through the internet
        T_NODISCARD static uint64_t getTime() noexcept;

    public:
        /// \brief Binance API for candlesticks support only a max of 1000 candles retrieve from the exchange
        /// This function returns a vector with the list of times from
        ///
        /// \param interval Supported exchange intervals
        /// \param startTime Start time
        /// \param endTime End time
        /// \param totalExpected The number of candles expected
        /// \return std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>
        ///         A vector with times. Each item has a tuple as an entrance. First value is the start time, then the end time, finally the number of elements
        ///         that can be retrieved from the exchange
        /// \remarks This function is intended to be use with getCandlestick
        /// \note startTime & endTime MUST BE IN MILLISECONDS OTHERWISE CALCULATIONS WILL BE TOTALLY WRONG
        /// \note tartTime and endTime will be adjusted from not "aligned" timestamps
        ///  EXAMPLE:
        ///  Supposed we want start from timestamp 1617304680000 in intervals of 30 minutes
        ///            However, 1617304680000 1 April 2021 19:18:00 and it is not "aligned" to a 30 minute interval
        ///            So the computations will be adjusted to a timestamp of 1617303600000 with corresponds to 1 April 2021 19:00:00
        ///    EXAMPLE Suppose we want to retrieved candles from 1 september 2019 00:00:00 (timestamp: 1567296000000 in ms) to
        ///            1 april 2021 19:00:00 (timestamp: in 1617303600000) in intervals of 1m of BTCUSDT
        ///
        ///            First we call getCandlesTimeAndLimits:
        ///
        ///         uint64_t expected;
        ///            auto timesAndLimits = getCandleTimeAndLimits(BinanceTimeIntervals::b1m, 1567296000000, 1617303600000, expected);
        ///
        ///            auto std::vector<Candlestick> candles;
        ///            candles.reserve(expected); // Avoid to many allocations
        ///
        ///            Then in a loop we unfold the next:
        ///            for( const intervals : timesAndLimits)
        ///            {
        ///                const auto &[start, end, limit] = intervals;
        ///
        ///                auto candles = getCandlestick("BTCUSDT"s, BinanceTimeIntervals::1bm, start, end, limit);
        ///                candles.push_back(candles); // Not necessary correct but just to get an idea
        ///            }
        ///
        T_NODISCARD static std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> getCandlesTimesAndLimits(const BinanceTimeIntervals &interval, const uint64_t &startTime, const uint64_t &endTime, uint64_t &totalExpected);

    public:
        /// \brief Get Ping status
        ///
        /// \remarks When getPing fails an exception is triggered otherwise "nothing" happens
        void apiPing();

        /// \brief Get Exchange Status
        ///
        /// \return true
        /// \return false
        T_NODISCARD bool apiExchangeStatus();

        /// \brief Get candlesticks
        ///
        /// \param symbol Symbol to be retrieved
        /// \param interval Time interval
        /// \param startTime Starting time
        /// \param endTime End time
        /// \param limit limit of candles 0 - 1000
        /// \return std::vector<Candlestick> A vector containing all candles
        T_NODISCARD std::vector<Candlestick> apiCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const std::size_t &startTime, const std::size_t &endTime, const std::size_t &limit);

        /// \brief Get the Exchange Information object
        ///
        /// \return ExchangeInformation Retrieves all the information
        T_NODISCARD ExchangeInformation apiExchangeInformation();

        /// \brief Get the account information
        ///
        /// \return std::string Return the exchange "data" string
        T_NODISCARD std::string apiAccountStatus();

        /// \brief Retrieve all account information
        ///
        /// \return AccountInformation  Account Information from the exchange
        T_NODISCARD AccountInformation apiAccountInformation();

        /// \brief Retrieve the orderbook for a symbol
        ///
        /// \param symbol Symbol name
        /// \return trader::api::Orderbook Orderbook data
        T_NODISCARD trader::api::Orderbook apiOrderbook(const std::string &symbol, const OrderBookLimits &limits);

        /// \brief Retrieve a user data stream listen key
        ///
        /// \return std::string The key
        T_NODISCARD std::string apiListenKey();

        /// \brief Keep a current listen key alive
        ///
        /// \param listenKey current listen key
        /// \note On fail, an exception must be throw
        void apiKeepListenKeyAlive(const std::string &listenKey);

        /// \brief Create a Limit order
        ///
        /// \param symbol Symbol
        /// \param timeWindow Time Frame
        /// \param tradeId Trade id
        /// \param side Order price
        /// \param price Order quantity
        /// \param quantity Buy/Sell
        /// \param tif Time in force
        /// \param testOnly  Use the order/test API
        /// \return ServerSideOrder The server response or if testOnly = true a simulated response
        T_NODISCARD ServerSideOrder apiCreateLimitOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &tradeId, const api::OrderSide &side, const trader::api::currency_t &price, const trader::api::currency_t &quantity, const api::OrderTimeInForce &tif, const ExchangeInformationSymbol &symInfo, const bool &testOnly);

        /**
         * Create a market order
         * @param symbol Symbol
         * @param timeWindow Time Frame
         * @param tradeId Trade id
         * @param side  Buy/sell
         * @param quantity Order quantity
         * @param testOnly Use the order/test API
         * @return The server response or if testOnly = true a simulated response
         */
        T_NODISCARD ServerSideOrder apiCreateMarketOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &tradeId, const api::OrderSide &side, const trader::api::currency_t &quantity, const ExchangeInformationSymbol &symInfo, const bool &testOnly);

        /// \brief Create a limit market order
        ///
        /// \param symbol Symbol
        /// \param timeWindow Time Frame
        /// \param tradeId Trade id
        /// \param price Order Price
        /// \param quantity Order quantity
        /// \param side Buy/Sell
        /// \param testOnly Use the order/test API
        /// \return ServerSideOrder The server response or if testOnly = true a simulated response
        T_NODISCARD ServerSideOrder apiCreateLimitMakerOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &tradeId, const trader::api::currency_t &price, const trader::api::currency_t &quantity, const api::OrderSide &side, const ExchangeInformationSymbol &symInfo, const bool &testOnly);

        /// \brief Query an order status
        ///
        /// \param symbol Symbol
        /// \param timeWindow Time frame
        /// \param orderId Order id
        /// \return QueryOrder Server response
        T_NODISCARD QueryOrder apiQueryOrder(const std::string &symbol, const std::size_t &timeWindow, const std::size_t &orderId);

        /// \brief Cancel an order
        ///
        /// \param symbol Symbol name
        /// \param timeWindow Time frame
        /// \param orderId The cancelation Order id
        /// \param cancelOrderId The order id to cancel
        /// \return CancelOrder
        T_NODISCARD CancelOrder apiCancelOrder(const std::string &symbol, const std::size_t &timeWindow, const std::string &orderId, const std::string &cancelOrderId);

        /// \brief Retrieve the last average price
        ///
        /// \param symbol Symbol name
        /// \return trader::api::currency_t the average price
        T_NODISCARD trader::api::currency_t apiAveragePrice(const std::string &symbol);

        // Futures
    public:
        /// \brief Test connectivity to the Rest API.
        ///
        /// \note On failure an exception is thrown otherwise the function continues the program normal execution
        void fapiPing();

        /// \brief Test connectivity to the Rest API and get the current server time.. NOTE: For parameter passing when time signature is required use getTime()
        ///
        /// \return uint64_t Server Time
        T_NODISCARD uint64_t fapiGetServerTime();

        /// \brief Current exchange trading rules and symbol information
        ///
        /// \return trader::api::FuturesExchangeInformation Exchange information data
        T_NODISCARD trader::api::FuturesExchangeInformation fapiGetExchangeInformation();

        /// \brief Get the orderbook
        ///
        /// \param symbol Symbol name
        /// \param limit Limit number: IMPORTANT: 5000 is not valid and will assert the function
        /// \return trader::api::Orderbook The order book data
        T_NODISCARD trader::api::FuturesOrderBook fapiGetOrderBook(const std::string &symbol, const OrderBookLimits &limit);

        /// \brief Get recent trades
        ///
        /// \param symbol Symbol name
        /// \param limit Limit number. Maximum is 1000. Limit higher than 1000 will be set to 1000 and limit <= 0 will set to 500
        /// \return  std::vector<trader::api::FuturesRecentTradesList> Information list
        T_NODISCARD std::vector<trader::api::FuturesRecentTradesList> fapiRecentTradesList(const std::string &symbol, const int64_t &limit = 500);

        /// \brief Get older market historical trades by limit. Requires authentication
        ///
        /// \param symbol Symbol name
        /// \param limit limit. Maximum is 1000. Limit higher than 1000 will be set to 1000 and limit <= 0 will set to 500
        /// \return std::vector<trader::api::FuturesRecentTradesList> Information list
        T_NODISCARD std::vector<trader::api::FuturesRecentTradesList> fapiOldTradesLookup(const std::string &symbol, const int &limit = 500);

        // \brief Get older market historical trades by id. Requires authentication
        ///
        /// \param symbol Symbol name
        /// \param id trade Id. Maximum is 1000. Limit higher than 1000 will be set to 1000 and limit <= 0 will set to 500
        /// \return std::vector<trader::api::FuturesRecentTradesList> Information list
        T_NODISCARD std::vector<trader::api::FuturesRecentTradesList> fapiOldTradesLookup(const std::string &symbol, const uint64_t &id);

        /// \brief Get compressed, aggregate trades. Trades that fill at the time, from the same order, with the same price will have the quantity aggregated
        ///
        /// \param symbol Symbol name
        /// \param limit limit. Max: 1000
        /// \param startTime Starting time. can be zero
        /// \param endTime End time. can be zero
        /// \param fromId Id. can be zero
        /// \return std::vector<FuturesCompressedAggregateTradeLists> List of all compressed/aggregate trades
        T_NODISCARD std::vector<trader::api::FuturesCompressedAggregateTradeLists> fapiCompressedAggregateTradesLists(const std::string &symbol, const int &limit = 500, const uint64_t &startTime = 0, const uint64_t &endTime = 0, const int64_t &fromId = 0);

        /// \brief Kline/candlestick bars for a symbol. Klines are uniquely identified by their open time.
        ///
        /// \param symbol Symbol name
        /// \param interval Candle interval
        /// \param limit Candles limit
        /// \param startTime Candles start time
        /// \param endTime Candles end time
        /// \return std::vector<trader::api::Candlestick>  Candle data
        T_NODISCARD std::vector<trader::api::Candlestick> fapiCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit = 500, const uint64_t startTime = 0, const uint64_t endTime = 0);

        /// \brief Kline/candlestick bars for a specific contract type.
        ///
        /// \param symbol Symbol name
        /// \param interval Candle interval
        /// \param limit Candles limit
        /// \param startTime Candles start time
        /// \param endTime Candles end time
        /// \return std::vector<trader::api::Candlestick>  Candle data
        T_NODISCARD std::vector<trader::api::Candlestick> fapiContinuesContractCandlestick(const std::string &symbol, const FuturesContractType &contractType, const BinanceTimeIntervals &interval, const int &limit = 500, const uint64_t startTime = 0, const uint64_t endTime = 0);

        /// \brief Kline/candlestick bars for the index price of a pair.
        /// volume, quoteAssetVolume, baseAssetVolume, takerBaseAssetVolume, takerQuoteAssetVolume are always zero
        ///
        /// \param symbol Symbol name
        /// \param interval Candle interval
        /// \param limit Candles limit
        /// \param startTime Candles start time
        /// \param endTime Candles end time
        /// \return std::vector<trader::api::Candlestick> Candle data
        T_NODISCARD std::vector<trader::api::Candlestick> fapiIndexPriceCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit = 500, const uint64_t startTime = 0, const uint64_t endTime = 0);

        /// \brief Kline/candlestick bars for the mark price of a symbol.
        /// volume, quoteAssetVolume, baseAssetVolume, takerBaseAssetVolume, takerQuoteAssetVolume are always zero
        ///
        /// \param symbol Symbol name
        /// \param interval Candle interval
        /// \param limit Candles limit
        /// \param startTime Candles start time
        /// \param endTime Candles end time
        /// \return std::vector<trader::api::Candlestick> Candle data
        T_NODISCARD std::vector<trader::api::Candlestick> fapiMarkPriceCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit = 500, const uint64_t startTime = 0, const uint64_t endTime = 0);

        /// \brief Mark Price and Funding Rate
        ///
        /// \param symbol Symbol name
        /// \return trader::api::MarkPrice Premium index (Mark Price)
        T_NODISCARD trader::api::MarkPrice fapiMarkPrice(const std::string &symbol);

        /// \brief Funding rate
        ///
        /// \param symbol Symbol name, optional
        /// \param limit Limit, optional. Max > 1000. More than 1000 the function will assert
        /// \param startTime Start time, optional
        /// \param endTime End time, optional
        /// \return std::vector<FundingRate> Return all data
        T_NODISCARD std::vector<trader::api::FundingRate> fapiGetFundingRateHistory(const std::string &symbol = "", const int &limit = 100, const uint64_t &startTime = 0, const uint64_t &endTime = 0);

        /// \brief 24 hour rolling window price change statistics.
        ///
        /// \param symbol Symbol name. optional
        /// \return trader::api::TickerPrice24hr all symbol data
        T_NODISCARD trader::api::TickerPrice24hr fapi24hrTickerPriceChangeStatistics(const std::string &symbol);

        /// \brief 24 hour rolling window price change statistics.
        ///
        /// \return std::vector<trader::api::TickerPrice24hr> symbol data
        T_NODISCARD std::vector<trader::api::TickerPrice24hr> fapi24hrTickerPriceChangeStatistics();

        /// \brief Latest price for a symbol or symbols
        ///
        /// \param symbol Symbol name
        /// \return trader::api::FuturesSymbolPriceTicker Symbol data
        T_NODISCARD trader::api::FuturesSymbolPriceTicker fapiSymbolPriceTicker(const std::string &symbol);

        /// \brief Latest price for a symbol or symbols
        ///
        /// \return std::vector<trader::api::FuturesSymbolPriceTicker> All symbols price ticker
        T_NODISCARD std::vector<trader::api::FuturesSymbolPriceTicker> fapiSymbolPriceTicker();

        /// \brief Best price/qty on the order book for a symbol or symbols.
        ///
        /// \param symbol Symbol name
        /// \return trader::api::FuturesOrderBookTicker Symbol data
        T_NODISCARD trader::api::FuturesOrderBookTicker fapiSymbolOrderBookTicker(const std::string &symbol);

        /// \brief Best price/qty on the order book for a symbol or symbols.
        ///
        /// \return std::vector<trader::api::FuturesOrderBookTicker> All symbol data
        T_NODISCARD std::vector<trader::api::FuturesOrderBookTicker> fapiSymbolOrderBookTicker();

        /// \brief Get present open interest of a specific symbol.
        ///
        /// \param symbol Symbol name
        /// \return trader::api::OpenInterest Data
        T_NODISCARD trader::api::OpenInterest fapiOpenInterest(const std::string &symbol);

        /// \brief Get present open interest of a specific symbol statistics
        ///
        /// \param symbol Symbol name
        /// \param period Period: Only "5m","15m","30m","1h","2h","4h","6h","12h","1d" are valid
        /// \param limit Limit, max is 500. optional
        /// \param startTime Start time. optional
        /// \param endTime End time. optional
        /// \return std::vector<trader::api::OpenInterestStatistics> Data
        T_NODISCARD std::vector<trader::api::OpenInterestStatistics> fapiOpenInterestStatistics(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit = 30, const uint64_t &startTime = 0, const uint64_t &endTime = 0);

        /// \brief Top Trader Long/Short Ratio (Accounts)
        ///
        /// \param symbol Symbol name
        /// \param period Period: Only "5m","15m","30m","1h","2h","4h","6h","12h","1d"
        /// \param limit Limit. optional. max 500
        /// \param startTime Start time. optional
        /// \param endTime End time. optional
        /// \return std::vector<TopTraderLongShortRatioAccounts> Data
        T_NODISCARD std::vector<trader::api::TopTraderLongShortRatioAccounts> fapiTopTraderLongShortRatioAccounts(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit = 30, const uint64_t &startTime = 0, const uint64_t &endTime = 0);

        /// \brief Top Trader Long/Short Ratio (Positions)
        ///
        /// \param symbol Symbol name
        /// \param period Period: Only "5m","15m","30m","1h","2h","4h","6h","12h","1d"
        /// \param limit Limit. optional. max 500
        /// \param startTime Start time. optional
        /// \param endTime End time. optional
        /// \return std::vector<TopTraderLongShortRatioPositions> Data
        T_NODISCARD std::vector<trader::api::TopTraderLongShortRatioPositions> fapiTopTraderLongShortRatioPositions(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit = 30, const uint64_t &startTime = 0, const uint64_t &endTime = 0);

        /// \brief Long/Short Ratio
        ///
        /// \param symbol Symbol name
        /// \param period Period: Only "5m","15m","30m","1h","2h","4h","6h","12h","1d"
        /// \param limit Limit. optional. max 500
        /// \param startTime Start time. optional
        /// \param endTime End time. optional
        /// \return std::vector<TopTraderLongShortRatioPositions> Data
        T_NODISCARD std::vector<trader::api::TopTraderLongShortRatio> fapiLongShortRatio(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit = 30, const uint64_t &startTime = 0, const uint64_t &endTime = 0);

        /// \brief Taker Buy/Sell Volume
        ///
        /// \param symbol Symbol name
        /// \param period Period. Only valid "5m","15m","30m","1h","2h","4h","6h","12h","1d"
        /// \param limit Limit. Max. 500
        /// \param startTime Start time optional
        /// \param endTime End time optional
        /// \return std::vector<trader::api::FuturesTakerBuySellVolume>
        T_NODISCARD std::vector<trader::api::FuturesTakerBuySellVolume> fapiTakerBuySellVolume(const std::string &symbol, const BinanceTimeIntervals &period, const int &limit = 30, const uint64_t &startTime = 0, const uint64_t &endTime = 0);

        /// \brief Get Candle data
        /// The BLVT NAV system is based on Binance Futures, so the endpoint is based on fapi
        /// \param symbol Symbol name
        /// \param interval Candle interval
        /// \param limit Candles limit
        /// \param startTime Candles start time
        /// \param endTime Candles end time
        /// \return std::vector<trader::api::Candlestick> Candle data
        T_NODISCARD std::vector<trader::api::Candlestick> fapiHistoricalBLVTNAVCandlestick(const std::string &symbol, const BinanceTimeIntervals &interval, const int &limit = 500, const uint64_t startTime = 0, const uint64_t endTime = 0);

        /// \brief Composite Index Symbol Information
        ///
        /// \param symbol Symbol name
        /// \return trader::api::CompositeIndexSymbolInformation Data
        T_NODISCARD trader::api::CompositeIndexSymbolInformation fapiCompositeIndexSymbolInformation(const std::string &symbol);

        /// \brief Change user's position mode (Hedge Mode or One-way Mode ) on EVERY symbol
        ///
        /// \param dualSidePosition "true": Hedge Mode; "false": One-way Mode
        /// \param recvWindow Received Frame Windoed
        /// \return std::pair<int, std::string> std::pair<int, std::string> Code, Message
        T_NODISCARD std::pair<int, std::string> fapiChangePositionMode(const bool &dualSidePosition, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Get user's position mode (Hedge Mode or One-way Mode ) on EVERY symbol
        ///
        /// \param recvWindow Received Frame Windoed
        /// \return true Hedge mode
        /// \return false One-way mode
        T_NODISCARD bool fapiGetCurrentPositionMode(const uint64_t &recvWindow);

        /// \brief Change user's Multi-Assets mode (Multi-Assets Mode or Single-Asset Mode) on Every symbol
        ///
        /// \param multiAssetsMargin "true": Multi-Assets Mode; "false": Single-Asset Mode
        /// \param recvWindow Received Frame Window
        /// \return std::pair<int, std::string> Code, Msg
        T_NODISCARD std::pair<int, std::string> fapiChangeMultiAssetsMode(const bool &multiAssetsMargin, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Get user's Multi-Assets mode (Multi-Assets Mode or Single-Asset Mode) on Every symbol
        ///
        /// \param recvWindow Received Frame Windoed
        /// \return true Multi-assets mode
        /// \return false Single-asset mode
        T_NODISCARD bool fapiGetMultiAssetsMode(const uint64_t &recvWindow);

        /// \brief Get Account Futures Balance
        ///
        /// \param recvWindow Received Frame Window
        /// \return std::vector<trader::api::FuturesAccountBalance> Data
        T_NODISCARD std::vector<trader::api::FuturesAccountBalance> fapiGetAccountBalance(const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Get current account information.
        ///
        /// \param recvWindow Received Frame Window
        /// \return trader::api::FuturesAccountInformation data
        T_NODISCARD trader::api::FuturesAccountInformation fapiGetAccountInformation(const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Change user's initial leverage of specific symbol market.
        ///
        /// \param symbol Symbol name
        /// \param leverage target initial leverage: int from 1 to 125
        /// \param recvWindow Received Frame Window
        /// \return trader::api::InitialLeverage Data
        T_NODISCARD trader::api::InitialLeverage fapiChangeInitialLeverage(const std::string &symbol, const int64_t &leverage, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Change user's margin type to isolated of specific symbol market
        ///
        /// \param symbol Symbol name
        /// \param recvWindow Received Frame Window
        /// \return std::pair<int, std::string> Code, Message
        T_NODISCARD std::pair<int, std::string> fapiChangeMarginTypeIsolated(const std::string &symbol, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Change user's margin type to crossed of specific symbol market
        ///
        /// \param symbol Symbol name
        /// \param recvWindow Received Frame Window
        /// \return std::pair<int, std::string> Code, Message
        T_NODISCARD std::pair<int, std::string> fapiChangeMarginTypeCrossed(const std::string &symbol, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Modify Isolated Position Margin
        ///
        /// \param symbol Symbol name
        /// \param amount Amount
        /// \param pos Position Margin Type
        /// \param recvWindow Received Frame Window
        /// \return std::tuple<double, int64_t, std::string, int64_t> amount, code, msg, type
        T_NODISCARD std::tuple<double, int, std::string, trader::api::PositionMargin> fapiModifyIsolatedPositionMarginOneWayMode(const std::string &symbol, const quantity_t &amount, const PositionMargin &pos, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Modify Isolated Position Margin For Hedge mode and long
        ///
        /// \param symbol Symbol name
        /// \param amount Amount
        /// \param pos Position margin type
        /// \param recvWindow Received Frame Window
        /// \return std::tuple<double, int64_t, std::string, int64_t> amount, code, msg, type
        T_NODISCARD std::tuple<double, int, std::string, trader::api::PositionMargin> fapiModifyIsolatedPositionMarginHedgeModeLong(const std::string &symbol, const quantity_t &amount, const PositionMargin &pos, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Modify Isolated Position Margin for Hedge mode and short
        ///
        /// \param symbol Symbol name
        /// \param amount Amount
        /// \param pos Position margin type
        /// \param recvWindow Received Frame Window
        /// \return std::tuple<double, int64_t, std::string, int64_t> amount, code, msg, type
        T_NODISCARD std::tuple<double, int, std::string, trader::api::PositionMargin> fapiModifyIsolatedPositionMarginHedgeModeShort(const std::string &symbol, const quantity_t &amount, const PositionMargin &pos, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Get Position Margin Change History
        ///
        /// \param symbol Symbol name
        /// \param pos Position margin type
        /// \param limit Limit
        /// \param startTime Start time
        /// \param endTime End time
        /// \param recvWindow Received Frame Window
        /// \return std::vector<trader::api::PositionMarginChange>
        T_NODISCARD std::vector<trader::api::PositionMarginChange> fapiGetPositionMarginChangeHistory(const std::string &symbol, const PositionMargin &pos, const uint64_t &limit = 500, const uint64_t &startTime = 0, const uint64_t endTime = 0, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Get current position information.
        ///
        /// \param symbol Symbol name
        /// \param recvWindow Received Frame Window
        /// \return std::vector<trader::api::PositionInformation> data
        T_NODISCARD std::vector<trader::api::PositionInformation> fapiGetPositionInformation(const std::string &symbol = "", const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Get trades for a specific account and symbol. from time
        ///
        /// \param symbol Symbol name
        /// \param limit Default 500; max 1000.
        /// \param startTime Start time
        /// \param endTime End time
        /// \param recvWindow Received Frame Window
        /// \return std::vector<trader::api::AccounTradeList> data
        T_NODISCARD std::vector<trader::api::AccountTradeList> fapiGetAccountTradeList(const std::string &symbol, const uint64_t &limit = 500, const uint64_t &startTime = 0, const uint64_t endTime = 0, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Get trades for a specific account and symbol. from id
        ///
        /// \param symbol Symbol name
        /// \param fromId Trade Id
        /// \param recvWindow Received Frame Window
        /// \return std::vector<trader::api::AccounTradeList> data
        T_NODISCARD std::vector<trader::api::AccountTradeList> fapiGetAccountTradeList(const std::string &symbol, const uint64_t &fromId, const uint64_t &limit = 500, const uint64_t &recvWindow = BINANCEAPI_DEFAULT_RECVWINDOW);

        /// \brief Start a new user data stream. The stream will close after 60 minutes unless a keepalive is sent.
        ///        If the account has an active listenKey, that listenKey will be returned and its validity will be extended for 60 minutes.
        ///
        /// \return std::string The listen key. Use in WSFuturesBinanceUser
        T_NODISCARD std::string fapiStartUserDataStream();

        /// \brief Keepalive a user data stream to prevent a time out. User data streams will close after 60 minutes. It's recommended to send a ping about every 60 minutes.
        ///
        void fapiKeepAliveUserDataStream();

        /// \brief Close out a user data stream.
        ///
        void fapiCloseUserDataStream();

        /*******************************************************************************
         * Futures Order Creation
         * This functions handle all possible combinations with One-way or Hedge Mode
         *******************************************************************************/

        T_NODISCARD OrderAcknowledge fapiNewOrderOneWayLimit(const std::string &symbol,
            const uint64_t &recvWindow,
            const std::string &userId,
            const OrderSide &side,
            const OrderTimeInForce &otif,
            const quantity_t &qty,
            const currency_t &price,
            const trader::api::FuturesExchangeInformationSymbol &symInfo,
            const bool &reduceOnly = false);

        T_NODISCARD OrderAcknowledge fapiCancelOrder(const std::string &symbol, const uint64_t &recvWindow, const std::string &userOrderId);
/*
        T_NODISCARD OrderAcknowledge fapiNewOrderOneWayTakeProfitLimit(const std::string &symbol,
            const uint64_t &recvWindow,
            const std::string &userId,
            const OrderSide &side,
            const OrderTimeInForce &otif,
            const currency_t &price,
            const currency_t &qu
            const trader::api::FuturesExchangeInformationSymbol &symInfo,
            const WorkingType &wt = WorkingType::contractPrice);*/

        T_NODISCARD int fapiNewOrderOneWayMarket();

        T_NODISCARD int fapiNewOrderOneWayStop();

        T_NODISCARD int fapiNewOrderOneWayTakeProfit();

        T_NODISCARD int fapiNewOrderOneWayStopMarket();

        T_NODISCARD int fapiNewOrderOneWayTakeProfitMarket();

        T_NODISCARD int fapiNewOrderOneWayTrailingStopMarket();

        T_NODISCARD int fapiNewOrderHedgeModeLimit();

    private:
        std::string m_spotAddress;
        std::string m_futuresAddres;
        std::string m_userAgent;
        double m_lastCallSeconds { 0.0 };
        BinanceKeys *m_binanceKeys { nullptr };
        BinanceLimits *m_limits { nullptr };
    };

} // namespace trader::api

#endif /*BINAPI_HPP_*/
