////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "schemas.hpp"

// R"()" [ \t]+
namespace BINAPI_NAMESPACE::local::schemas
{
    char api_error[]    = { R"({
"$schema": "http://json-schema.org/schema#",
"$id": "http://richardqpc.com/binapi/api_error.schema.json",
"title": "API Error Schema",
"type": "object",
"properties": { "code": { "type": "number" }, "msg": { "type": "string" } },
"additionalProperties": false,
"required": [ "code", "msg" ] }
)" };
    char sapi_error[]   = { R"({
"$schema": "http://json-schema.org/schema#",
"$id": "http://richardqpc.com/binapi/wapi_error.schema.json",
"title": "WAPI Error Schema",
"type": "object",
"properties": { "success": { "type": "boolean" },"msg": { "type": "string" } },
"additionalProperties": false,
"required": [ "msg" ] }
)" };

    char empty_schema[] = { R"({
"$schema": "http://json-schema.org/draft-07/schema",
"$id": "http://richardqpc.com/empty.schema.json",
"type": "object",
"required": [],
"additionalProperties": true
})" };

    namespace SPOT
    {
        char system_status[]                 = { R"({"$schema": "http://json-schema.org/draft-07/schema",
"$id": "http://richardqpc.com/binapi/system_status.schema.json",
"type": "object",
"description": "System Status",
"required": [ "status", "msg"],
"properties": {  "status": {    "$id": "#/properties/status",    "type": "integer",    "description": "0: normal. 1: system maintenance"  },  "msg": { "$id": "#/properties/msg", "type": "string",  "description": "normal or system maintenance"  }
},"additionalProperties": false})" };
        char all_coins_information[]         = { R"({"$schema": "http://json-schema.org/draft-04/schema#",
"type": "array","items": [{"type": "object","properties": {"coin": {"type": "string"},"depositAllEnable": {"type": "boolean"},"free": {"type": "string"},"freeze": {"type": "string"},"ipoable": {"type": "string"},
"ipoing": {"type": "string"},"isLegalMoney": {"type": "boolean"},"locked": {"type": "string"},"name": {
"type": "string"},"networkList": {"type": "array","items": [{"type": "object","properties": {"addressRegex": {"type": "string"},"coin": {"type": "string"},"depositDesc": {"type": "string"},
"depositEnable": {"type": "boolean"},"isDefault": {"type": "boolean"},"memoRegex": {"type": "string"},"minConfirm": {"type": "integer"},"name": {"type": "string"},"network": {"type": "string"},"resetAddressStatus": {
"type": "boolean"},"specialTips": {"type": "string"},"unLockConfirm": {"type": "integer"},"withdrawDesc": {"type": "string"},"withdrawEnable": {"type": "boolean"},"withdrawFee": {"type": "string"},"withdrawIntegerMultiple": {
"type": "string"},"withdrawMax": {"type": "string"},"withdrawMin": {"type": "string"},"sameAddress": {"type": "boolean"}},"additionalProperties": false,"required": ["addressRegex","coin","depositEnable","isDefault",
"memoRegex","minConfirm","name","network","resetAddressStatus","specialTips","unLockConfirm","withdrawEnable","withdrawFee","withdrawIntegerMultiple","withdrawMax","withdrawMin","sameAddress"]},{"type": "object",
"properties": {"addressRegex": {"type": "string"},"coin": {"type": "string"},"depositEnable": {"type": "boolean"},"isDefault": {"type": "boolean"},"memoRegex": {"type": "string"},"minConfirm": {"type": "integer"},
"name": {"type": "string"},"network": {"type": "string"},"resetAddressStatus": {"type": "boolean"},"specialTips": {"type": "string"},"unLockConfirm": {"type": "integer"},"withdrawEnable": {"type": "boolean"},"withdrawFee": {"type": "string"
},"withdrawIntegerMultiple": {"type": "string"},"withdrawMax": {"type": "string"},"withdrawMin": {"type": "string"},"sameAddress": {"type": "boolean"}},"additionalProperties": false,"required": ["addressRegex",
"coin","depositEnable","isDefault","memoRegex","minConfirm","name","network","resetAddressStatus","specialTips","unLockConfirm","withdrawEnable","withdrawFee","withdrawIntegerMultiple","withdrawMax","withdrawMin","sameAddress"
]}],"additionalItems": false},"storage": {"type": "string"},"trading": {"type": "boolean"},"withdrawAllEnable": {"type": "boolean"},"withdrawing": {"type": "string"}},"additionalProperties": false,"required": [
"coin","depositAllEnable","free","freeze","ipoable","ipoing","isLegalMoney","locked","name","networkList","storage","trading","withdrawAllEnable","withdrawing"]}],"additionalItems": false})" };
        char snapshot_spot[]                 = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":
{"code":{"type":"integer"},"msg":{"type":"string"},"snapshotVos":{"type":"array","items":[{"type":"object","properties":{"data":{"type":"object",
"properties":{"balances":{"type":"array","items":[{"type":"object","properties":{"asset":{"type":"string"},"free":{"type":"string"},"locked":{"type":"string"}},
"additionalProperties":false,"required":["asset","free","locked"]},{"type":"object","properties":{"asset":{"type":"string"},"free":{"type":"string"},"locked":
{"type":"string"}},"additionalProperties":false,"required":["asset","free","locked"]}],"additionalItems":false},"totalAssetOfBtc":{"type":"string"}},
"additionalProperties":false,"required":["balances","totalAssetOfBtc"]},"type":{"type":"string"},"updateTime":{"type":"integer"}},"additionalProperties":false,"required":
["data","type","updateTime"]}],"additionalItems":false}},"additionalProperties":false,"required":["code","msg","snapshotVos"]})" };
        char snapshot_margin[]               = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"code":{"type":"integer"},"msg":{"type":"string"},
