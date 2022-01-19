////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WSFuturesBinanceUser.cpp

/**
 * @file  WSFuturesBinanceUser.cpp
 * @brief
 *
 * @author Ricardo Romero
 * @date 03/12/2021
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#include "WSFuturesBinanceUser.hpp"
#include "Binapi.hpp"

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

#define JTO_STRING(x, y) \
    std::string { x[y].GetString(), x[y].GetStringLength() }

trader::api::ws::WSFuturesBinanceUser::WSFuturesBinanceUser()
{
    // Init protocol
    m_protocols[0] = { "wsProtocol", WSFuturesBinanceUser::eventManager, 0, 65536, 0, nullptr, 0 };
    m_protocols[1] = { nullptr, nullptr, 0, 0, 0, nullptr, 0 };
    // lws_set_log_level(LLL_THREAD | LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO, nullptr);
    lws_set_log_level(0, nullptr);
}

trader::api::ws::WSFuturesBinanceUser::~WSFuturesBinanceUser() = default;

void trader::api::ws::WSFuturesBinanceUser::terminate()
{
    m_terminate = true;
    lws_cancel_service(m_context);
}

void trader::api::ws::WSFuturesBinanceUser::run(const std::string &listenKey, const uint64_t &ping)
{
    std::string endpoint = "/ws/" + listenKey;
    if (m_running)
        throw(std::runtime_error("WebSocket already running"));

    m_terminate = false;
    m_ping      = ping;

    if (m_context != nullptr)
        lws_context_destroy(m_context);

    // Init Context
    lws_context_creation_info info {} /*C++ Initialization just to get rid of clang-tidy and clang warnings of uninitialized variables*/;
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
    ccinfo.address        = "fstream.binance.com";
    ccinfo.port           = 443;
    ccinfo.path           = endpoint.c_str();
    ccinfo.host           = lws_canonical_hostname(m_context);
    ccinfo.origin         = "origin";
    ccinfo.protocol       = m_protocols[0].name;
    ccinfo.userdata       = reinterpret_cast<void *>(this);
    ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

    m_lws                 = lws_client_connect_via_info(&ccinfo);

    if (m_lws == nullptr)
        throw std::runtime_error("failed to connect");

    while (!m_terminate)
    {
        lws_service(m_context, 0 /*As documentation denotes it, this value is deprecated and must be zero*/);
    }
    lws_context_destroy(m_context);
    m_context = nullptr;
    m_running = false;
}

