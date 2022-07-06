////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "WSSpotBinanceAPI.hpp"
#include "Binapi.hpp"
#include <random>

#include <fmt/color.h>
#include <fmt/core.h>

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

#define JTO_STRING(x, y)                         \
    std::string                                  \
    {                                            \
        x[y].GetString(), x[y].GetStringLength() \
    }

static std::string symbolToLower(const std::string &symbol)
{
    std::string str = symbol;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::WSSpotBinanceAPI(std::string endpoint) :
    WSThread("stream.binance.com", std::move(endpoint), 9443)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribe(const std::string &stream, int id)
{
    std::string subscribe = fmt::format(R"({{"method": "SUBSCRIBE","params":["{}"],"id":{}}})", stream, id);
    sendData(subscribe);
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::unsubscribe(const std::string &stream, int id)
{
    std::string unsubscribe = fmt::format(R"({{"method": "UNSUBSCRIBE","params":["{}"],"id":{}}})", stream, id);
    sendData(unsubscribe);
}

/**
 * \brief ccinfo.context        = m_context;
ccinfo.address        = "stream.binance.com";
ccinfo.port           = 9443;
ccinfo.path           = endpoint.c_str();
ccinfo.host           = lws_canonical_hostname(m_context);
ccinfo.origin         = "origin";
ccinfo.protocol       = m_protocols[0].name;
ccinfo.userdata       = reinterpret_cast<void *>(this);
ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;


 9443
 */

/*
void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::run(const std::string &endpoint, const bool &multipleStreams)
{
    lws_set_log_level(7, nullptr);
    if (m_running)
        throw(std::runtime_error("WebSocket already running"));

    m_terminate = false;

    if (m_context != nullptr)
        lws_context_destroy(m_context);

    // Init Context
    lws_context_creation_info info {} // C++ Initialization just to get rid of clang-tidy and clang warnings of uninitialized variables;
    memset(&info, 0, sizeof(info)); // Proper zero initialization of a C struct

    info.port      = CONTEXT_PORT_NO_LISTEN;
    info.protocols = m_protocols;
    info.gid       = -1;
    info.uid       = -1;
    info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    m_context = lws_create_context(&info);

    lws_client_connect_info ccinfo {};
    memset(&ccinfo, 0, sizeof(lws_client_connect_info));
    ccinfo.context        = m_context;
    ccinfo.address        = "stream.binance.com";
    ccinfo.port           = 9443;
    ccinfo.path           = endpoint.c_str();
    ccinfo.host           = lws_canonical_hostname(m_context);
    ccinfo.origin         = "origin";
    ccinfo.protocol       = m_protocols[0].name;
    ccinfo.userdata       = reinterpret_cast<void *>(this);
    ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

    m_lws                 = lws_client_connect_via_info(&ccinfo);

    if (m_lws == nullptr)
        throw std::runtime_error("failed to connect");

    m_combined = multipleStreams;
    while (!m_terminate)
    {
       /// lws_service(m_context, 0 As documentation denotes it, this value is deprecated and must be zero);
    }
    lws_context_destroy(m_context);
    m_context = nullptr;
}
*/

/*
void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::terminate()
{
if (!m_running)
    return;
m_terminate = true;
if (m_lws != nullptr)
    lws_callback_on_writable(m_lws);
}*/

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeKline(const std::string &symbol, BinanceTimeIntervals interval)
{
    std::string stream = fmt::format("{}@kline_{}", symbolToLower(symbol), BINAPI_NAMESPACE::BinanceAPI::fromIntervalToString(interval));
    SUBSCRIBE_METHOD(stream)
}

int BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::unsubscribeKline(const std::string &symbol, BinanceTimeIntervals interval)
{
    std::string stream = fmt::format("{}@kline_{}", symbolToLower(symbol), BINAPI_NAMESPACE::BinanceAPI::fromIntervalToString(interval));
    UNSUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeIndividualMiniTicker(const std::string &symbol)
{
    std::string stream = fmt::format("{}@miniTicker", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream);
}

int BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::unsubscribeIndividualMiniTicker(const std::string &symbol)
{
    std::string stream = fmt::format("{}@miniTicker", symbolToLower(symbol));
    UNSUBSCRIBE_METHOD(stream);
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeAllMarketMiniTickers()
{
    std::string stream = "!miniTicker@arr";
    SUBSCRIBE_METHOD(stream);
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeIndividualSymbolTicker(const std::string &symbol)
{
    std::string stream = fmt::format("{}@ticker", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream);
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeAllMarketTickers()
{
    std::string stream = "!ticker@arr";
    SUBSCRIBE_METHOD(stream);
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeIndividualSymbolBookTickerStreams(const std::string &symbol)
{
    std::string stream = fmt::format("{}@bookTicker", symbolToLower(symbol));
    SUBSCRIBE_METHOD(stream);
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeAllBookTickerStream()
{
    std::string stream = "!bookTicker";
    SUBSCRIBE_METHOD(stream);
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribePartialBookDepth(const std::string &symbol, int levels, int update)
{
    assert(levels == 5 || levels == 10 || levels == 20);
    assert(update == 1000 || update == 100);
    std::string stream = fmt::format("{}@depth{}@{}ms", symbolToLower(symbol), levels, update);
    SUBSCRIBE_METHOD(stream)
}

std::variant<std::string, int> BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribeDiffBookDepth(const std::string &symbol, int update)
{
    assert(update == 1000 || update == 100);
    std::string stream = fmt::format("{}@depth@{}ms", symbolToLower(symbol), update);
    SUBSCRIBE_METHOD(stream)
}

int BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::unsubscribeDiffBookDepth(const std::string &symbol, int update)
{
    assert(update == 1000 || update == 100);
    std::string stream = fmt::format("{}@depth@{}ms", symbolToLower(symbol), update);
    UNSUBSCRIBE_METHOD(stream)
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::subscribe([[maybe_unused]] bool result, [[maybe_unused]] int id)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::unsubscribe([[maybe_unused]] bool result, [[maybe_unused]] int id)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::kline([[maybe_unused]] const std::string &symbol, [[maybe_unused]] uint64_t eventTime, [[maybe_unused]] BinanceTimeIntervals interval, [[maybe_unused]] const Candlestick &cs)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::individualSymbolMiniTicker([[maybe_unused]] const std::string &symbol, [[maybe_unused]] uint64_t eventTime, [[maybe_unused]] const StreamIndividualSymbolMiniTicker &ticker)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::allMarketMiniTickers([[maybe_unused]] const std::multimap<std::string, std::pair<uint64_t, StreamMarketMiniTickers>> &mm)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::individualSymbolTicker([[maybe_unused]] const std::string &symbol, [[maybe_unused]] uint64_t eventTime, [[maybe_unused]] const StreamIndividualSymbolTicker &ticker)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::allMarketTickers([[maybe_unused]] const std::multimap<std::string, std::pair<uint64_t, StreamMarketTickers>> &mm)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::bookTicker([[maybe_unused]] const std::string &symbol, [[maybe_unused]] uint64_t eventTime, [[maybe_unused]] const StreamBookTicker &ticker)
{
}

void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::depthUpdate([[maybe_unused]] const std::string &symbol, [[maybe_unused]] uint64_t eventTime, [[maybe_unused]] const StreamDepthUpdate &sdp)
{
}
/*
void BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::sendData(const std::string &msg)
{
    char *str = new char[msg.size() + LWS_PRE + 1];
    memset(str, 0, msg.size() + LWS_PRE);
    strcpy(str + LWS_PRE, msg.c_str());
    lws_write(m_lws, reinterpret_cast<unsigned char *>(str) + LWS_PRE, msg.size(), LWS_WRITE_TEXT);
    // All previous calls are C functions and do not throw so we can be assure that delete is going to be called
    delete[] str;
}
*/
auto BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::receivedData() -> void
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

        const rapidjson::Value &value = m_combined ? (jsonDoc.FindMember("data") != jsonDoc.MemberEnd() ? jsonDoc["data"].GetObject() : jsonDoc.GetObject()) : jsonDoc.GetObject();
        std::string type              = JTO_STRING(value, "e");

        if (!value.IsArray())
        {
            if (type == "kline")
            {
                const auto &cd           = value["k"].GetObject();
                BinanceTimeIntervals bti = BinanceTimeIntervals::i1M;
                std::string interval     = JTO_STRING(cd, "i");
                if (interval == "1m")
                    bti = BinanceTimeIntervals::i1m;
                else if (interval == "3m")
                    bti = BinanceTimeIntervals::i3m;
                else if (interval == "5m")
                    bti = BinanceTimeIntervals::i5m;
                else if (interval == "15m")
                    bti = BinanceTimeIntervals::i15m;
                else if (interval == "30m")
                    bti = BinanceTimeIntervals::i30m;
                else if (interval == "1h")
                    bti = BinanceTimeIntervals::i1h;
                else if (interval == "2h")
                    bti = BinanceTimeIntervals::i2h;
                else if (interval == "4h")
                    bti = BinanceTimeIntervals::i4h;
                else if (interval == "6h")
                    bti = BinanceTimeIntervals::i6h;
                else if (interval == "8h")
                    bti = BinanceTimeIntervals::i8h;
                else if (interval == "12h")
                    bti = BinanceTimeIntervals::i12h;
                else if (interval == "1d")
                    bti = BinanceTimeIntervals::i1d;
                else if (interval == "3d")
                    bti = BinanceTimeIntervals::i3d;
                else if (interval == "1w")
                    bti = BinanceTimeIntervals::i1w;
                else if (interval == "1M")
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
                kline(JTO_STRING(value, "s"), value["E"].GetUint64(), bti, cs);
            }

            else if (type == "24hrMiniTicker")
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
            else if (type == "24hrTicker")
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
            else if (type == "bookTicker")
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

            else if (type == "depthUpdate")
            {

                StreamDepthUpdate sdp {
                    .transactionTime         = 0,
                    .firstUpdateId           = value["U"].GetUint64(),
                    .finalUpdateId           = value["u"].GetUint64(),
                    .finalUpdateIdLastStream = 0
                };
                for (const auto &bids : value["b"].GetArray())
                    sdp.bids[std::stod(JTO_STRING(bids, 0))] = std::stod(JTO_STRING(bids, 1));
                for (const auto &asks : value["a"].GetArray())
                    sdp.asks[std::stod(JTO_STRING(asks, 0))] = std::stod(JTO_STRING(asks, 1));

                depthUpdate(JTO_STRING(value, "s"), value["E"].GetUint64(), sdp);
            }
        }
        else // STREAM IS AN ARRAY
        {
            std::multimap<std::string, std::pair<uint64_t, StreamMarketMiniTickers>> mmsmmt;
            std::multimap<std::string, std::pair<uint64_t, StreamMarketTickers>> mmsmt;
            for (const auto &streamArray : value.GetArray())
            {
                const std::string stype = JTO_STRING(streamArray, "e");
                if (stype == "24hrMiniTicker")
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