"snapshotVos":{"type":"array","items":[{"type":"object","properties":{"data":{"type":"object","properties":{"marginLevel":{"type":"string"},"totalAssetOfBtc":{"type":"string"},
"totalLiabilityOfBtc":{"type":"string"},"totalNetAssetOfBtc":{"type":"string"},"userAssets":{"type":"array","items":[{"type":"object","properties":{"asset":{"type":"string"},
"borrowed":{"type":"string"},"free":{"type":"string"},"interest":{"type":"string"},"locked":{"type":"string"},"netAsset":{"type":"string"}},"additionalProperties":false,
"required":["asset","borrowed","free","interest","locked","netAsset"]}],"additionalItems":false}},"additionalProperties":false,"required":["marginLevel","totalAssetOfBtc",
"totalLiabilityOfBtc","totalNetAssetOfBtc","userAssets"]},"type":{"type":"string"},"updateTime":{"type":"integer"}},"additionalProperties":false,"required":["data","type","updateTime"]}],
"additionalItems":false}},"additionalProperties":false,"required":["code","msg","snapshotVos"]})" };
        char snapshot_futures[]              = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"code":{"type":"integer"},"msg":{"type":"string"},
"snapshotVos":{"type":"array","items":[{"type":"object","properties":{"data":{"type":"object","properties":{"assets":{"type":"array","items":[{"type":"object","properties":
{"asset":{"type":"string"},"marginBalance":{"type":"string"},"walletBalance":{"type":"string"}},"additionalProperties":false,"required":["asset","marginBalance","walletBalance"]}],
"additionalItems":false},"position":{"type":"array","items":[{"type":"object","properties":{"entryPrice":{"type":"string"},"markPrice":{"type":"string"},"positionAmt":{"type":"string"},
"symbol":{"type":"string"},"unRealizedProfit":{"type":"string"}},"additionalProperties":false,"required":["entryPrice","markPrice","positionAmt","symbol"]}],
"additionalItems":false}},"additionalProperties":false,"required":["assets","position"]},"type":{"type":"string"},"updateTime":{"type":"integer"}},"additionalProperties":false,
"required":["data","type","updateTime"]}],"additionalItems":false}},"additionalProperties":false,"required":["code","msg","snapshotVos"]})" };
        char *disable_fast_withdraw_switch   = empty_schema;
        char *enable_fast_withdraw_switch    = empty_schema;
        char withdraw[]                      = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"id":{"type":"string"}},"additionalProperties":false,"required":["id"]})" };
        char deposit_history[]               = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","items":{"type":"object","properties":{"amount":{"type":"string"},
"coin":{"type":"string"},"network":{"type":"string"},"status":{"type":"integer"},"address":{"type":"string"},"addressTag":{"type":"string"},"txId":{"type":"string"},"insertTime":
{"type":"integer"},"transferType":{"type":"integer"},"unlockConfirm":{"type":"string"},"confirmTimes":{"type":"string"}},"additionalProperties":false,"required":
["amount","coin","network","status","address","addressTag","txId","insertTime","transferType","unlockConfirm","confirmTimes"]},"additionalItems":false})" };
        char withdraw_history[]              = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","items":{"type":"object","properties":{"address":{"type":"string"},"amount":
{"type":"string"},"applyTime":{"type":"string"},"coin":{"type":"string"},"id":{"type":"string"},"withdrawOrderId":{"type":"string"},"network":{"type":"string"},"transferType":{"type":"integer"},
"status":{"type":"integer"},"transactionFee":{"type":"string"},"confirmNo":{"type":"integer"},"info":{"type":"string"},"txId":{"type":"string"}},"additionalProperties":false,"required":["address",
"amount","applyTime","coin","id","network","transferType","status","transactionFee","confirmNo","info","txId"]},"additionalItems":false})" };
        char deposit_address[]               = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"address":{"type":"string"},"coin":{"type":"string"},"tag"