void trader::api::ws::WSFuturesBinanceUser::receivedData(const std::string &msg)
{
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(msg.c_str());
    if (jsonDoc.HasParseError())
    {
        fmt::print(fmt::fg(fmt::color::red), "STREAM ERROR: ");
        fmt::print("JSON data is corrupted\n");
        return;
    }

    try
    {

        std::string eventType = JTO_STRING(jsonDoc, "e");

        if (eventType == "listenKeyExpired")
        {
            listenKeyExpired(jsonDoc["E"].GetUint64());
        }
        else if (eventType == "MARGIN_CALL")
        {
            trader::api::UserStreamMarginCallFutures usmcf;
            if (jsonDoc.FindMember("cw") != jsonDoc.MemberEnd())
                usmcf.crossMarginWallet = std::stod(JTO_STRING(jsonDoc, "cw"));
            for (const auto &pos : jsonDoc["p"].GetArray())
            {
                const std::string symbol { JTO_STRING(pos, "s") };
                const UserStreamMarginCallPosition usmcp {
                    .side = [&pos]() {
                        const std::string side { JTO_STRING(pos, "ps") };
                        if (side == "LONG")
                            return PositionSide::LONG;
                        else if (side == "SHORT")
                            return PositionSide::SHORT;
                        else
                            return PositionSide::BOTH;
                    }(),
                    .positionAmount            = std::stod(JTO_STRING(pos, "pa")),
                    .markPrice                 = std::stod(JTO_STRING(pos, "mp")),
                    .isolatedWallet            = std::stod(JTO_STRING(pos, "iw")),
                    .unrealizedPnL             = std::stod(JTO_STRING(pos, "up")),
                    .maintenanceMarginRequired = std::stod(JTO_STRING(pos, "mm")),
                    .marginType                = [&]() {
                        const std::string type { JTO_STRING(pos, "mt") };
                        if (type == "CROSSED")
                            return MarginType::crossed;
                        return MarginType::isolated;
                    }(),
                };
                usmcf.positions[symbol] = usmcp;
            }
            eventMarginCall(jsonDoc["E"].GetUint64(), usmcf);
        }
        else if (eventType == "ACCOUNT_UPDATE")
        {
        }
        else if (eventType == "ORDER_TRADE_UPDATE")
        {
            const auto &objectData = jsonDoc["o"];
            std::string clientOrderId { JTO_STRING(objectData, "s") };
            const bool isLiquidation = clientOrderId.starts_with("autoclose-");
            if (isLiquidation)
                clientOrderId.erase(0, std::string("autoclose-").size());

            const bool isADLAutoClose = [&clientOrderId]() {
                if (clientOrderId == "adl_autoclose")
                {
                    clientOrderId.clear();
                    return true;
                }
                return false;
            }();

            UserStreamOrderUpdate usou {
                .clientOrderId  = clientOrderId,
                .isLiquidation  = isLiquidation,  // See BinanceAPI documentation. if isLiquidation == true; clientOrderId will not have the 'autoclose-' prefix
                .isADLAutoClose = isADLAutoClose, // See BinanceAPI documentation
                .side           = [&objectData]() {
                    const std::string side { JTO_STRING(objectData, "S") };
                    if (side == "SELL")
                        return OrderSide::sell;
                    else
                        return OrderSide::buy;
                }(),
                .type = [&objectData]() {
                    const std::string type { JTO_STRING(objectData, "o") };
                    if (type == "MARKET")
                        return StreamOrderTypeFutures::market;
                    else if (type == "LIMIT")
                        return StreamOrderTypeFutures::limit;
                    else if (type == "STOP")
                        return StreamOrderTypeFutures::stop;
                    else if (type == "TAKE_PROFIT")
                        return StreamOrderTypeFutures::takeProfit;
                    else if (type == "LIQUIDATION")
                        return StreamOrderTypeFutures::liquidation;
                    else if (type == "TRAILING_STOP_MARKET")
                        return StreamOrderTypeFutures::trailingStopMarket;
                    else
                        return StreamOrderTypeFutures::unknown;
                }(),
                .tif = [&objectData]() {
                    const std::string tif { JTO_STRING(objectData, "f") };
                    if (tif == "GTC")
                        return OrderTimeInForce::GTC;
                    else if (tif == "IOC")
                        return OrderTimeInForce::IOC;
                    else if (tif == "FOK")
                        return OrderTimeInForce::FOK;
                    else
                        return OrderTimeInForce::GTX;
                }(),
                .originalQty   = std::stod(JTO_STRING(objectData, "q")),
                .originalPrice = std::stod(JTO_STRING(objectData, "p")),
                .averagePrice  = std::stod(JTO_STRING(objectData, "ap")),
                .stopPrice     = std::stod(JTO_STRING(objectData, "sp")),
                .execType      = [&objectData]() {
                    const std::string exec { JTO_STRING(objectData, "x") };
                    if (exec == "NEW")
                        return StreamOrderExecutionTypeFutures::sNew;
                    else if (exec == "CANCELED")
                        return StreamOrderExecutionTypeFutures::canceled;
                    else if (exec == "CALCULATED")
                        return StreamOrderExecutionTypeFutures::calculated;
                    else if (exec == "EXPIRED")
                        return StreamOrderExecutionTypeFutures::expired;
                    else if (exec == "TRADE")
                        return StreamOrderExecutionTypeFutures::trade;
                    else
                        return StreamOrderExecutionTypeFutures::unknown;
                }(),
                .status = [&objectData]() {
                    const std::string status { JTO_STRING(objectData, "X") };
                    if (status == "NEW")
                        return StreamOrderStatusFutures::sNew;
                    else if (status == "PARTIALLY_FILLED")
                        return StreamOrderStatusFutures::partiallyFilled;
                    else if (status == "FILLED")
                        return StreamOrderStatusFutures::filled;
                    else if (status == "CANCELED")
                        return StreamOrderStatusFutures::canceled;
                    else if (status == "EXPIRED")
                        return StreamOrderStatusFutures::expired;
                    else if (status == "NEW_INSURANCE")
                        return StreamOrderStatusFutures::newInsurance;
                    else if (status == "NEW_ADL")
                        return StreamOrderStatusFutures::newADL;
                    else
                        return StreamOrderStatusFutures::unknown;
                }(),
                .orderId              = objectData["i"].GetInt64(),
                .lastFilledQty        = std::stod(JTO_STRING(objectData, "l")),
                .filledAccumulatedQty = std::stod(JTO_STRING(objectData, "z")),
                .lastFilledPrice      = std::stod(JTO_STRING(objectData, "L")),
                .commissionAmmount    = [&objectData]() {
                    if (objectData.FindMember("n") != objectData.MemberEnd())
                        return std::stod(JTO_STRING(objectData, "n"));
                    return -1.0;
                }(),
                .commissionAsset = [&objectData]() {
                    if (objectData.FindMember("N") != objectData.MemberEnd())
                        return JTO_STRING(objectData, "N");
                    return std::string("");
                }(),
                .tradeTime    = objectData["T"].GetUint64(),
                .tradeId      = objectData["t"].GetInt64(),
                .bidsNotional = std::stod(JTO_STRING(objectData, "b")),
                .asksNotional = std::stod(JTO_STRING(objectData, "a")),
                .isMaker      = objectData["m"].GetBool(),
                .isReduceOnly = objectData["R"].GetBool(),
                .workType     = [&objectData]() {
                    const std::string work { JTO_STRING(objectData, "wt") };
                    if (work == "MARK_PRICE")
                        return WorkingType::markPrice;
                    else
                        return WorkingType::contractPrice;
                }(),
                .originalOrderType = [&objectData]() {
                    const std::string type { JTO_STRING(objectData, "ot") };

                    if (type == "LIMIT")
                        return OrderType::limit;
                    else if (type == "MARKET")
                        return OrderType::market;
                    else if (type == "STOP_LOSS")
                        return OrderType::stopLoss;
                    else if (type == "STOP_LOSS_LIMIT")
                        return OrderType::stopLossLimit;
                    else if (type == "TAKE_PROFIT")
                        return OrderType::takeProfit;
                    else if (type == "TAKE_PROFIT_LIMIT")
                        return OrderType::takeProfitLimit;
                    else if (type == "LIMIT_MAKER")
                        return OrderType::limitMaker;
                    else if (type == "STOP")
                        return OrderType::stop;
                    else if (type == "STOP_MARKET")
                        return OrderType::stopMarket;
                    else if (type == "TAKE_PROFIT_MARKET")
                        return OrderType::takeProfitMarket;
                    else if (type == "TRAILING_PROFIT_MARKET")
                        return OrderType::trailingStopMarket;
                    return OrderType::unknown;
                }(),
                .posSide = [&objectData]() {
                    const std::string pos { JTO_STRING(objectData, "ps") };
                    if (pos == "LONG")
                        return PositionSide::LONG;
                    else if (pos == "SHORT")
                        return PositionSide::SHORT;
                    return PositionSide::BOTH;
                }(),
                .closeAll        = objectData["cp"].GetBool(), // if not conditional order the value is false
                .activationPrice = [&objectData]() {
                    if (objectData.FindMember("AP") != objectData.MemberEnd())
                        return std::stod(JTO_STRING(objectData, "AP"));
                    return -1.0;
                }(),
                .callbackRate = [&objectData]() {
                    if (objectData.FindMember("cr") != objectData.MemberEnd())
                        return std::stod(JTO_STRING(objectData, "cr"));
                    return -1.0;
                }(),
                .realizedProfit = std::stod(JTO_STRING(objectData, "rp"))
            };
            eventOrderUpdate(jsonDoc["E"].GetUint64(), jsonDoc["T"].GetUint64(), JTO_STRING(objectData, "s"), usou);
        }
        else if (eventType == "ACCOUNT_CONFIG_UPDATE")
        {
            const auto eventTime       = jsonDoc["E"].GetUint64();
            const auto transactionTime = jsonDoc["T"].GetUint64();
            if (jsonDoc.FindMember("ac") != jsonDoc.MemberEnd())
                eventLeverageUpdate(eventTime, transactionTime, jsonDoc["ac"]["s"].GetString(), jsonDoc["ac"]["l"].GetInt64());

            if (jsonDoc.FindMember("ai") != jsonDoc.MemberEnd())
                eventMultiAssetsModeUpdate(eventTime, transactionTime, jsonDoc["ai"]["j"].GetBool());
        }
    } catch (const std::exception &ex)
    {
        fmt::print(fmt::fg(fmt::color::red), "JSON STREAM ERROR: ");
        fmt::print("JSON data could not be parsed: {}\n", ex.what());
    }
}

