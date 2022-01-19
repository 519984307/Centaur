////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// bindefs.hpp

/**
 * @file bindefs.hpp
 * @brief description
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

#ifndef BINDEFS_HPP_
#define BINDEFS_HPP_

#include <bitset>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace trader::api
{
    // Use to define a currency
    using currency_t = double;
    // Use to define a Symbol. ex. BTCUSDT
    using symbol_t = std::string;
    // Use to define an asset. ex. BTC
    using asset_t = std::string;
    // Use to manage percentages
    using percentage_t = double;
    // Use to manage an asset quantity
    using quantity_t = double;

    enum class TradingStatus : int
    {
        Trading,
        Break
    };

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

    /// \brief Bits on AccountInformation::permissions
    enum AccountPermissions
    {
        SPOT = 0,
        MARGIN,
        LEVERAGED
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
        addPositionMargin = 1,
        reducePositionMargin = 2
    };

    enum class MarginType
    {
        crossed,
        isolated
    };

    enum class PositionSide
    {
        BOTH, // one-way
        LONG, // hedge-mode
        SHORT // hedge-mode
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
        uint64_t openTime{0};
        double open{0};
        double high{0};
        double low{0};
        double close{0};
        double volume{0}; /// If the structure is generated from a stream. This value is  equal to takerBaseAssetVolume
        uint64_t closeTime{0};
        double quoteAssetVolume{0};
        double baseAssetVolume{0};
        uint64_t numberOfTrades{0};
        double takerBaseAssetVolume{0};
        double takerQuoteAssetVolume{0};
        bool isClosed{true};     /// Stream field otherwise always true
        int64_t firstTradeId{0}; /// Stream field otherwise always 0
        int64_t lastTradeId{0};  /// Stream field otherwise always 0
    };

    struct EIRateLimits
    {
        uint32_t seconds{0};
        uint32_t limit{0};
    };

    namespace EIFilters
    {
        struct PriceFilter
        {
            bool present{false};
            currency_t minPrice{0};
            currency_t maxPrice{0};
            quantity_t tickSize{0};
        };

        struct PercentPrice
        {
            bool present{false};
            currency_t multiplierUp{0};
            currency_t multiplierDown{0};
            int averagePriceMins{0};         // Not used in futures
            currency_t multiplierDecimal{0}; // Used in futures
        };

        struct LotSize
        {
            bool present{false};
            quantity_t minQty{0};
            quantity_t maxQty{0};
            quantity_t stepSize{0};
        };

        struct MinNotional
        {
            bool present{false};
            currency_t minNotional{0};
            int averagePriceMins{0};   // Not used in futures
            bool applyToMarket{false}; // Not used in futures
        };

        struct IcebergParts
        {
            bool present{false};
            int limit{0};
        };

        struct MarketLotSize
        {
            bool present{false};
            quantity_t minQty{0};
            quantity_t maxQty{0};
            quantity_t stepSize{0};
        };

        struct MaxNumOrders
        {
            bool present{false};
            int maxNumOrders{0};
        };

        struct MaxNumAlgoOrders
        {
            bool present{false};
            int maxNumAlgoOrders{0};
        };

        struct MaxNumIcerbergOrders
        {
            bool present{false};
            int maxNumIcebergOrders{0};
        };

        struct MaxPosition
        {
            bool present{false};
            quantity_t maxPosition{0};
        };
    } // namespace EIFilters

    struct ExchangeInformationSymbol
    {
        symbol_t symbol;
        asset_t baseAsset;
        asset_t quoteAsset;
        int dbSymbolId{-1};
        TradingStatus status;
        int baseAssetPrecision{0};
        int quotePrecision{0};
        int quoteAssetPrecision{0};
        int baseCommissionPrecision{0};
        int quoteCommissionPrecision{0};
        std::bitset<7> orderType;
        std::bitset<3> permissions;
        bool icebergAllowed{false};
        bool ocoAllowed{false};
        bool quoteOrderQtyMarketAllowed{false};
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
    };

    struct ExchangeInformation
    {
        std::string timezone;
        EIRateLimits limitWeight;
        EIRateLimits limitOrderSecond;
        EIRateLimits limitOrderDay;

        std::unordered_map<std::string, ExchangeInformationSymbol> symbols;
    };

    struct FutureAssets
    {
        std::string asset;
        bool marginAvailable;
        double autoAssetExchange;
    };

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

        std::unordered_map<std::string, FutureAssets> assets;

        std::unordered_map<std::string, FuturesExchangeInformationSymbol> symbols;
    };

    struct AssetBalance
    {
        quantity_t free{0};
        quantity_t locked{0};
    };

    struct AccountInformation
    {
        std::string accountType;
        uint64_t makerCommission{0};
        uint64_t takerCommission{0};
        uint64_t buyerCommission{0};
        uint64_t sellerCommission{0};
        bool canTrade{false};
        bool canWithdraw{false};
        bool canDeposit{false};
        std::unordered_map<symbol_t, AssetBalance> balances;
        std::bitset<3> permissions;
    };

    struct OrderbookEntry
    {
        std::string price;
        std::string quantity;
    };

    struct Orderbook
    {
        uint64_t lastUpdateId{0};
        std::vector<OrderbookEntry> asks;
        std::vector<OrderbookEntry> bids;
    };

    struct FuturesOrderBook
    {
        uint64_t lastTimeUpdate{0};
        uint64_t messageOutputTime;
        uint64_t transactionTime;
        std::vector<OrderbookEntry> asks;
        std::vector<OrderbookEntry> bids;
    };

    struct UserStreamOrder
    {
        uint64_t eventTime;
        symbol_t symbol;
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
        currency_t poisitionAmount;
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
        PositionSide positionSide;
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
        PositionSide posSide;
        std::string symbol;
        uint64_t updateTime;
    };

    struct OrderAcknowledge
    {
        std::string symbol;
        std::string clientOrderId;
        bool reduceOnly;
        bool closePosition;
        OrderSide side;
        PositionSide position;
        uint64_t orderId;
    };

    struct FuturesCancelOrder
    {
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
        PositionSide side;
        currency_t positionAmount;
        currency_t markPrice;
        currency_t isolatedWallet;
        currency_t unrealizedPnL;
        currency_t maintenanceMarginRequired;
        MarginType marginType;
    };

    struct UserStreamMarginCallFutures
    {
        currency_t crossMarginWallet{-1.}; // if not present on the stream crossMarginWallet = -1
        std::map<std::string, UserStreamMarginCallPosition> positions;
    };

    struct UserStreamOrderUpdate
    {
        std::string clientOrderId; // if isADLAutoClose == true; clientOrderId will be empty;
        bool isLiquidation;        // See BinanceAPI documentation. if isLiquidation == true; clientOrderId will not have the 'autoclose-' prefix
        bool isADLAutoClose;       // See BinanceAPI documentation
        OrderSide side;
        StreamOrderTypeFutures type;
        OrderTimeInForce tif;
        currency_t originalQty;
        currency_t originalPrice;
        currency_t averagePrice;
        currency_t stopPrice; // Ignored when type==trailingStopMarket
        StreamOrderExecutionTypeFutures execType;
        StreamOrderStatusFutures status;
        int64_t orderId;
        currency_t lastFilledQty;
        currency_t filledAccumulatedQty;
        currency_t lastFilledPrice;
        currency_t commissionAmmount{-1}; // if no commission -1
        std::string commissionAsset;         // if no commission will be empty
        uint64_t tradeTime;
        int64_t tradeId;
        currency_t bidsNotional;
        currency_t asksNotional;
        bool isMaker;
        bool isReduceOnly;
        WorkingType workType;
        OrderType originalOrderType;
        PositionSide posSide;
        bool closeAll; // if not conditional order the value is false
        currency_t activationPrice{-1};
        currency_t callbackRate;
        currency_t realizedProfit;
    };

} // namespace trader::api

#endif /*BINDEFS_HPP_*/
