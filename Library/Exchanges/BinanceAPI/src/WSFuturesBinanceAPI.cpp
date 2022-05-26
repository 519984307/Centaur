////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

//  WSFuturesBinanceAPI.cpp

#include "WSFuturesBinanceAPI.hpp"

#include "Binapi.hpp"
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

#include <fmt/color.h>
#include <fmt/core.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

#define SUBSCRIBE_METHOD(stream)              \
    if (!isRunning())                         \
        return { stream };                    \
    else                                      \
    {                                         \
        int id = randNumber();                \
        while (true)                          \
        {                                     \
            if (!m_subscribeIds.contains(id)) \
            {                                 \
                m_subscribeIds.insert(id);    \
                break;                        \
            }                                 \
        }                                     \
        subscribe(std::string(stream), id);   \
        return { id };                        \
    }

#define UNSUBSCRIBE_METHOD(stream)              \
    if (isRunning())                            \
    {                                           \
        int id = randNumber();                  \
        while (true)                            \
        {                                       \
            if (!m_unsubscribeIds.contains(id)) \
            {                                   \
                m_unsubscribeIds.insert(id);    \
                break;                          \
            }                                   \
        }                                       \
        unsubscribe(std::string(stream), id);   \
        return id;                              \
    }                                           \
    return -1;

#define JTO_STRING(x, y) \
    std::string { x[y].GetString(), x[y].GetStringLength() }

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

namespace glstr
{
    constexpr std::string_view g_1m { "1m" };
    constexpr std::string_view g_3m { "3m" };
    constexpr std::string_view g_5m { "5m" };
    constexpr std::string_view g_15m { "15m" };
    constexpr std::string_view g_30m { "30m" };
    constexpr std::string_view g_1h { "1h" };
    constexpr std::string_view g_2h { "2h" };
    constexpr std::string_view g_4h { "4h" };
    constexpr std::string_view g_6h { "6h" };
    constexpr std::string_view g_8h { "8h" };
    constexpr std::string_view g_12h { "12h" };
    constexpr std::string_view g_1d { "1d" };
    constexpr std::string_view g_3d { "3d" };
    constexpr std::string_view g_1w { "1w" };
    constexpr std::string_view g_1M { "1M" };

    constexpr std::string_view g_aggTrade { "aggTrade" };
    constexpr std::string_view g_markPriceUpdate { "markPriceUpdate" };
    constexpr std::string_view g_kline { "kline" };
    constexpr std::string_view g_continuous_kline { "continuous_kline" };
    constexpr std::string_view g_24hrMiniTicker { "24hrMiniTicker" };
    constexpr std::string_view g_24hrTicker { "24hrTicker" };
    constexpr std::string_view g_bookTicker { "bookTicke" };
    constexpr std::string_view g_forceOrder { "forceOrder" };
    constexpr std::string_view g_depthUpdate { "depthUpdate" };
    constexpr std::string_view g_nav { "nav" };
    constexpr std::string_view g_compositeIndex { "compositeIndex" };

    constexpr std::string_view g_PERPETUAL { "PERPETUAL" };
    constexpr std::string_view g_CURRENT_QUARTER { "CURRENT_QUARTER" };
    constexpr std::string_view g_NEXT_QUARTER { "NEXT_QUARTER" };

} // namespace glstr

