////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#pragma once

#ifndef BINAPI_HPP_
#define BINAPI_HPP_

#include "BinanceAPIGlobal.hpp"
#include "BinanceAPILimits.hpp"
//////
#include "BinanceAPIDefs.hpp"

#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace BINAPI_NAMESPACE
{
    namespace local
    {
        enum class BINAPI_HIDDEN HTTPRequest
        {
            GET,
            POST,
            DELETE,
            PUT
        };

        struct BINAPI_HIDDEN BinanceAPIRequest final
        {
            /// Some exchanges may define a weight for each call
            /// This parameter will be passed to Limits::secure...Call()
            long long callWeight { 0 };
            /// Request endpoint
            std::string_view endPoint {};
            /// HTTP request type
            HTTPRequest http { HTTPRequest::GET };
            /// This field is to manage correctly the Binance Limits for orders
            bool isOrderCall { false };
            /// Normal response Schema
            std::unique_ptr<rapidjson::SchemaDocument> docSchema { nullptr };
            /// Error Schema
            std::unique_ptr<rapidjson::SchemaDocument> errorSchema { nullptr };
            /// A user data type
            bool isSAPI;
            /// When signed prevent to send a signature only an API-key in the header. This is the third parameter in secureRequest
            bool preventSignatureWhenSigned { false };
        };

        auto BINAPI_HIDDEN loadJsonSchema(const char *schemaData) noexcept -> rapidjson::SchemaDocument;

        auto BINAPI_HIDDEN loadSchema(const char *data) -> std::unique_ptr<rapidjson::SchemaDocument>;
    } // namespace local

    using eparams = std::vector<std::pair<std::string, std::string>>;

    // Return types
    using AllCoinsInformation = std::unordered_map<BINAPI_NAMESPACE::sym_t, BINAPI_NAMESPACE::SPOT::CoinInformation>;

    /// \brief Binance user keys
    struct BINAPI_EXPORT BinanceKeys
    {
        std::string apiKey;
        std::string secretKey;
    };

    class BINAPI_EXPORT BinanceAPI
    {
    public:
        BinanceAPI(std::string_view baseURL, std::string_view userAgent, BinanceKeys *keys, BinanceLimits *exl) noexcept;
        virtual ~BinanceAPI() noexcept;

    public:
        /// \brief Converts BinanceTimeIntervals to a string that the BinanceAPI understands
        ///
        /// \param interval Time interval
        /// \return constexpr std::string_view The string. e.g. fromIntervalToString(BinanceTimeIntervals::i12h) returns std::string_view{"12h"}
        static constexpr auto fromIntervalToString(const BINAPI_NAMESPACE::BinanceTimeIntervals &interval) noexcept -> std::string_view
        {
            switch (interval)
            {
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1m:
                    return "1m";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i3m:
                    return "3m";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i5m:
                    return "5m";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i15m:
                    return "15m";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i30m:
                    return "30m";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1h:
                    return "1h";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i2h:
                    return "2h";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i4h:
                    return "4h";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i6h:
                    return "6h";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i8h:
                    return "8h";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i12h:
                    return "12h";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1d:
                    return "1d";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i3d:
                    return "3d";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1w:
                    return "1w";
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1M:
                    return "1M";
            }
        }

        /// \brief Returns a conversion of BinanceTimeIntervals to milliseconds
        /// \note This function is intended to be use in conjuction with trader::api::BinanceAPI::getCandlesTimesAndLimits
        ///
        /// \param interval Time interval
        /// \return constexpr uint64_t The milliseconds of the interval. e.g. fromIntervalToMilliseconds(BinanceTimeIntervals::i6h) returns uint64_t{60'000ull * 60 * 6}
        static inline constexpr auto fromIntervalToMilliseconds(const BinanceTimeIntervals &interval) noexcept -> uint64_t
        {
            switch (interval)
            {
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1m:
                    return 60'000ull; // 60,000 milliseconds in a minute
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i3m:
                    return 60'000ull * 3; // * minutes
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i5m:
                    return 60'000ull * 5;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i15m:
                    return 60'000ull * 15;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i30m:
                    return 60'000ull * 30;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1h:
                    return 60'000ull * 60; // * 60 minutes
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i2h:
                    return 60'000ull * 60 * 2; // * 60 minutes * 2 hours
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i4h:
                    return 60'000ull * 60 * 4;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i6h:
                    return 60'000ull * 60 * 6;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i8h:
                    return 60'000ull * 60 * 8;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i12h:
                    return 60'000ull * 60 * 12;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1d:
                    return 60'000ull * 60 * 24;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i3d:
                    return 60'000ull * 60 * 24 * 3; // *60 minutes * 24 hours (24 hours in a day) * 3 days
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1w:
                    return 60'000ull * 60 * 24 * 7;
                case BINAPI_NAMESPACE::BinanceTimeIntervals::i1M:
                    return 60'000ull * 60 * 24 * 30;
            }
        }

    protected:
        /// \brief Compute HMAC 256
        ///
        /// \param key API Key
        /// \param plainText Plain text to cipher
        /// \param out A char array of at least EVP_MAX_MD_SIZE size
        static auto HMAC256(const std::string &key, const std::string &plainText, char *out) noexcept -> void;

        auto getKeys() const noexcept -> BinanceKeys *;
        auto getLimits() const noexcept -> BinanceLimits *;

        /// \brief Make an API rest request
        ///
        /// \param request  Request parameters
        /// \param params Parameters of the call. There is no guarantee this remains intact
        /// \param secure Will call ExchangeAPI::secure to ensure a secure request
        /// \param timeLog On success a JSON Document otherwise will throw
        /// \return rapidjson::Document ExchangeAPIError on error
        auto request(const local::BinanceAPIRequest &request, const eparams &params, const bool &secure = false, double *const timeLog = nullptr) -> rapidjson::Document;

    private:
        auto secureRequest(cpr::Session &session, cpr::Parameters &parameters, const cpr::CurlHolder &cprCurlHolder, const bool &preventSignature) noexcept -> void;

        /// \brief Handle an API Error request
        ///
        /// \param data The user defined by the user
        /// \param value This will set to the APIError (apiCode) in the exception
        /// \param code This will set to the APIError (apiMessage) in the exception
        /// \param message nor code not message are initialized
        /// \return true No error
        /// \return false Error
        static auto handleAPIRequestError(const bool &data, const rapidjson::Value &value, int &code, std::string &message) -> bool;

        /// \brief When an HTTP call returns a code different of 200 and the response body is a JSON object this function needs to handle
        ///
        /// \param jsonObject The JSON object string
        /// \param code This will set to the httpAPIRequestError2 (apiCode) in the exception
        /// \param message This will set to the httpAPIRequestError2 (apiMessage) in the exception
        static auto handleHTTPErrorResponse(const std::string &jsonObject, int &code, std::string &message) -> void;

    protected:
        /// \brief Wrapper for a request with no parameters
        ///
        /// \param requests Request data
        /// \param secure Secure or not
        /// \return rapidjson::Document The JSON Document
        auto apiRequest(const local::BinanceAPIRequest &requests, const bool &secure = false) -> rapidjson::Document;

        /// \brief Wrapper for a request with parameters
        ///
        /// \param requests Request data
        /// \param parameters Parameters to pass
        /// \param secure Secure or not
        /// \return rapidjson::Document The JSON Document
        auto apiRequest(const local::BinanceAPIRequest &requests, const eparams &parameters, const bool &secure = false) -> rapidjson::Document;

    public:
        /// \brief Get the last api call the time to retrieve the data from the internet
        ///
        /// \return double A double with the seconds
        /// \remarks This time does not include the time taken to parse the actual JSON data
        T_NODISCARD auto getLastCallTime() const noexcept -> double;

        /// \brief Retrieve UTC time
        ///
        /// \return uint64_t Time in milliseconds
        /// \note This uses internal clock. Does not make any API request so does not go through the internet
        T_NODISCARD static auto getTime() noexcept -> uint64_t;

        /// \brief Change the default recvWindow parameter of the API. Default is 4000
        /// \param recvWindow new value
        auto setRecvWindow(const uint32_t recvWindow) noexcept -> void;

        /// \brief Returns recvWindow to the exchange default (5000)
        auto setRecvWindowDefault() noexcept -> void;

        /// \brief Gets the recvWindow parameter
        /// \return setted value
        T_NODISCARD auto getRecvWindow() const noexcept -> uint32_t;

    public:
        /// \brief Binance API for candlesticks support only a max of 1000 candles retrieve from the exchange per API call
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
        ///            std::vector<Candlestick> candles;
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
        T_NODISCARD static auto getCandlesTimesAndLimits(const BINAPI_NAMESPACE::BinanceTimeIntervals &interval, const uint64_t &startTime, const uint64_t &endTime, uint64_t &totalExpected) noexcept -> std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>;

    private:
        std::string_view m_baseURL;
        std::string_view m_userAgent;
        BinanceKeys *m_binanceKeys { nullptr };
        BinanceLimits *m_binanceLimits { nullptr };
        double m_lastCallSeconds { 0.0 };
        uint32_t m_recvWindow { 5000 };
    };

    // Spot/Margin/Savings/Mining enp-oints wrapper
    class BINAPI_EXPORT BinanceAPISpot : public BinanceAPI
    {
    public:
        BinanceAPISpot(std::string_view userAgent, BinanceKeys *keys, BinanceLimits *exl);
        BinanceAPISpot(BinanceKeys *keys, BinanceLimits *exl);
        ~BinanceAPISpot() override;

        // WALLET ENDPOINTS
    public:
        /// \brief Get Exchange Status
        ///
        /// \return true
        /// \return false
        T_NODISCARD auto getExchangeStatus() -> bool;

        /// \brief Get information of coins (available for deposit and withdraw) for user
        /// \return Data
        T_NODISCARD auto getAllCoinsInformation() -> AllCoinsInformation;

        /// \brief Get account snapshot for the spot account
        /// \param limit Min: 7, Max 30
        /// \param startTime If not sent last sevens days are returned
        /// \param endTime If not sent last sevens days are returned
        /// \return Data
        T_NODISCARD auto getDailyAccountSnapshotSpot(uint64_t limit, uint64_t startTime = 0, uint64_t endTime = 0) -> std::vector<SPOT::AccountSnapshotSpot>;

        /// \brief Get account snapshot for the margin account
        /// \param limit Min: 7, Max 30
        /// \param startTime If not sent last sevens days are returned
        /// \param endTime If not sent last sevens days are returned
        /// \return Data
        T_NODISCARD auto getDailyAccountSnapshotMargin(uint64_t limit, uint64_t startTime = 0, uint64_t endTime = 0) -> std::vector<SPOT::AccountSnapshotMargin>;

        /// \brief Get account snapshot for the futures account
        /// \param limit Min: 7, Max 30
        /// \param startTime If not sent last sevens days are returned
        /// \param endTime If not sent last sevens days are returned
        /// \return Data
        T_NODISCARD auto getDailyAccountSnapshotFutures(uint64_t limit, uint64_t startTime = 0, uint64_t endTime = 0) -> std::vector<SPOT::AccountSnapshotFutures>;

        /// \brief This request will disable fastwithdraw switch under your account.
        /// You need to enable "trade" option for the api key which requests this endpoint
        auto disableFastWithdrawSwitch() -> void;

        /// \brief This request will enable fastwithdraw switch under your account.
        /// You need to enable "trade" option for the api key which requests this endpoint.
        /// When Fast Withdraw Switch is on, transferring funds to a Binance account will be done instantly.
        /// There is no on-chain transaction, no transaction ID and no withdrawal fee.
        auto enableFastWithdrawSwitch() -> void;

        /// \brief Withdraw
        /// \param coin Coin name
        /// \param address Destination Address
        /// \param network Network to use
        /// \param amount Amount to transfer
        /// \param orderId Client Order ID. If empty, it will no be passed
        /// \param addressTag Secondary address identifier. If empty it will not be passed
        /// \param transactionFeeFlag When making internal transfer, true for returning the fee to the destination account; false for returning the fee back to the departure account. Exchange Default is false.
        /// \param name Description of the address. All spaces will be encoded automatically encoded as %20 as Binance requires spaces to be encoded as %20.
        /// \param wType Wallet type. Exchange default is spot Wallet
        /// \return The transfer id
        /// \note Although BinanceAPI does not make network mandatory, this interface does
        T_NODISCARD auto withdraw(v_asset_t coin, v_address_t address, std::string_view network, currency_t amount,
            std::string_view orderId    = "",
            std::string_view addressTag = "",
            bool transactionFeeFlag     = false,
            std::string_view name       = "",
            SPOT::WalletType wType      = SPOT::WalletType::spot) -> std::string;

        /// \brief Deposit history
        /// \param coin Coin name
        /// \param limit MAX 1000
        /// \param offset Data offset. Default: 0
        /// \param startTime Make sure the period between startTime and endTime is 90 days. If zero the parameter will not be sent
        /// \param endTime End time. If zero the parameter will not be sent
        /// \param status Transaction status. TransactionDepositHistoryStatus::undefined is an internal state and passing it
        ///               will cause this parameter to be omitted. However, if the exchange updates this values, and do not correspond
        ///               to the internal states, the function will return the undefined state until this interface is updated
        /// \return An array with the values

        T_NODISCARD auto depositHistory(v_asset_t coin, uint64_t limit = 1000, uint64_t offset = 0,
            uint64_t startTime = 0, uint64_t endTime = 0,
            SPOT::TransactionDepositHistoryStatus status = SPOT::TransactionDepositHistoryStatus::undefined) -> std::vector<SPOT::DepositHistory>;

        /// \brief Deposit history
        /// \param coin Coin name
        /// \param limit MAX 1000
        /// \param offset Data offset. If zero the parameter will not be sent
        /// \param startTime Make sure the period between startTime and endTime is 90 days. If zero the parameter will not be sent
        /// \param endTime End time. If zero the parameter will not be sent
        /// \param status Transaction status. TransactionWithdrawHistoryStatus::undefined is an internal state and passing it
        ///               will cause this parameter to be omitted. However, if the exchange updates this values, and do not correspond
        ///               to the internal states, the function will return the undefined state until this interface is updated
        /// \return An array with the values
        T_NODISCARD auto withdrawHistory(v_asset_t coin, uint64_t limit = 1000, uint64_t offset = 0,
            uint64_t startTime = 0, uint64_t endTime = 0,
            SPOT::TransactionWithdrawHistoryStatus status = SPOT::TransactionWithdrawHistoryStatus::undefined) -> std::vector<SPOT::WithdrawHistory>;

        /// \brief Deposit history
        /// \param coin Coin name
        /// \param limit MAX 1000
        /// \param offset Data offset
        /// \param startTime Make sure the period between startTime and endTime is 90 days
        /// \param endTime End time
        /// \param status Transaction status. TransactionWithdrawHistoryStatus::undefined is an internal state and passing it
        ///               will cause this parameter to be omitted. However, if the exchange updates this values, and do not correspond
        ///               to the internal states, the function will return the undefined state until this interface is updated
        /// \return An array with the values
        T_NODISCARD auto withdrawHistory(std::string_view clientOrderId, v_asset_t coin, uint64_t limit = 1000, uint64_t offset = 0,
            uint64_t startTime = 0, uint64_t endTime = 0,
            SPOT::TransactionWithdrawHistoryStatus status = SPOT::TransactionWithdrawHistoryStatus::undefined) -> std::vector<SPOT::WithdrawHistory>;

        /// \brief Returns the deposit address of a coin
        /// \param coin Coin
        /// \param network You may or may not send this parameter
        /// \return Address data
        T_NODISCARD auto depositAddress(v_asset_t coin, std::string_view network = "") -> SPOT::DepositAddress;

        /// \brief Retrive the account status
        /// \return Status string
        T_NODISCARD auto accountStatus() -> std::string;

        /// \brief Fetch account api trading status detail
        /// \return STATUS
        T_NODISCARD auto accountAPITradingStatus() -> SPOT::AccountAPITradingStatus;

        /// \brief DustLog
        /// \param startTime Start time
        /// \param endTime End time
        /// \return Return only 100 records if times are not specified and only returns records from 2020/21/01
        T_NODISCARD auto dustLog(uint64_t startTime = 0, uint64_t endTime = 0) -> SPOT::DustLog;

        /// \brief Assets that can be converted to BNB
        /// \return assets information
        T_NODISCARD auto getAssetsConvertedToBNB() -> SPOT::AssetsToBNB;

        /// \brief Convert dust assets to BNB
        /// \param assets Array of assets
        /// \return Transfer data
        T_NODISCARD auto dustTransfer(const std::vector<asset_t> &assets) -> SPOT::DustTransfer;

        /// \brief Query Asset Dividend Record
        /// \param asset Asset Name (can be empty)
        /// \param limit Max is 500 will assert when limit > 500
        /// \param startTime Start time of the query. Can be omitted
        /// \param endTime End time of the query. Can be omitted
        /// \return A pair in the first value the number of rows and the second is a vector with the row data
        T_NODISCARD auto assetDividendRecord(v_asset_t asset, uint64_t limit = 20, uint64_t startTime = 0, uint64_t endTime = 0) -> std::pair<int64_t, std::vector<SPOT::AssetDividend>>;

        /// \brief Fetch deatails of asset supported on Binance. Currently only one asset detail is supported
        /// \param asset Asset Name (can be empty)
        /// \return The corresponding information
        T_NODISCARD auto assetDetail(v_asset_t asset) -> SPOT::AssetDetail;

        /// \brief Fetch trade fee
        /// \param symbol Symbol Name (can be empty)
        /// \return The corresponding fees
        T_NODISCARD auto tradeFee(v_sym_t symbol) -> std::map<sym_t, SPOT::TradeFee>;

        /// \brief User Universal Transfer. Make user your API Key has enabled the Permits Universal Transfer
        /// \param type Transfer type
        /// \param asset Asset Name
        /// \param amount Amount
        /// \param precision Amount precision
        /// \param fromSymbol If you pass something when is not needed it will be ignored. Optional: VIEW THE API BINANCE DOCUMENTATION!
        /// \param toSymbol If you pass something when is not needed it will be ignored. Optional: VIEW THE API BINANCE DOCUMENTATION!
        /// \return A transfer ID
        T_NODISCARD auto userUniversalTransfer(SPOT::UniversalTransferType type,
            v_asset_t asset,
            currency_t amount,
            int64_t precision,
            v_sym_t fromSymbol,
            v_sym_t toSymbol) -> uint64_t;

        /// \brief Query a universal transfer
        /// \param type  Transfer type
        /// \param fromSymbol If you pass something when is not needed it will be ignored. Optional: VIEW THE API BINANCE DOCUMENTATION!
        /// \param toSymbol If you pass something when is not needed it will be ignored. Optional: VIEW THE API BINANCE DOCUMENTATION!
        /// \param current Current
        /// \param size Size. Max is 100. will assert when size > 100
        /// \param startTime Start time of the query
        /// \param endTime End time of the query
        /// \return A key with the first parameter with the number of rows (as returned from the exchange). The second has a vector with the information of the query
        T_NODISCARD auto queryUserUniversalTransferHistory(
            SPOT::UniversalTransferType type,
            v_sym_t fromSymbol,
            v_sym_t toSymbol,
            int64_t current    = 1,
            int64_t size       = 10,
            uint64_t startTime = 0,
            uint64_t endTime   = 0) -> std::pair<int64_t, std::vector<SPOT::UniversalTransferQuery>>;

        /// \brief Funding wallet for an specific asset
        /// \param asset Asset Name
        /// \param needBtcValuation When BTC Valuation is needed
        /// \return Funding Wallet history
        T_NODISCARD auto fundingWallet(v_asset_t asset, bool needBtcValuation) -> std::vector<SPOT::FundingWallet>;

        /// \brief Funding wallet without an asset
        /// \param needBtcValuation When BTC Valuation is needed
        /// \return Funding Wallet history
        T_NODISCARD auto fundingWallet(bool needBtcValuation) -> std::vector<SPOT::FundingWallet>;

        /// \brief Retrieve the API Key permissions
        /// \return API Key Permissions
        T_NODISCARD auto getAPIKeyPermission() -> SPOT::APIKeyPermissions;

        // MARKET DATA ENDPOINTS
    public:
        /// \brief Test for server ping. In case a ping failure happens. The function will throw an exception
        auto ping() -> void;

        /// \brief Check for server time
        /// \return Server time
        T_NODISCARD auto checkServerTime() -> uint64_t;

        /// \brief Retrieve Exchange Information
        /// \return The exchange information
        T_NODISCARD auto getExchangeInformation() -> SPOT::ExchangeInformation;

        /// \brief Retrive the book order. Useful for keeping in order the WebSocket orderbook
        /// \param symbol Symbol name
        /// \param limit Limit of the entries. 100 is the default and the max is 5000. If the max is exceeded it will be truncated to 5000. Although the BinanceAPI does this,
        ///                this wrapper does it before sending it to the servers
        /// \return Returns the order book
        T_NODISCARD auto getOrderBook(v_sym_t symbol, uint32_t limit = 100) -> SPOT::OrderBook;
    };

    // COIN-M Futures end-points wrapper
    class BINAPI_EXPORT BinanceAPIFutures : public BinanceAPI
    {
    };

} // namespace BINAPI_NAMESPACE

#endif /*BINAPI_HPP_*/