int trader::api::ws::WSFuturesBinanceUser::eventManager(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    auto client = reinterpret_cast<trader::api::ws::WSFuturesBinanceUser *>(user);

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif /*__clang__*/
    switch (reason)
    {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lws_callback_on_writable(wsi);
            if (client)
            {
                if (client->m_ping)
                    lws_set_timer_usecs(wsi, static_cast<lws_usec_t>(client->m_ping) * LWS_USEC_PER_SEC);
                client->m_running = true;
                client->connected();
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            {
                std::string msg = std::string(reinterpret_cast<char *>(in), len);

                if (client)
                {
                    if (lws_frame_is_binary(wsi))
                    {
                        fmt::print("LWS received binary data and was ignored.\n");
                    }
                    else
                        client->receivedData(std::string(reinterpret_cast<char *>(in), len));
                }
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            break;

        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_CLIENT_CLOSED:
            if (client)
            {
                // Set proper flags to allow reconnection
                client->m_running   = false;
                client->m_terminate = true;
                client->close();
            }
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            if (client)
            {
                client->m_running   = false;
                client->m_terminate = true;
                client->connectionError();
            }
            break;
        case LWS_CALLBACK_TIMER:
            if (client && client->m_ping)
            {
                uint8_t ping[LWS_PRE + 125];
                int m = lws_write(wsi, ping + LWS_PRE, 0, LWS_WRITE_PING);
                if (m < 0)
                    client->pingSent(false);
                else
                    client->pingSent(true);

                lws_set_timer_usecs(wsi, static_cast<lws_usec_t>(client->m_ping * LWS_USEC_PER_SEC));
            }
            break;

        default:
            break;
    }

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

    return 0;
}