#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace
{

    std::string symbolToLower(const std::string &symbol)
    {
        std::string str = symbol;
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    std::string symbolToUpper(const std::string &symbol)
    {
        std::string str = symbol;
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }
} // namespace

BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::WSFuturesBinanceAPI(std::string endPoint) :
    BINAPI_NAMESPACE::ws::WSThread("fstream.binance.com", std::move(endPoint))
{
}

void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribe(const std::string &stream, const int &id)
{
    std::string subscribe = fmt::format(R"({{"method": "SUBSCRIBE","params":["{}"],"id":{}}})", stream, id);
    sendData(subscribe);
}

void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::unsubscribe(const std::string &stream, const int &id)
{
    std::string unsubscribe = fmt::format(R"({{"method": "UNSUBSCRIBE","params":["{}"],"id":{}}})", stream, id);
    sendData(unsubscribe);
}

void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribe([[maybe_unused]] const bool &result, [[maybe_unused]] const int &id)
{
}

void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::unsubscribe([[maybe_unused]] const bool &result, [[maybe_unused]] const int &id)
{
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeMarkPrice(const std::string &symbol, const int &update)
{
    assert(update == 1000 || update == 3000);
    const int sec = [&update]() {
        switch (update)
        {
            case 1000:
                return 1;
            case 3000:
                [[fallthrough]];
            default:
                return 3;
        }
    }();
    std::string stream = fmt::format("{}@markPrice@{}s", symbolToLower(symbol), sec);
    SUBSCRIBE_METHOD(stream)
}
std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeMarkPriceAllMarket(const int &update)
{
    assert(update == 1000 || update == 3000);
    const int sec = [&update]() {
        switch (update)
        {
            case 1000:
                return 1;
            case 3000:
                [[fallthrough]];
            default:
                return 3;
        }
    }();
    std::string stream = fmt::format("!markPrice@{}s", sec);
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeAggregateTrade(const std::string &symbol)
{
    std::string stream = fmt::format("{}@aggTrade", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeKline(const std::string &symbol, const BinanceTimeIntervals &interval)
{
    std::string stream = fmt::format("{}@kline_{}", symbolToLower(symbol), BINAPI_NAMESPACE::BinanceAPI::fromIntervalToString(interval));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeContinuousContractKline(const std::string &symbol, const ContractType &ctype, const BinanceTimeIntervals &interval)
{
    const std::string cotc = [&ctype]() -> std::string {
        switch (ctype)
        {
            case ContractType::none:
                [[fallthrough]];
            case ContractType::PERPETUAL:
                return { "perpetual" };
            case ContractType::CURRENT_QUARTER:
                return { "current_quarter" };
            case ContractType::NEXT_QUARTER:
                return { "next_quarter" };
        }
    }();

    std::string stream = fmt::format("{}_{}@continuousKline_{}", symbolToLower(symbol), cotc, BinanceAPI::fromIntervalToString(interval));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeIndividualMiniTicker(const std::string &symbol)
{
    std::string stream = fmt::format("{}@miniTicker", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream)
}

int BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::unsubscribeIndividualMiniTicker(const std::string &symbol)
{
    std::string stream = fmt::format("{}@miniTicker", symbolToLower(symbol));
    UNSUBSCRIBE_METHOD(stream);
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeAllMarketMiniTickers() {
    SUBSCRIBE_METHOD("!miniTicker@arr")
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeIndividualSymbolTicker(const std::string &symbol)
{
    std::string stream = fmt::format("{}@ticker", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeAllMarketTickers() {
    SUBSCRIBE_METHOD("!ticker@arr")
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeIndividualSymbolBookTickerStreams(const std::string &symbol)
{
    std::string stream = fmt::format("{}@bookTicker", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeAllBookTickerStream() {
    SUBSCRIBE_METHOD("!bookTicker")
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeLiquidationOrder(const std::string &symbol)
{
    std::string stream = fmt::format("{}@forceOrder", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeAllMarketLiquidationOrderStream() {
    SUBSCRIBE_METHOD("!forceOrder@arr")
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribePartialBookDepth(const std::string &symbol, const int &levels, const int &update)
{
    assert(levels == 5 || levels == 10 || levels == 20);
    assert(update == 250 || update == 500 || update == 100);
    std::string stream = fmt::format("{}@depth{}@{}ms", symbolToLower(symbol), levels, update);
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeDiffBookDepth(const std::string &symbol, const int &update)
{
    assert(update == 250 || update == 500 || update == 100);
    std::string stream = fmt::format("{}@depth@{}ms", symbolToLower(symbol), update);
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeBLVTInfo(const std::string &tokenName)
{
    std::string stream = fmt::format("{}@tokenNav", symbolToUpper(tokenName));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeBLVTNavKline(const std::string &tokenName, const BinanceTimeIntervals &interval)
{
    std::string stream = fmt::format("{}@nav_Kline_{}", symbolToUpper(tokenName), BinanceAPI::fromIntervalToString(interval));
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::subscribeCompositeIndexSymbolInformation(const std::string &symbol)
{
    std::string stream = fmt::format("{}@compositeIndex", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream)
}

void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::receivedData()
{
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(getJsonData().c_str());
    if (jsonDoc.HasParseError())
    {
        fmt::print(fmt::fg(fmt::color::red), "STREAM ERROR: ");
        fmt::print("JSON data is corrupted\n");
        return;
    }

    try
    {
        // Test for (un)subscription messages
        if (jsonDoc.FindMember("result") != jsonDoc.MemberEnd())
        {

            bool result = jsonDoc["result"].IsNull();

            int id      = jsonDoc["id"].GetInt();
            // Find the Id
            auto siid = m_subscribeIds.find(id);
            if (siid != m_subscribeIds.end())
            {
                m_subscribeIds.erase(siid);
                subscribe(result, id);
            }
            else
            {
                auto uiid = m_unsubscribeIds.find(id);
                if (uiid != m_unsubscribeIds.end())
                {
                    unsubscribe(result, id);
                    m_unsubscribeIds.erase(uiid);
                }
            }

            return;
        }

        const rapidjson::Value &value = jsonDoc.FindMember("data") != jsonDoc.MemberEnd() ? jsonDoc["data"].GetObject() : jsonDoc.GetObject();

        const std::string_view type { value["e"].GetString(), value["e"].GetStringLength() };

        if (!value.IsArray())
        {
            if (type == glstr::g_aggTrade)
            {
                StreamAggregateTrade sat {
                    .aggregateTradeId = value["a"].GetInt64(),
                    .price            = std::stod(JTO_STRING(value, "p")),
                    .qty              = std::stod(JTO_STRING(value, "q")),
                    .firstTradeId     = value["f"].GetInt64(),
                    .lastTradeId      = value["l"].GetInt64(),
                    .tradeTime        = value["T"].GetUint64(),
                    .isMaker          = value["m"].GetBool()
                };
                aggregateTradeStream(JTO_STRING(value, "s"), value["E"].GetUint64(), sat);
            }
            else if (type == glstr::g_markPriceUpdate)
            {

                StreamMarkPrice smp {
                    .markPrice        = std::stod(JTO_STRING(value, "p")),
                    .indexPrice       = std::stod(JTO_STRING(value, "i")),
                    .estimSettlePrice = std::stod(JTO_STRING(value, "P")),
                    .fundingRate      = std::stod(JTO_STRING(value, "r")),
                    .nextFundingTime  = value["T"].GetUint64(),
                };
                markPriceStream(JTO_STRING(value, "s"), value["E"].GetUint64(), smp);
            }
            else if (type == glstr::g_kline)
            {
                const auto &cd           = value["k"].GetObject();
                BinanceTimeIntervals bti = BinanceTimeIntervals::i1M;

                const std::string_view interval { cd["i"].GetString(), cd["i"].GetStringLength() };

                if (interval == glstr::g_1m)
                    bti = BinanceTimeIntervals::i1m;
                else if (interval == glstr::g_3m)
                    bti = BinanceTimeIntervals::i3m;
                else if (interval == glstr::g_5m)
                    bti = BinanceTimeIntervals::i5m;
                else if (interval == glstr::g_15m)
                    bti = BinanceTimeIntervals::i15m;
                else if (interval == glstr::g_30m)
                    bti = BinanceTimeIntervals::i30m;
                else if (interval == glstr::g_1h)
                    bti = BinanceTimeIntervals::i1h;
                else if (interval == glstr::g_2h)
                    bti = BinanceTimeIntervals::i2h;
                else if (interval == glstr::g_4h)
                    bti = BinanceTimeIntervals::i4h;
                else if (interval == glstr::g_6h)
                    bti = BinanceTimeIntervals::i6h;
                else if (interval == glstr::g_8h)
                    bti = BinanceTimeIntervals::i8h;
                else if (interval == glstr::g_12h)
                    bti = BinanceTimeIntervals::i12h;
                else if (interval == glstr::g_1d)
                    bti = BinanceTimeIntervals::i1d;
                else if (interval == glstr::g_3d)
                    bti = BinanceTimeIntervals::i3d;
                else if (interval == glstr::g_1w)
                    bti = BinanceTimeIntervals::i1w;
                else if (interval == glstr::g_1M)
                    bti = BinanceTimeIntervals::i1M;

                Candlestick cs {
                    .openTime              = cd["t"].GetUint64(),
                    .open                  = std::stod(JTO_STRING(cd, "o")),
                    .high                  = std::stod(JTO_STRING(cd, "h")),
                    .low                   = std::stod(JTO_STRING(cd, "l")),
                    .close                 = std::stod(JTO_STRING(cd, "c")),
                    .volume                = std::stod(JTO_STRING(cd, "v")),
                    .closeTime             = cd["T"].GetUint64(),
                    .quoteAssetVolume      = std::stod(JTO_STRING(cd, "q")),
                    .baseAssetVolume       = std::stod(JTO_STRING(cd, "v")),
                    .numberOfTrades        = cd["n"].GetUint64(),
                    .takerBaseAssetVolume  = std::stod(JTO_STRING(cd, "V")),
                    .takerQuoteAssetVolume = std::stod(JTO_STRING(cd, "Q")),
                    .isClosed              = cd["x"].GetBool(), /// Stream field otherwise always true
                    .firstTradeId          = cd["f"].GetInt64(),
                    .lastTradeId           = cd["L"].GetInt64()
                };
                kline( {value["s"].GetString(), value["s"].GetStringLength()}, value["E"].GetUint64(), bti, cs);
            }
            else if (type == glstr::g_continuous_kline)
            {
                const auto &cd           = value["k"].GetObject();
                BinanceTimeIntervals bti = BinanceTimeIntervals::i1M;
                const std::string_view interval { cd["i"].GetString(), cd["i"].GetStringLength() };

                if (interval == glstr::g_1m)
                    bti = BinanceTimeIntervals::i1m;
                else if (interval == glstr::g_3m)
                    bti = BinanceTimeIntervals::i3m;
                else if (interval == glstr::g_5m)
                    bti = BinanceTimeIntervals::i5m;
                else if (interval == glstr::g_15m)
                    bti = BinanceTimeIntervals::i15m;
                else if (interval == glstr::g_30m)
                    bti = BinanceTimeIntervals::i30m;
                else if (interval == glstr::g_1h)
                    bti = BinanceTimeIntervals::i1h;
                else if (interval == glstr::g_2h)
                    bti = BinanceTimeIntervals::i2h;
                else if (interval == glstr::g_4h)
                    bti = BinanceTimeIntervals::i4h;
                else if (interval == glstr::g_6h)
                    bti = BinanceTimeIntervals::i6h;
                else if (interval == glstr::g_8h)
                    bti = BinanceTimeIntervals::i8h;
                else if (interval == glstr::g_12h)
                    bti = BinanceTimeIntervals::i12h;
                else if (interval == glstr::g_1d)
                    bti = BinanceTimeIntervals::i1d;
                else if (interval == glstr::g_3d)
                    bti = BinanceTimeIntervals::i3d;
                else if (interval == glstr::g_1w)
                    bti = BinanceTimeIntervals::i1w;
                else if (interval == glstr::g_1M)
                    bti = BinanceTimeIntervals::i1M;

                ContractType ct = ContractType::none;
                const std::string_view cts { value["ct"].GetString(), value["ct"].GetStringLength() };

                if (cts == glstr::g_PERPETUAL)
                    ct = ContractType::PERPETUAL;
                else if (cts == glstr::g_CURRENT_QUARTER)
                    ct = ContractType::CURRENT_QUARTER;
                else if (cts == glstr::g_NEXT_QUARTER)
                    ct = ContractType::NEXT_QUARTER;

                Candlestick cs {
                    .openTime              = cd["t"].GetUint64(),
                    .open                  = std::stod(JTO_STRING(cd, "o")),
                    .high                  = std::stod(JTO_STRING(cd, "h")),
                    .low                   = std::stod(JTO_STRING(cd, "l")),
                    .close                 = std::stod(JTO_STRING(cd, "c")),
                    .volume                = std::stod(JTO_STRING(cd, "v")),
                    .closeTime             = cd["T"].GetUint64(),
                    .quoteAssetVolume      = std::stod(JTO_STRING(cd, "q")),
                    .baseAssetVolume       = std::stod(JTO_STRING(cd, "v")),
                    .numberOfTrades        = cd["n"].GetUint64(),
                    .takerBaseAssetVolume  = std::stod(JTO_STRING(cd, "V")),
                    .takerQuoteAssetVolume = std::stod(JTO_STRING(cd, "Q")),
                    .isClosed              = cd["x"].GetBool(), /// Stream field otherwise always true
                    .firstTradeId          = cd["f"].GetInt64(),
                    .lastTradeId           = cd["L"].GetInt64()
                };
                continuousKline( {value["ps"].GetString(), value["ps"].GetStringLength()}, value["E"].GetUint64(), ct, bti, cs);
            }

            else if (type == glstr::g_24hrMiniTicker)
            {
                StreamIndividualSymbolMiniTicker smmt {
                    .closePrice                  = std::stod(JTO_STRING(value, "c")),
                    .openPrice                   = std::stod(JTO_STRING(value, "o")),
                    .highPrice                   = std::stod(JTO_STRING(value, "h")),
                    .lowPrice                    = std::stod(JTO_STRING(value, "l")),
                    .totalTradedBaseAssetVolume  = std::stod(JTO_STRING(value, "v")),
                    .totalTradedQuoteAssetVolume = std::stod(JTO_STRING(value, "q"))
                };
                individualSymbolMiniTicker(JTO_STRING(value, "s"), value["E"].GetUint64(), smmt);
            }
            else if (type == glstr::g_24hrTicker)
            {
                StreamIndividualSymbolTicker sst {
                    .priceChange                 = std::stod(JTO_STRING(value, "p")),
                    .priceChangePercent          = std::stod(JTO_STRING(value, "P")),
                    .weightedAveratePrice        = std::stod(JTO_STRING(value, "w")),
                    .lastPrice                   = std::stod(JTO_STRING(value, "c")),
                    .lastPriceQuantity           = std::stod(JTO_STRING(value, "Q")),
                    .openPrice                   = std::stod(JTO_STRING(value, "o")),
                    .highPrice                   = std::stod(JTO_STRING(value, "h")),
                    .lowPrice                    = std::stod(JTO_STRING(value, "l")),
                    .totalTradedBaseAssetVolume  = std::stod(JTO_STRING(value, "v")),
                    .totalTradedQuoteAssetVolume = std::stod(JTO_STRING(value, "q")),
                    .statisticsOpenTime          = value["O"].GetInt64(),
                    .statisticsCloseTime         = value["C"].GetInt64(),
                    .firstTreadeId               = value["F"].GetInt64(),
                    .lastTradeId                 = value["L"].GetInt64(),
                    .totalNumberOfTrades         = value["n"].GetUint64()
                };
                individualSymbolTicker(JTO_STRING(value, "s"), value["E"].GetUint64(), sst);
            }
            else if (type == glstr::g_bookTicker)
            {
                StreamBookTicker sbt {
                    .transactionTime = value["T"].GetUint64(),
                    .bestBidPice     = std::stod(JTO_STRING(value, "b")),
                    .bestBidQty      = std::stod(JTO_STRING(value, "B")),
                    .bestAskPice     = std::stod(JTO_STRING(value, "a")),
                    .bestAskQty      = std::stod(JTO_STRING(value, "A")),
                };
                bookTicker(JTO_STRING(value, "s"), value["E"].GetUint64(), sbt);
            }
            else if (type == glstr::g_forceOrder)
            {
                const auto &fod           = value["o"].GetObject();

                const std::string sSide   = JTO_STRING(fod, "S");
                const std::string sType   = JTO_STRING(fod, "T");
                const std::string sTIF    = JTO_STRING(fod, "f");
                const std::string sStatus = JTO_STRING(fod, "X");

                StreamLiquidationOrder slo {
                    .side = [&sSide]() {
                        if (sSide == "SELL")
                            return OrderSide::sell;
                        return OrderSide::buy;
                    }(),
                    .type = [&sType]() {
                        if (sType == "LIMIT")
                            return OrderType::limit;
                        else if (sType == "MARKET")
                            return OrderType::market;
                        else if (sType == "STOP_LOSS")
                            return OrderType::stopLoss;
                        else if (sType == "STOP_LOSS_LIMIT")
                            return OrderType::stopLossLimit;
                        else if (sType == "TAKE_PROFIT")
                            return OrderType::takeProfit;
                        else if (sType == "TAKE_PROFIT_LIMIT")
                            return OrderType::takeProfitLimit;
                        else if (sType == "LIMIT_MAKER")
                            return OrderType::limitMaker;
                        else if (sType == "STOP")
                            return OrderType::stop;
                        else if (sType == "STOP_MARKET")
                            return OrderType::stopMarket;
                        else if (sType == "STOP_PROFIT_MARKET")
                            return OrderType::takeProfitMarket;
                        else if (sType == "TRAILING_STOP_MARKET")
                            return OrderType::trailingStopMarket;
                        return OrderType::limit; // SHOULD NOT BE REACHED, unless API changes
                    }(),
                    .tif = [&sTIF]() {
                        if (sTIF == "GTC")
                            return OrderTimeInForce::GTC;
                        else if (sTIF == "IOC")
                            return OrderTimeInForce::IOC;
                        else if (sTIF == "FOK")
                            return OrderTimeInForce::FOK;
                        return OrderTimeInForce::GTC;
                    }(),
                    .originalQty  = std::stod(JTO_STRING(fod, "q")),
                    .price        = std::stod(JTO_STRING(fod, "p")),
                    .averagePrice = std::stod(JTO_STRING(fod, "ap")),
                    .status       = [&sStatus]() {
                        if (sStatus == "NEW")
                            return OrderStatus::newOrder;
                        else if (sStatus == "PARTIALLY_FILLED")
                            return OrderStatus::partiallyFilled;
                        else if (sStatus == "FILLED")
                            return OrderStatus::filled;
                        else if (sStatus == "CANCELED")
                            return OrderStatus::canceled;
                        else if (sStatus == "PENDING_CANCEL")
                            return OrderStatus::pendingCancel;
                        else if (sStatus == "REJECTED")
                            return OrderStatus::rejected;
                        return OrderStatus::expired;
                    }(),
                    .orderLastFilledQty        = std::stod(JTO_STRING(fod, "l")),
                    .orderFilledAccumulatedQty = std::stod(JTO_STRING(fod, "z")),
                    .tradeTime                 = fod["T"].GetUint64()
                };
                liquidationOrder(JTO_STRING(fod, "s"), value["E"].GetUint64(), slo);
            }
            else if (type == glstr::g_depthUpdate)
            {
                StreamDepthUpdate sdp {
                    .transactionTime         = value["T"].GetUint64(),
                    .firstUpdateId           = value["U"].GetUint64(),
                    .finalUpdateId           = value["u"].GetUint64(),
                    .finalUpdateIdLastStream = value["pu"].GetUint64()
                };
                for (const auto &bids : value["b"].GetArray())
                    sdp.bids[JTO_STRING(bids, 0)] = JTO_STRING(bids, 1);
                for (const auto &asks : value["a"].GetArray())
                    sdp.asks[JTO_STRING(asks, 0)] = JTO_STRING(asks, 1);

                depthUpdate(JTO_STRING(value, "s"), value["E"].GetUint64(), sdp);
            }
            else if (type == glstr::g_nav)
            {
                StreamBLVTInfo sbi;
                sbi.tokenIssued    = value["m"].GetDouble();
                sbi.BLVTNav        = value["n"].GetDouble();
                sbi.realLeverage   = value["l"].GetDouble();
                sbi.targetLeverage = value["t"].GetDouble();
                sbi.fundingRatio   = value["f"].GetDouble();
                for (const auto &baskets : value["b"].GetArray())
                    sbi.baskets[JTO_STRING(baskets, "s")] = baskets["n"].GetDouble();
                blvtInfo(JTO_STRING(value, "s"), value["E"].GetUint64(), sbi);
            }
            else if (type == glstr::g_compositeIndex)
            {
                std::vector<StreamCompositionIndex> vsci;
                for (const auto &composition : value["c"].GetArray())
                {
                    StreamCompositionIndex sci {
                        .baseAsset          = JTO_STRING(composition, "b"),
                        .quoteAsset         = JTO_STRING(composition, "q"),
                        .weightInQuantity   = std::stod(JTO_STRING(composition, "")),
                        .weightInPercentage = std::stod(JTO_STRING(composition, "")),
                        .indexPrice         = std::stod(JTO_STRING(composition, ""))
                    };
                    vsci.push_back(sci);
                }
                compositeIndex(JTO_STRING(value, "s"), value["E"].GetUint64(), std::stod(JTO_STRING(value, "p")), vsci);
            }
        }
        else // STREAM IS AN ARRAY
        {
            std::vector<StreamMarkPriceAllMarket> vsmpam;
            std::multimap<std::string, std::pair<uint64_t, StreamMarketMiniTickers>> mmsmmt;
            std::multimap<std::string, std::pair<uint64_t, StreamMarketTickers>> mmsmt;
            for (const auto &streamArray : value.GetArray())
            {
                const std::string stype = JTO_STRING(streamArray, "e");
                if (stype == "markPriceUpdate")
                {
                    StreamMarkPriceAllMarket smpam {
                        .symbol           = JTO_STRING(streamArray, "s"),
                        .markPrice        = std::stod(JTO_STRING(streamArray, "p")),
                        .indexPrice       = std::stod(JTO_STRING(streamArray, "i")),
                        .estimSettlePrice = std::stod(JTO_STRING(streamArray, "P")),
                        .fundingRate      = std::stod(JTO_STRING(streamArray, "r")),
                        .nextFundingTime  = streamArray["T"].GetUint64(),
                        .eventTime        = streamArray["E"].GetUint64()
                    };
                    vsmpam.push_back(smpam);
                }
                else if (stype == "24hrMiniTicker")
                {
                    StreamMarketMiniTickers smmt {
                        .closePrice                  = std::stod(JTO_STRING(streamArray, "c")),
                        .openPrice                   = std::stod(JTO_STRING(streamArray, "o")),
                        .highPrice                   = std::stod(JTO_STRING(streamArray, "h")),
                        .lowPrice                    = std::stod(JTO_STRING(streamArray, "l")),
                        .totalTradedBaseAssetVolume  = std::stod(JTO_STRING(streamArray, "v")),
                        .totalTradedQuoteAssetVolume = std::stod(JTO_STRING(streamArray, "q"))
                    };
                    mmsmmt.insert({ JTO_STRING(value, "s"), { value["E"].GetUint64(), smmt } });
                }
                else if (stype == "24hrTicker")
                {
                    StreamMarketTickers smt {
                        .priceChange                 = std::stod(JTO_STRING(value, "p")),
                        .priceChangePercent          = std::stod(JTO_STRING(value, "P")),
                        .weightedAveratePrice        = std::stod(JTO_STRING(value, "w")),
                        .lastPrice                   = std::stod(JTO_STRING(value, "c")),
                        .lastPriceQuantity           = std::stod(JTO_STRING(value, "Q")),
                        .openPrice                   = std::stod(JTO_STRING(value, "o")),
                        .highPrice                   = std::stod(JTO_STRING(value, "h")),
                        .lowPrice                    = std::stod(JTO_STRING(value, "l")),
                        .totalTradedBaseAssetVolume  = std::stod(JTO_STRING(value, "v")),
                        .totalTradedQuoteAssetVolume = std::stod(JTO_STRING(value, "q")),
                        .statisticsOpenTime          = value["O"].GetInt64(),
                        .statisticsCloseTime         = value["C"].GetInt64(),
                        .firstTreadeId               = value["F"].GetInt64(),
                        .lastTradeId                 = value["L"].GetInt64(),
                        .totalNumberOfTrades         = value["n"].GetUint64()
                    };
                    mmsmt.insert({ JTO_STRING(value, "s"), { value["E"].GetUint64(), smt } });
                }
            }
            if (!vsmpam.empty())
                markPriceStreamAllMarket(vsmpam);
            if (!mmsmmt.empty())
                allMarketMiniTickers(mmsmmt);
            if (!mmsmt.empty())
                allMarketTickers(mmsmt);
        }
    } catch (const std::exception &ex)
    {
        fmt::print(fmt::fg(fmt::color::red), "JSON STREAM ERROR: ");
        fmt::print("JSON data could not be parsed: {}\n", ex.what());
    }
}

void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::aggregateTradeStream([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamAggregateTrade &as) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::markPriceStream([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamMarkPrice &smp) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::markPriceStreamAllMarket([[maybe_unused]] const std::vector<StreamMarkPriceAllMarket> &vsmpal) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::kline([[maybe_unused]] const BINAPI_NAMESPACE::sym_t &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const BinanceTimeIntervals &interval, [[maybe_unused]] const Candlestick &cs) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::continuousKline([[maybe_unused]] const BINAPI_NAMESPACE::sym_t &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const ContractType &ct, [[maybe_unused]] const BinanceTimeIntervals &interval, [[maybe_unused]] const Candlestick &cs) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::individualSymbolMiniTicker([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamIndividualSymbolMiniTicker &ticker) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::allMarketMiniTickers([[maybe_unused]] const std::multimap<std::string, std::pair<uint64_t, StreamMarketMiniTickers>> &mm) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::individualSymbolTicker([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamIndividualSymbolTicker &ticker) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::allMarketTickers([[maybe_unused]] const std::multimap<std::string, std::pair<uint64_t, StreamMarketTickers>> &mm) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::bookTicker([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamBookTicker &ticker) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::liquidationOrder([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamLiquidationOrder &slo) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::depthUpdate([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamDepthUpdate &sdp) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::blvtInfo([[maybe_unused]] const std::string &blvtName, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const StreamBLVTInfo &blvt) { }
void BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::compositeIndex([[maybe_unused]] const std::string &symbol, [[maybe_unused]] const uint64_t &eventTime, [[maybe_unused]] const currency_t &price, [[maybe_unused]] const std::vector<StreamCompositionIndex> &sci) { }

std::string BINAPI_NAMESPACE::ws::WSFuturesBinanceAPI::constructEndPointFromList(const std::vector<std::string> &subscriptions) noexcept
{
    std::string ret;
    int count = 0;
    for (auto &sub : subscriptions)
    {
        ret += sub + "/";
        ++count;
    }
    if (!ret.empty())
        ret.pop_back();
    if (count == 1)
        ret.insert(0, "/ws/");
    else if (count > 1)
        ret.insert(0, "/stream?streams=");
    else
        ret.clear();
    return ret;
}
