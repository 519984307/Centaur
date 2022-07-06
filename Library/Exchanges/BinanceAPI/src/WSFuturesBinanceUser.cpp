////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

//  WSFuturesBinanceUser.cpp

#include "WSFuturesBinanceUser.hpp"
#include "Binapi.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

#define JTO_STRING(x, y)                         \
    std::string                                  \
    {                                            \
        x[y].GetString(), x[y].GetStringLength() \
    }

BINAPI_NAMESPACE::ws::WSFuturesBinanceUser::WSFuturesBinanceUser(const std::string &listenKey, uint64_t pingTimer) :
    WSThread("fstream.binance.com", fmt::format("/ws/{}", listenKey), 443)
{
    setPingTimer(pingTimer);
}

BINAPI_NAMESPACE::ws::WSFuturesBinanceUser::~WSFuturesBinanceUser() = default;

void BINAPI_NAMESPACE::ws::WSFuturesBinanceUser::receivedData()
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
        std::string eventType = JTO_STRING(jsonDoc, "e");

        if (eventType == "listenKeyExpired")
        {
            listenKeyExpired(jsonDoc["E"].GetUint64());
        }
        else if (eventType == "MARGIN_CALL")
        {
            BINAPI_NAMESPACE::UserStreamMarginCallFutures usmcf;
            if (jsonDoc.FindMember("cw") != jsonDoc.MemberEnd())
                usmcf.crossMarginWallet = std::stod(JTO_STRING(jsonDoc, "cw"));
            for (const auto &pos : jsonDoc["p"].GetArray())
            {
                const std::string symbol { JTO_STRING(pos, "s") };
                const UserStreamMarginCallPosition usmcp {
                    .side = [&pos]() {
                        const std::string side { JTO_STRING(pos, "ps") };
                        if (side == "LONG")
                            return Futures::PositionSide::LONG;
                        else if (side == "SHORT")
                            return Futures::PositionSide::SHORT;
                        else
                            return Futures::PositionSide::BOTH;
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
            u_order_t clientOrderId { objectData["s"].GetString(), objectData["s"].GetStringLength() };

            const auto isLiquidation = clientOrderId.starts_with("autoclose-");
            if (isLiquidation)
                clientOrderId.erase(0, strlen("autoclose-"));

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
                .orderId              = objectData["i"].GetUint64(),
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
                        return u_order_t { objectData["N"].GetString(), objectData["N"].GetStringLength() };
                    return u_order_t {};
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
                        return Futures::PositionSide::LONG;
                    else if (pos == "SHORT")
                        return Futures::PositionSide::SHORT;
                    return Futures::PositionSide::BOTH;
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
            eventOrderUpdate(jsonDoc["E"].GetUint64(), jsonDoc["T"].GetUint64(), { objectData["s"].GetString(), objectData["s"].GetStringLength() }, usou);
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