:{"type":"string"},"url":{"type":"string"}},"additionalProperties":false,"required":["address","coin","tag","url"]})" };
        char account_status[]                = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"data":{"type":"string"}},"additionalProperties":false,"required":["data"]})" };
        char account_api_trading_status[]    = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"data":{"type":"object","properties":{"isLocked":{"type":"boolean"}
,"plannedRecoverTime":{"type":"integer"},"triggerCondition":{"type":"object","properties":{"GCR":{"type":"integer"},"IFER":{"type":"integer"},"UFR":{"type":"integer"}},"additionalProperties":false,"required":
["GCR","IFER","UFR"]},"updateTime":{"type":"integer"}},"additionalProperties":false,"required":["isLocked","plannedRecoverTime","triggerCondition","updateTime"]}},"additionalProperties":false,"required":["data"]})" };
        char dustlog[]                       = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"total":{"type":"integer"},"userAssetDribblets":{"type":"array",
"items":{"type":"object","properties":{"operateTime":{"type":"integer"},"totalTransferedAmount":{"type":"string"},"totalServiceChargeAmount":{"type":"string"},"transId":{"type":"integer"},
"userAssetDribbletDetails":{"type":"array","items":{"type":"object","properties":{"transId":{"type":"integer"},"serviceChargeAmount":{"type":"string"},"amount":{"type":"string"},"operateTime":
{"type":"integer"},"transferedAmount":{"type":"string"},"fromAsset":{"type":"string"}},"additionalProperties":false,"required":["transId","serviceChargeAmount","amount","operateTime","transferedAmount","fromAsset"]},
"additionalItems":false}},"additionalProperties":false,"required":["operateTime","totalTransferedAmount","totalServiceChargeAmount","transId","userAssetDribbletDetails"]},"additionalItems":false}},
"additionalProperties":false,"required":["total","userAssetDribblets"]})" };
        char assets_convert_to_bnb[]         = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"details":{"type":"array","items":{"type":"object","properties":
{"asset":{"type":"string"},"assetFullName":{"type":"string"},"amountFree":{"type":"string"},"toBTC":{"type":"string"},"toBNB":{"type":"string"},"toBNBOffExchange":{"type":"string"},"exchange":
{"type":"string"}},"additionalProperties":false,"required":["asset","assetFullName","amountFree","toBTC","toBNB","toBNBOffExchange","exchange"]},"additionalItems":false},"totalTransferBtc":
{"type":"string"},"totalTransferBNB":{"type":"string"},"dribbletPercentage":{"type":"string"}},"additionalProperties":false,"required":["details","totalTransferBtc","totalTransferBNB","dribbletPercentage"]})" };
        char dust_transfer[]                 = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"totalServiceCharge":{"type":"string"},"totalTransfered":
{"type":"string"},"transferResult":{"type":"array","items":{"type":"object","properties":{"amount":{"type":"string"},"fromAsset":{"type":"string"},"operateTime":{"type":"integer"},
"serviceChargeAmount":{"type":"string"},"tranId":{"type":"integer"},"transferedAmount":{"type":"string"}},"additionalProperties":false,"required":["amount","fromAsset","operateTime",
"serviceChargeAmount","tranId","transferedAmount"]},"additionalItems":false}},"additionalProperties":false,"required":["totalServiceCharge","totalTransfered","transferResult"]})" };
        char asset_dividend_record[]         = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"rows":{"type":"array","items":{"type":"object","properties":{"id":
{"type":"integer"},"amount":{"type":"string"},"asset":{"type":"string"},"divTime":{"type":"integer"},"enInfo":{"type":"string"},"tranId":{"type":"integer"}},"additionalProperties":false,
"required":["id","amount","asset","divTime","enInfo","tranId"]},"additionalItems":false},"total":{"type":"integer"}},"additionalProperties":false,"required":["rows","total"]})" };
        char asset_detail[]                  = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"CTR":{"type":"object","properties":
{"minWithdrawAmount":{"type":"string"},"depositStatus":{"type":"boolean"},"withdrawFee":{"type":"integer"},"withdrawStatus":{"type":"boolean"},"depositTip":{"type":"string"}},
"additionalProperties":false,"required":["minWithdrawAmount","depositStatus","withdrawFee","withdrawStatus","depositTip"]},"SKY":{"type":"object","properties":{"minWithdrawAmount":
{"type":"string"},"depositStatus":{"type":"boolean"},"withdrawFee":{"type":"number"},"withdrawStatus":{"type":"boolean"}},"additionalProperties":false,"required":["minWithdrawAmount",
"depositStatus","withdrawFee","withdrawStatus"]}},"additionalProperties":false,"required":["CTR","SKY"]})" };
        char trade_fee[]                     = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","items":{"type":"object","properties":{"symbol":{"type":"string"},
