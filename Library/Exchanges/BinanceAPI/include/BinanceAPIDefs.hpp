////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#pragma once

#ifndef BINDEFS_HPP_
#define BINDEFS_HPP_

#include <bitset>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace BINAPI_NAMESPACE
{
    // Use to define a currency
    using currency_t = double;
    // Use to manage percentages
    using percentage_t = double;
    // Use to manage an asset quantity
    using quantity_t = double;
    // Use the fixed string for symbol. Fix the size to a 32-byte string
    using v_sym_t = std::string_view;
    using sym_t   = std::string;
    // Asset definition. Although an asset size is less than a symbol; making them the same size makes the operation between them easier
    using v_asset_t = std::string_view;
    using asset_t   = std::string;
    // Addresses
    using address_t   = std::string;
    using v_address_t = std::string;
    // Define the User ID type. Although Binance defines the client with 36 characters ( ^[\.A-Z\:/a-z0-9_-]{1,36}$ ).
    // When Binance Liquidates the order it appends at the start of the string 'autoclose-' (10 bytes) so make sure you take into account that in
    // The length of the order string
    using vu_order_t = std::string_view;
    using u_order_t  = std::string;

    enum class OrderTimeInForce
    {
        // An order will be on the book unless the order is canceled.
        GTC = 0, // Good Til Canceled
                 // An order will try to fill the order as much as it can before the order expires.
        IOC,     // Immediate Or Cancel
                 // An order will expire if the full order cannot be filled upon execution.
        FOK,     // Fill or Kill
        GTX,     // GTX is not supported under the SPOT API only Futures API
    };

    enum class OrderSide
    {
        buy = 0,
        sell
    };

    /// \brief Binance time intervals
    /// m: minutes
    /// h: hours
    /// d: days
    /// w: week
    /// M: months
    enum class BinanceTimeIntervals
    {
        i1m,
        i3m,
        i5m,
        i15m,
        i30m,
        i1h,
        i2h,
        i4h,
        i6h,
        i8h,
        i12h,
        i1d,
        i3d,
        i1w,
        i1M
    };

    enum class MarginType
    {
        crossed,
        isolated
    };

    struct AssetBalance
    {
        quantity_t free { 0 };
        quantity_t locked { 0 };
    };

    struct MarginAssetBalance
    {
        currency_t borrowed;
        currency_t free;
        currency_t interest;
        currency_t locked;
        currency_t netAsset;
    };

    namespace SPOT
    {
        enum class WalletType
        {
            spot    = 0,
            funding = 1,
        };

        enum class TransferType
        {
            internal,
            external,
            undefined
        };

        enum class TransactionDepositHistoryStatus
        {
            pending   = 0,
            success   = 1,
            credited  = 6, // But not cannot withdraw
            undefined = 100
        };

        enum class TransactionWithdrawHistoryStatus
        {
            emailSent        = 0,
            cancelled        = 1,
            awaitingApproval = 2,
            rejected         = 3,
            processing       = 4,
            failure          = 5,
            completed        = 6,
            undefined        = 100
        };

        enum class UniversalTransferType
        {
            MAIN_UMFUTURE,                 // Spot account transfer to USDⓈ-M Futures account
            MAIN_CMFUTURE,                 // Spot account transfer to COIN-M Futures account
            MAIN_MARGIN,                   // Spot account transfer to Margin（cross）account
            UMFUTURE_MAIN,                 // USDⓈ-M Futures account transfer to Spot account
            UMFUTURE_MARGIN,               // USDⓈ-M Futures account transfer to Margin（cross）account
            CMFUTURE_MAIN,                 // COIN-M Futures account transfer to Spot account
            CMFUTURE_MARGIN,               // COIN-M Futures account transfer to Margin(cross) account
            MARGIN_MAIN,                   // Margin（cross）account transfer to Spot account
            MARGIN_UMFUTURE,               // Margin（cross）account transfer to USDⓈ-M Futures
            MARGIN_CMFUTURE,               // Margin（cross）account transfer to COIN-M Futures
            ISOLATEDMARGIN_MARGIN,         // Isolated margin account transfer to Margin(cross) account
            MARGIN_ISOLATEDMARGIN,         // Margin(cross) account transfer to Isolated margin account
            ISOLATEDMARGIN_ISOLATEDMARGIN, // Isolated margin account transfer to Isolated margin account
            MAIN_FUNDING,                  // Spot account transfer to Funding account
            FUNDING_MAIN,                  // Funding account transfer to Spot account
            FUNDING_UMFUTURE,              // Funding account transfer to UMFUTURE account
            UMFUTURE_FUNDING,              // UMFUTURE account transfer to Funding account
            MARGIN_FUNDING,                // MARGIN account transfer to Funding account
            FUNDING_MARGIN,                // Funding account transfer to Margin account
            FUNDING_CMFUTURE,              // Funding account transfer to CMFUTURE account
            CMFUTURE_FUNDING,              // CMFUTURE account transfer to Funding account
        };

        enum class TradingStatus : int32_t
        {
            Trading,
            Break
        };

        /// \brief Bits on AccountInformation::permissions
        enum AccountPermissions
        {
            spot = 0,
            margin,
            leveraged
        };

        struct CoinInformation
        {
            struct NetworkList
            {
                asset_t coin;
                std::string addressRegEx;
                std::string depositDescription;
                std::string memoRegEx;
                std::string name;
                std::string network;
                std::string specialTips;
                std::string withdrawDescription;
                currency_t withdrawFee;
                currency_t withdrawIntegerMultiple;
                currency_t withdrawMin;
                currency_t withdrawMax;
                int64_t minConfirmation;
                int64_t unLockConfirm;
                bool depositEnable;
                bool isDefault;
                bool resetAddressStatus;
                bool withdrawEnable;
                bool sameAddress;
            };

            asset_t coin;
            asset_t name;
            currency_t free;
            currency_t freeze;
            currency_t ipoable;
            currency_t ipoing;
            currency_t locked;
            currency_t storage;
            currency_t withdrawing;
            bool depositAlLEnable;
            bool isLegalMoney;
            bool trading;
            bool withdrawAllEnable;
            std::vector<NetworkList> networkList;
        };

        struct AccountSnapshotSpot
        {
            uint64_t updateTime;
            currency_t totalAssetOfBtc;
            std::unordered_map<asset_t, AssetBalance> balances;
        };

        struct AccountSnapshotMargin
        {
            uint64_t updateTime;
            currency_t marginLevel;
            currency_t totalAssetOfBtc;
            currency_t totalLiabilityOfBtc;
            currency_t totalNetAssetOfBtc;
            std::unordered_map<asset_t, MarginAssetBalance> balances;
        };

        struct AccountSnapshotFutures
        {
            struct FuturesAssetBalance
            {
                currency_t marginBalance;
                currency_t walletBalance;
            };

            struct FuturesPositionInformation
            {
                currency_t entryPrice;
                currency_t markPrice;
                currency_t positionAmt;
                currency_t unRealizedProfit;
            };

            uint64_t updateTime;
            std::unordered_map<asset_t, FuturesAssetBalance> balances;
            std::unordered_map<asset_t, FuturesPositionInformation> positions;
        };

        struct DepositHistory
        {
            asset_t coin;
            address_t address;
            address_t addressTag;
            std::string txId;
            std::string network;
            std::string unlockConfirm;
            std::string confirmTimes;
            currency_t amount;
            uint64_t insertTime;
            TransactionDepositHistoryStatus status;
            TransferType transferType;
        };

        struct WithdrawHistory
        {
            asset_t coin;
            address_t address;
            std::string applyTime;
            std::string id;
            std::string withdrawOrderId;
            std::string network;
            std::string info;
            std::string txId;
            currency_t amount;
            currency_t transactionFee;
            int64_t confirmNo;
            TransactionWithdrawHistoryStatus status;
            TransferType transferType;
        };

        struct DepositAddress
        {
            address_t address;
            asset_t coin;
            std::string tag;
            std::string url;
        };

        struct AccountAPITradingStatus
        {
            uint64_t updateTime;
            uint64_t recoverTime;
            int64_t triggerGCR;
            int64_t triggerIFER;
            int64_t triggerUFR;
            bool isLocked;
        };

        struct DustLog
        {
            struct UserAssetDribbletDetails
            {
                asset_t fromAsset;
                uint64_t transId;
                uint64_t operateTime;
                currency_t serviceChargeAmount;
                currency_t amount;
                currency_t transferedAmount;
            };
            struct UserAssetDribblets
            {
                uint64_t operateTime;
                uint64_t transId;
                currency_t totalTransferedAmount;
                currency_t totalServiceChargeAmount;
                std::vector<UserAssetDribbletDetails> dribbletDetails;
            };
            int64_t totalCounts;
            std::vector<UserAssetDribblets> dribblets;
        };

        struct AssetsToBNB
        {
            struct Details
            {
                asset_t asset;
                asset_t fullName;
                currency_t amountFree;
                currency_t toBTC;
                currency_t toBNB;
                currency_t toBNBOfExchange;
                currency_t exchange;
            };
            currency_t totalTransferBTC;
            currency_t totalTransferBNB;
            currency_t dribbletPercentage;
            std::vector<Details> details;
        };

        struct DustTransfer
        {
            struct TransferResult
            {
                asset_t fromAsset;
                uint64_t tranId;
                uint64_t operateTime;
                currency_t amount;
                currency_t serviceChargeAmount;
                currency_t transferedAmount;
            };
            currency_t totalServiceCharge;
            currency_t totalTransfered;
            std::vector<TransferResult> transferResult;
        };

        struct AssetDividend
        {
            asset_t asset;
            uint64_t id;
            uint64_t divTime;
            uint64_t transId;
            currency_t amount;
            std::string enInfo;
        };

        struct AssetDetail
        {
            struct CTRDetail
            {
                std::string depositTip;
                currency_t withdrawFee;
                currency_t minWithdrawAmount;
                bool depositStatus;
                bool withdrawStatus;
            };

            struct SKYDetail
            {
                currency_t minWithdrawAmount;
                currency_t withdrawFee;
                bool depositStatus;
                bool withdrawStatus;
            };

            CTRDetail ctrDetail;
            SKYDetail skyDetail;
        };

        struct TradeFee
        {
            currency_t makerCommission;
            currency_t takerCommission;
        };

        struct UniversalTransferQuery
        {
            asset_t asset;
            std::string status;
            uint64_t timestamp;
            uint64_t tranId;
            currency_t amount;
            UniversalTransferType type;
        };

        struct FundingWallet
        {
            asset_t asset;
            currency_t free;
            currency_t locked;
            currency_t freeze;
            currency_t withdrawing;
            currency_t btcValuation;
        };

        struct APIKeyPermissions
        {
            uint64_t tradingAuthorityExpirationTime;
            uint64_t createTime;
            bool ipRestrict;
            bool enableWithdrawals;
            bool enableInternalTransfer;
            bool permitsUniversalTransfer;
            bool enableVanillaOptions;
            bool enableReading;
            bool enableFutures;
            bool enableMargin;
            bool enableSpotAndMarginTrading;
        };

        struct EIRateLimits
        {
            uint32_t seconds { 0 };
            uint32_t limit { 0 };
        };

        namespace EIFilters
        {
            struct PriceFilter
            {
                bool present { false };
                currency_t minPrice { 0 };
                currency_t maxPrice { 0 };
                quantity_t tickSize { 0 };
            };

            struct PercentPrice
            {

                currency_t multiplierUp { 0 };
                currency_t multiplierDown { 0 };
                currency_t multiplierDecimal { 0 }; // Used in futures
                int32_t averagePriceMins { 0 };     // Not used in futures
                bool present { false };
            };

            struct LotSize
            {
                quantity_t minQty { 0 };
                quantity_t maxQty { 0 };
                quantity_t stepSize { 0 };
                bool present { false };
            };

            struct MinNotional
            {
                currency_t minNotional { 0 };
                int32_t averagePriceMins { 0 }; // Not used in futures
                bool applyToMarket { false };   // Not used in futures
                bool present { false };
            };

            struct IcebergParts
            {
                int limit { 0 };
                bool present { false };
            };

            struct MarketLotSize
            {
                quantity_t minQty { 0 };
                quantity_t maxQty { 0 };
                quantity_t stepSize { 0 };
                bool present { false };
            };

            struct MaxNumOrders
            {
                int32_t maxNumOrders { 0 };
                bool present { false };
            };

            struct MaxNumAlgoOrders
            {
                int32_t maxNumAlgoOrders { 0 };
                bool present { false };
            };

            struct MaxNumIcerbergOrders
            {
                int32_t maxNumIcebergOrders { 0 };
                bool present { false };
            };

            struct MaxPosition
            {
                quantity_t maxPosition { 0 };
                bool present { false };
            };
        } // namespace EIFilters

        struct ExchangeInformationSymbol
        {
            sym_t symbol;
            asset_t baseAsset;
            asset_t quoteAsset;
            TradingStatus status;
            int32_t dbSymbolId { -1 };
            int32_t baseAssetPrecision { 0 };
            int32_t quotePrecision { 0 };
            int32_t quoteAssetPrecision { 0 };
            int32_t baseCommissionPrecision { 0 };
            int32_t quoteCommissionPrecision { 0 };
            std::bitset<7> orderType;
            std::bitset<3> permissions;
            EIFilters::PriceFilter eifPrice;
            EIFilters::PercentPrice eifPercentPrice;
            EIFilters::LotSize eifLotSize;
            EIFilters::MinNotional eifMinNotional;
            EIFilters::IcebergParts eifIcebergParts;
            EIFilters::MarketLotSize eifMarketLotSize;
            EIFilters::MaxNumOrders eifMaxNumOrders;
            EIFilters::MaxNumAlgoOrders eifMaxNumAlgoOrders;
            EIFilters::MaxNumIcerbergOrders eifMaxNumIcerbergOrders;
            EIFilters::MaxPosition eifMaxPosition;
            bool icebergAllowed { false };
            bool ocoAllowed { false };
            bool quoteOrderQtyMarketAllowed { false };
        };

        struct ExchangeInformation
        {
            std::string timezone;
            EIRateLimits limitWeight;
            EIRateLimits limitOrderSecond;
            EIRateLimits limitOrderDay;
            std::unordered_map<sym_t, ExchangeInformationSymbol> symbols;
        };

        struct OrderBookEntry
        {
            currency_t price {};
            currency_t quantity {};
        };

        struct OrderBook
        {
            uint64_t lastUpdateId { 0 };
            std::vector<OrderBookEntry> asks;
            std::vector<OrderBookEntry> bids;
        };

    } // namespace SPOT

    namespace Futures
    {
        enum class PositionSide
        {
            BOTH, // one-way
            LONG, // hedge-mode
            SHORT // hedge-mode
        };
    }

    // In  ExchangeInformationSymbol::orderType each value correspond to the bits in the bitset
    enum class OrderType
    {
        limit = 0,          // SPOT, FuturesAPI
        market,             // SPOT, FuturesAPI
        stopLoss,           // SPOT
        stopLossLimit,      // SPOT
        takeProfit,         // SPOT, FuturesAPI
        takeProfitLimit,    // SPOT
        limitMaker,         // SPOT
        stop,               // FuturesAPI
        stopMarket,         // FuturesAPI
        takeProfitMarket,   // FuturesAPI
        trailingStopMarket, // FuturesAPI
        unknown,            // Returned on Stream. must not happen, API updates may occur in returning the unknow type
    };

    enum class StreamOrderTypeFutures
    {
        market,
        limit,
        stop,
        takeProfit,
        liquidation,
        trailingStopMarket,
        unknown, // must not happen, API updates may occur in returning the unknow type
    };

    enum class StreamOrderExecutionTypeFutures
    {
        sNew,
        canceled,
        calculated, // liquidation execution
        expired,
        trade,
        unknown, // must not happen, API updates may occur in returning the unknow type
    };

    enum class StreamOrderStatusFutures
    {
        sNew,
        partiallyFilled,
        filled,
        canceled,
        expired,
        newInsurance, // Liquidation with insurance fund
        newADL,       // Counterparty liquidation
        unknown,      // must not happen, API updates may occur in returning the unknow type
    };

    enum class OrderStatus
    {
        newOrder = 0,    // The order has been accepted by the engine.
        partiallyFilled, // A part of the order has been filled.
        filled,          // The order has been completed.
        canceled,        // The order has been canceled by the user.
        pendingCancel,   // Currently unused
        rejected,        // The order was not accepted by the engine and not processed.
        expired          // The order was canceled according to the order type's rules
        // (e.g. LIMIT FOK orders with no fill, LIMIT IOC or MARKET orders that partially fill) or by the exchange
        // (e.g. orders canceled during liquidation, orders canceled during maintenance)
    };

    enum class FuturesContractType
    {
        perpetual,
        currentMonth,
        nextMonth,
        currentQuarter,
        nextQuarter
    };

    enum class ContractType
    {
        none,
        PERPETUAL,
        CURRENT_QUARTER,
        NEXT_QUARTER
    };

    enum class FutureSymbolStatus
    {
        TRADING,
        PENDING_TRADING
    };

    enum class FutureUnderlyingType
    {
        COIN,
        INDEX
    };

    enum class FutureUnderlyingSubType
    {
        DEFI,
        HOT,
        BSC,
        NFT,
        INDEX
    };

    enum class WorkingType
    {
        markPrice,
        contractPrice
    };

    enum class PositionMargin : int
    {
        addPositionMargin    = 1,
        reducePositionMargin = 2
    };

    enum class OrderBookLimits
    {
        l5,
        l10,
        l20,
        l50,
        l100,
        l500,
        l1000,
        l5000
    };

    struct Candlestick
    {
        uint64_t openTime { 0 };
        double open { 0 };
        double high { 0 };
        double low { 0 };
        double close { 0 };
        double volume { 0 }; /// If the structure is generated from a stream. This value is  equal to takerBaseAssetVolume
        uint64_t closeTime { 0 };
        double quoteAssetVolume { 0 };
        double baseAssetVolume { 0 };
        uint64_t numberOfTrades { 0 };
        double takerBaseAssetVolume { 0 };
        double takerQuoteAssetVolume { 0 };
        bool isClosed { true };     /// Stream field otherwise always true
        int64_t firstTradeId { 0 }; /// Stream field otherwise always 0
        int64_t lastTradeId { 0 };  /// Stream field otherwise always 0
    };

    struct FutureAssets
    {
        std::string asset;
        bool marginAvailable;
        double autoAssetExchange;
    };
    /*
        struct FuturesExchangeInformationSymbol
        {
            std::string symbol;
            std::string pair;
            ContractType contractType;
            uint64_t deliveryDate;
            uint64_t onboardDate;
            FutureSymbolStatus status;
            percentage_t maintMarginPercent;
            percentage_t requiredMarginPercent;
            std::string baseAsset;
            std::string quoteAsset;
            std::string marginAsset;
            int64_t pricePrecision;
            int64_t quantityPrecision;
            int64_t baseAssetPrecision;
            int64_t quotePrecision;
            FutureUnderlyingType underlyingType;
            std::vector<FutureUnderlyingSubType> underlyingSubType;
            int64_t settlePlan;
            quantity_t triggerProtect;
            quantity_t liquidationFee;
            quantity_t marketTakeBound;
            EIFilters::PriceFilter eifPrice;
            EIFilters::PercentPrice eifPercentPrice;
            EIFilters::LotSize eifLotSize;
            EIFilters::MinNotional eifMinNotional;
            EIFilters::MarketLotSize eifMarketLotSize;
            EIFilters::MaxNumOrders eifMaxNumOrders;
            EIFilters::MaxNumAlgoOrders eifMaxNumAlgoOrders;
            std::set<OrderType> orderTypes;
            std::set<OrderTimeInForce> orderTimeInForce;
        };

        struct FuturesExchangeInformation
        {
            uint64_t serverTime;
            std::string futuresType;
            EIRateLimits limitWeight;
            EIRateLimits limitOrderSecond;
            EIRateLimits limitOrderMinute;

            std::unordered_map<sym_t, FutureAssets> assets;
            std::unordered_map<sym_t, FuturesExchangeInformationSymbol> symbols;
        };
    */
    struct AccountInformation
    {
        std::string accountType;
        uint64_t makerCommission { 0 };
        uint64_t takerCommission { 0 };
        uint64_t buyerCommission { 0 };
        uint64_t sellerCommission { 0 };
        bool canTrade { false };
        bool canWithdraw { false };
        bool canDeposit { false };
        std::unordered_map<sym_t, AssetBalance> balances;
        std::bitset<3> permissions;
    };

    struct FuturesOrderBook
    {
        uint64_t lastTimeUpdate { 0 };
        uint64_t messageOutputTime;
        uint64_t transactionTime;
        //     std::vector<OrderbookEntry> asks;
        //    std::vector<OrderbookEntry> bids;
    };

    struct UserStreamOrder
    {
        uint64_t eventTime;
        sym_t symbol;
        std::string clientOrderId;
        OrderSide side;
        OrderType type;
        OrderTimeInForce orderTimeInForce;
        quantity_t orderQuantity;
        currency_t orderPrice;
        currency_t stopPrice;
        quantity_t icebergQuantity;
        int64_t orderListId;
        std::string originalIdOnCancel; // Original client order ID; This is the ID of the order being canceled
        std::string executionType;
        OrderStatus orderStatus;
        std::string rejectionCode;
        int64_t orderId;
        quantity_t lastExecutedQuantity;
        quantity_t cumulativeFilledQuantity;
        currency_t lastExecutedPrice;
        currency_t commissionAmount;
        asset_t commissionAsset;
        uint64_t transactionTime;
        int64_t tradeId;
        bool orderInBook;
        bool maker;
        uint64_t orderCreationTime;
        quantity_t cumulativeQuoteAssetQuantity;
        currency_t transactedQuotePrice;
        quantity_t quoteOrderQuantity;
    };

    struct ServerSideOrder
    {
        std::string symbol;
        std::string clientOrderId;
        uint64_t orderId;
        uint64_t transactionTime;
    };

    struct QueryOrder
    {
        std::string symbol;
        uint64_t orderId;
        int64_t orderListId;
        std::string clientOrderId;
        currency_t price;
        quantity_t origQty;
        quantity_t executedQty;
        quantity_t cumulativeQuoteQty;
        OrderStatus status;
        OrderTimeInForce timeInForce;
        OrderType type;
        OrderSide side;
        currency_t stopPrice;
        currency_t icebergQty;
        uint64_t time;
        uint64_t updateTime;
        bool isWorking;
        quantity_t origQuoteOrderQty;
    };

    struct CancelOrder
    {
        std::string symbol;
        uint64_t orderId;
        int64_t orderListId;
        std::string clientOrderId;
        std::string cancelClientOrderId;
        currency_t price;
        quantity_t origQty;
        quantity_t executedQty;
        quantity_t cumulativeQuoteQty;
        OrderStatus status;
        OrderTimeInForce timeInForce;
        OrderType type;
        OrderSide side;
    };

    struct FuturesRecentTradesList
    {
        int64_t id;
        currency_t price;
        quantity_t quantity;
        quantity_t quoteQuantity;
        uint64_t time;
        bool isBuyerMaker;
    };

    struct FuturesCompressedAggregateTradeLists
    {
        int64_t id;
        currency_t price;
        quantity_t quantity;
        int64_t firstTradeId;
        int64_t lastTradeId;
        uint64_t timestamp;
        bool isBuyerMaker;
    };

    struct MarkPrice
    {
        std::string symbol;
        currency_t markPrice;
        currency_t indexPrice;
        percentage_t lastFundingRate;
        uint64_t nextFundingTime;
        percentage_t interestRate;
        uint64_t time;
    };

    struct FundingRate
    {
        std::string symbol;
        percentage_t fundingRate;
        uint64_t fundingTime;
    };

    struct TickerPrice24hr
    {
        std::string symbol;
        currency_t priceChange;
        percentage_t priceChangePercent;
        currency_t weightedAvgPrice;
        currency_t lastPrice;
        quantity_t lastQty;
        currency_t openPrice;
        currency_t highPrice;
        currency_t lowPrice;
        quantity_t volume;
        quantity_t quoteVolume;
        uint64_t openTime;
        uint64_t closeTime;
        int64_t firstId;
        int64_t lastId;
        int64_t count;
    };

    struct FuturesSymbolPriceTicker
    {
        std::string symbol;
        currency_t price;
        uint64_t time;
    };

    struct FuturesOrderBookTicker
    {
        std::string symbol;
        currency_t bidPrice;
        quantity_t bidQty;
        currency_t askPrice;
        quantity_t askQty;
        uint64_t time;
    };

    struct OpenInterest
    {
        currency_t openInterest;
        std::string symbol;
        uint64_t time;
    };

    struct OpenInterestStatistics
    {
        std::string symbol;
        currency_t sumOpenInterest;
        currency_t sumOpenInterestValue;
        uint64_t timestamp;
    };

    typedef struct TopTraderLongShortRatio
    {
        std::string symbol;
        currency_t longShortRatio;
        currency_t longAccount;
        currency_t shortAccount;
        uint64_t timestamp;
    } TopTraderLongShortRatioAccounts, TopTraderLongShortRatioPositions;

    struct FuturesTakerBuySellVolume
    {
        percentage_t buySellRatio;
        quantity_t buyVol;
        quantity_t sellVol;
        uint64_t timestamp;
    };

    struct CompositeBaseAsset
    {
        std::string baseAsset;
        std::string quoteAsset;
        quantity_t weightInQuantity;
        percentage_t weightInPercentage;
    };

    struct CompositeIndexSymbolInformation
    {
        std::string symbol;
        uint64_t time;
        std::string component;
        std::vector<CompositeBaseAsset> baseAssetList;
    };

    struct InitialLeverage
    {
        std::string symbol;
        currency_t maxNotionalValue;
        int64_t leverage;
    };

    struct FuturesAccountBalance
    {
        std::string accountAlias;
        std::string assetName;
        currency_t walletBalance;
        currency_t crossedWalletBalance;
        percentage_t crossUnPnL;
        currency_t availableBalance;
        currency_t maxWidrawAmount;
        bool marginAvailable;
        uint64_t updateTime;
    };

    struct FuturesAccountInformationAsset
    {
        std::string asset;
        currency_t walletBalance;
        currency_t unrealizedProfit;
        currency_t marginBalance;
        currency_t maintMargin;
        currency_t initialMargin;
        currency_t positionInitialMargin;
        currency_t openOrderInitialMargin;
        currency_t crossWalletBalance;
        currency_t crossUnPnl;
        currency_t availableBalance;
        currency_t maxWithdrawAmount;
        bool marginAvailable;
        uint64_t updateTime;
    };

    struct FuturesAccountInformationPosition
    {
        std::string symbol;
        currency_t initialMargin;
        currency_t maintMargin;
        currency_t unrealizedProfit;
        currency_t positionInitialMargin;
        currency_t openOrderInitialMargin;
        currency_t leverage;
        bool isolated;
        currency_t entryPrice;
        currency_t maxNotional;
        std::string positionSide;
        currency_t positionAmount;
        currency_t notional;
        currency_t isolatedWallet;
        uint64_t updateTime;
    };

    struct FuturesAccountInformation
    {
        int64_t feeTier;
        bool canTrade;
        bool canDeposit;
        bool canWithdraw;
        uint64_t updateTime;
        currency_t totalInitialMargin;
        currency_t totalMaintMargin;
        currency_t totalWalletBalance;
        currency_t totalUnrealizedProfit;
        currency_t totalMarginBalance;
        currency_t totalPositionInitialMargin;
        currency_t totalOpenOrderInitialMargin;
        currency_t totalCrossWalletBalance;
        currency_t totalCrossUnPnl;
        currency_t availableBalance;
        currency_t maxWithdrawAmount;

        std::unordered_map<std::string, FuturesAccountInformationAsset> assets;
        std::unordered_map<std::string, FuturesAccountInformationPosition> positions;
    };

    struct PositionMarginChange
    {
        quantity_t amount;
        std::string asset;
        std::string symbol;
        uint64_t time;
        PositionMargin type;
        std::string positionSide;
    };

    struct PositionInformation
    {
        currency_t entryPrice;
        MarginType marginType;
        bool isAutoAddMargin;
        currency_t isolatedMargin;
        currency_t leverage;
        currency_t liquidationPrice;
        currency_t markPrice;
        currency_t maxNotionalVal;
        currency_t positionAmt;
        std::string symbol;
        currency_t unrealizedProfit;
        Futures::PositionSide positionSide;
        uint64_t updateTime;
    };

    struct AccountTradeList
    {
        bool buyer;
        currency_t commission;
        std::string commissionAsset;
        int64_t id;
        bool maker;
        int64_t orderId;
        currency_t price;
        quantity_t qty;
        quantity_t quoteQty;
        currency_t realizePnL;
        OrderSide side;
        Futures::PositionSide posSide;
        std::string symbol;
        uint64_t updateTime;
    };

    struct OrderAcknowledge
    {
        std::string symbol;
        u_order_t clientOrderId;
        bool reduceOnly;
        bool closePosition;
        OrderSide side;
        Futures::PositionSide position;
        uint64_t orderId;
    };

    struct StreamMarkPrice
    {
        currency_t markPrice;
        currency_t indexPrice;
        currency_t estimSettlePrice;
        currency_t fundingRate;
        uint64_t nextFundingTime;
    };

    struct StreamMarkPriceAllMarket
    {
        std::string symbol;
        currency_t markPrice;
        currency_t indexPrice;
        currency_t estimSettlePrice;
        currency_t fundingRate;
        uint64_t nextFundingTime;
        uint64_t eventTime;
    };

    struct StreamAggregateTrade
    {
        int64_t aggregateTradeId;
        currency_t price;
        quantity_t qty;
        int64_t firstTradeId;
        int64_t lastTradeId;
        uint64_t tradeTime;
        bool isMaker;
    };

    typedef struct MiniTickers
    {
        currency_t closePrice;
        currency_t openPrice;
        currency_t highPrice;
        currency_t lowPrice;
        currency_t totalTradedBaseAssetVolume;
        currency_t totalTradedQuoteAssetVolume;
    } StreamIndividualSymbolMiniTicker, StreamMarketMiniTickers;

    typedef struct SymbolTicker
    {
        currency_t priceChange;
        currency_t priceChangePercent;
        currency_t weightedAveratePrice;
        currency_t lastPrice;
        currency_t lastPriceQuantity;
        currency_t openPrice;
        currency_t highPrice;
        currency_t lowPrice;
        currency_t totalTradedBaseAssetVolume;
        currency_t totalTradedQuoteAssetVolume;
        int64_t statisticsOpenTime;
        int64_t statisticsCloseTime;
        int64_t firstTreadeId;
        int64_t lastTradeId;
        uint64_t totalNumberOfTrades;
    } StreamIndividualSymbolTicker, StreamMarketTickers;

    struct StreamBookTicker
    {
        uint64_t transactionTime;
        currency_t bestBidPice;
        currency_t bestBidQty;
        currency_t bestAskPice;
        currency_t bestAskQty;
    };

    struct StreamLiquidationOrder
    {
        OrderSide side;
        OrderType type;
        OrderTimeInForce tif;
        quantity_t originalQty;
        currency_t price;
        currency_t averagePrice;
        OrderStatus status;
        quantity_t orderLastFilledQty;
        quantity_t orderFilledAccumulatedQty;
        uint64_t tradeTime;
    };

    struct StreamDepthUpdate
    {
        uint64_t transactionTime; // Transaction Time is not available on Spot
        uint64_t firstUpdateId;
        uint64_t finalUpdateId;
        uint64_t finalUpdateIdLastStream;        // Final Update Id is not available on Spot
        std::map<std::string, std::string> bids; // FIRST: price level; SECOND: quantity
        std::map<std::string, std::string> asks; // FIRST: price level; SECOND: quantity
    };

    struct StreamBLVTInfo
    {
        double tokenIssued;
        std::map<std::string, double> baskets; // FIRST: futures symbol; SECOND: position
        double BLVTNav;
        double realLeverage;
        double targetLeverage;
        double fundingRatio;
    };

    struct StreamCompositionIndex
    {
        std::string baseAsset;
        std::string quoteAsset;
        currency_t weightInQuantity;
        currency_t weightInPercentage;
        currency_t indexPrice;
    };

    struct UserStreamMarginCallPosition
    {
        Futures::PositionSide side;
        currency_t positionAmount;
        currency_t markPrice;
        currency_t isolatedWallet;
        currency_t unrealizedPnL;
        currency_t maintenanceMarginRequired;
        MarginType marginType;
    };

    struct UserStreamMarginCallFutures
    {
        currency_t crossMarginWallet { -1. }; // if not present on the stream crossMarginWallet = -1
        std::map<std::string, UserStreamMarginCallPosition> positions;
    };

    struct UserStreamOrderUpdate
    {
        u_order_t clientOrderId; // if isADLAutoClose == true; clientOrderId will be empty;
        bool isLiquidation;      // See BinanceAPI documentation. if isLiquidation == true; clientOrderId will not have the 'autoclose-' prefix
        bool isADLAutoClose;     // See BinanceAPI documentation
        OrderSide side;
        StreamOrderTypeFutures type;
        OrderTimeInForce tif;
        currency_t originalQty;
        currency_t originalPrice;
        currency_t averagePrice;
        currency_t stopPrice; // Ignored when type==trailingStopMarket
        StreamOrderExecutionTypeFutures execType;
        StreamOrderStatusFutures status;
        uint64_t orderId;
        currency_t lastFilledQty;
        currency_t filledAccumulatedQty;
        currency_t lastFilledPrice;
        currency_t commissionAmmount { -1 }; // if no commission -1
        asset_t commissionAsset;             // if no commission will be empty
        uint64_t tradeTime;
        int64_t tradeId;
        currency_t bidsNotional;
        currency_t asksNotional;
        bool isMaker;
        bool isReduceOnly;
        WorkingType workType;
        OrderType originalOrderType;
        Futures::PositionSide posSide;
        bool closeAll; // if not conditional order the value is false
        currency_t activationPrice { -1 };
        currency_t callbackRate;
        currency_t realizedProfit;
    };

    struct FuturesQueryOrder
    {
        sym_t symbol;
        u_order_t clientOrderId;
        currency_t averagePrice;
        quantity_t cumulativeQuote;
        quantity_t executedQty;
        quantity_t originalQty;
        currency_t price;
        currency_t stopPrice;
        currency_t priceRate;
        currency_t activatePrice;
        uint64_t orderId;
        uint64_t updateTime;
        uint64_t orderTime;
        OrderType originalType;
        OrderType type;
        OrderSide side;
        Futures::PositionSide positionSide;
        WorkingType workingType;
        OrderTimeInForce otif;
        OrderStatus orderStatus;
        bool reduceOnly;
        bool priceProtect;
        bool closePosition;
    };

} // namespace BINAPI_NAMESPACE

#endif /*BINDEFS_HPP_*/