"makerCommission":{"type":"string"},"takerCommission":{"type":"string"}},"additionalProperties":false,"required":["symbol","makerCommission","takerCommission"]},"additionalItems":false})" };
        char user_universal_transfer[]       = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"tranId":{"type":"integer"}},
"additionalProperties":false,"required":["tranId"]})" };
        char query_user_universal_transfer[] = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"total":{"type":"integer"},"rows":
{"type":"array","items":{"type":"object","properties":{"asset":{"type":"string"},"amount":{"type":"string"},"type":{"type":"string"},"status":{"type":"string"},"tranId":{"type":"integer"},"timestamp":{"type":"integer"}},"additionalProperties":false,"required":["asset","amount","type","status","tranId","timestamp"]},"additionalItems":false}},"additionalProperties":false,"required":["total","rows"]})" };
        char funding_wallet[]                = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","items":{"type":"object","properties":{"asset":
{"type":"string"},"free":{"type":"string"},"locked":{"type":"string"},"freeze":{"type":"string"},"withdrawing":{"type":"string"},"btcValuation":{"type":"string"}},"additionalProperties":
false,"required":["asset","free","locked","freeze","withdrawing","btcValuation"]},"additionalItems":false})" };
        char get_api_key_permissions[]       = { R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"ipRestrict":{"type":"boolean"},
"createTime":{"type":"integer"},"enableWithdrawals":{"type":"boolean"},"enableInternalTransfer":{"type":"boolean"},"permitsUniversalTransfer":{"type":"boolean"},"enableVanillaOptions":
{"type":"boolean"},"enableReading":{"type":"boolean"},"enableFutures":{"type":"boolean"},"enableMargin":{"type":"boolean"},"enableSpotAndMarginTrading":{"type":"boolean"},
"tradingAuthorityExpirationTime":{"type":"integer"}},"additionalProperties":false,"required":["ipRestrict","createTime","enableWithdrawals","enableInternalTransfer",
"permitsUniversalTransfer","enableVanillaOptions","enableReading","enableFutures","enableMargin","enableSpotAndMarginTrading","tradingAuthorityExpirationTime"]})" };
        char *test_connectivity              = empty_schema;
        char check_server_time[]             = { R"({"$schema": "http://json-schema.org/draft-04/schema#","type": "object","properties": { "serverTime": { "type": "integer" }},"required": ["serverTime"]})" };
        char exchange_info[]                 = { R"({"$schema": "http://json-schema.org/draft-04/schema#","type": "object","properties": {  "timezone": {"type": "string"  }, "serverTime":
{"type": "integer"  },  "rateLimits": {"type": "array","items": {"type": "object","properties": {"rateLimitType": { "type": "string"},"interval": { "type": "string"},"intervalNum": { "type":
"integer"},"limit": { "type": "integer"}},"additionalProperties": true,"required": ["rateLimitType","interval","intervalNum","limit"]},"additionalItems": true  },  "exchangeFilters": {"type":
 "array","items": {"items": {},"additionalItems": true,"additionalProperties": true},"additionalItems": true  },  "symbols": {"type": "array","items": {"type": "object","properties": {"symbol":
{ "type": "string"},"status": { "type": "string", "enum": [  "TRADING",  "BREAK" ]},"baseAsset": { "type": "string"},"baseAssetPrecision": { "type": "integer"},"quoteAsset": { "type": "string"},
"quotePrecision": { "type": "integer"},"quoteAssetPrecision": { "type": "integer"},"baseCommissionPrecision": { "type": "integer"},"quoteCommissionPrecision": { "type": "integer"},"orderTypes":
{ "type": "array", "items": {  "type": "string" }, "additionalItems": true},"icebergAllowed": { "type": "boolean"},"ocoAllowed": { "type": "boolean"},"quoteOrderQtyMarketAllowed": { "type": "boolean"},
"allowTrailingStop": { "type": "boolean"},"isSpotTradingAllowed": { "type": "boolean"},"isMarginTradingAllowed": { "type": "boolean"},"filters": { "type": "array", "items": {  "type": "object",
"properties": {"filterType": {"type": "string"},"minPrice": {"type": "string"},"maxPrice": {"type": "string"},"tickSize": {"type": "string"},"multiplierUp": {"type": "string"},"multiplierDown":
{"type": "string"},"avgPriceMins": {"type": "integer"},"minQty": {"type": "string"},"maxQty": {"type": "string"},"stepSize": {"type": "string"},"minNotional": {"type": "string"},"applyToMarket":
 {"type": "boolean"},"limit": {"type": "integer"},"minTrailingAboveDelta": {"type": "integer"},"maxTrailingAboveDelta": {"type": "integer"},"minTrailingBelowDelta": {"type": "integer"},
"maxTrailingBelowDelta": {"type": "integer"},"maxNumOrders": {"type": "integer"},"maxNumAlgoOrders": {"type": "integer"}  },  "additionalProperties": true,  "required": ["filterType"  ] },
"additionalItems": true},"permissions": { "type": "array", "items": {  "type": "string" }, "additionalItems": true}},"additionalProperties": true,"required": ["symbol","status","baseAsset",
"baseAssetPrecision","quoteAsset","quotePrecision","quoteAssetPrecision","baseCommissionPrecision","quoteCommissionPrecision","orderTypes","icebergAllowed","ocoAllowed","quoteOrderQtyMarketAllowed",
"allowTrailingStop","isSpotTradingAllowed","isMarginTradingAllowed","filters","permissions"]},"additionalItems": true  }},"additionalProperties": true,"required": [  "timezone",  "serverTime",
 "rateLimits",  "exchangeFilters",  "symbols"]})" };
        char order_book[] = {R"({"$schema": "http://json-schema.org/draft-07/schema","$id": "http://richardqpc.com/schema/order_book_ticker.json","type": "object","required": [ "lastUpdateId", "bids",
 "asks"],"properties": { "lastUpdateId": { "$id": "#/properties/lastUpdateId", "type": "integer" }, "bids": { "$id": "#/properties/bids", "type": "array", "additionalItems": true, "items": { "$id":
"#/properties/bids/items", "anyOf": [ { "$id": "#/properties/bids/items/anyOf/0", "type": "array", "additionalItems": true, "items": { "$id": "#/properties/bids/items/anyOf/0/items", "anyOf":
 [ { "$id": "#/properties/bids/items/anyOf/0/items/anyOf/0", "type": "string" } ] } } ] } }, "asks": { "$id": "#/properties/asks", "type": "array", "additionalItems": true, "items":
 { "$id": "#/properties/asks/items", "anyOf": [ { "$id": "#/properties/asks/items/anyOf/0", "type": "array", "additionalItems": true, "items": { "$id": "#/properties/asks/items/anyOf/0/items",
"anyOf": [ { "$id": "#/properties/asks/items/anyOf/0/items/anyOf/0", "type": "string" } ] } } ] } }},"additionalProperties": false})"};
    } // namespace SPOT
    namespace Futures
    {

        char query_order[] = {
            R"({"$schema": "http://json-schema.org/draft-04/schema#",
"type": "object","properties": {"avgPrice": {"type": "string"},"clientOrderId": {"type": "string"},"cumQuote": {"type": "string"},"executedQty": {"type": "string"},"orderId": {"type": "integer"},"origQty": {"type": "string"},
  "origType": {"type": "string" },"price": {"type": "string"},"reduceOnly": {"type": "boolean" },"side": { "type": "string" }, "positionSide": {"type": "string"},
  "status": {"type": "string"}, "stopPrice": { "type": "string" },"closePosition": {"type": "boolean"},"symbol": {"type": "string"},"time": {"type": "integer"}, "timeInForce": {"type": "string"},
  "type": {"type": "string"}, "activatePrice": { "type": "string"}, "priceRate": { "type": "string"}, "updateTime": {"type": "integer"}, "workingType": { "type": "string" }, "priceProtect": { "type": "boolean" }},
"additionalProperties": false, "required": ["avgPrice","clientOrderId","cumQuote","executedQty","orderId", "origQty", "origType", "price","reduceOnly","side", "positionSide", "status", "stopPrice","closePosition","symbol","time",
"timeInForce","type","activatePrice","priceRate", "updateTime", "workingType", "priceProtect"]})"
        };
    } // namespace Futures
} // namespace BINAPI_NAMESPACE::local::schemas
