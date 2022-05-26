////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include <array>
#include <iostream>
#include <random>

#include <fmt/format.h>

#include "APIException.hpp"
#include "schemas.hpp"

#include "Binapi.hpp"
#include "schemas.hpp"

#define VIEW_INIT(x)       \
    {                      \
        x.data(), x.size() \
    }

#define S_VIEW_INIT(x)     \
    std::string            \
    {                      \
        x.data(), x.size() \
    }

namespace
{
#if defined(__clang__) || defined(__GNUC__)
    BINAPI_WARN_PUSH
    // clang-format off
BINAPI_WARN_OFF(exit-time-destructors)
BINAPI_WARN_OFF(global-constructors)
// clang-format on
#endif /*defined(__clang__) || defined(__GNUC__)*/

    using namespace BINAPI_NAMESPACE::local;
    const std::array<BinanceAPIRequest, 23> g_spotRequests {
        {
            { 1, "/sapi/v1/system/status", HTTPRequest::GET, false, loadSchema(schemas::SPOT::system_status), loadSchema(schemas::sapi_error), true, false },
            { 10, "/sapi/v1/capital/config/getall", HTTPRequest::GET, false, loadSchema(schemas::SPOT::all_coins_information), loadSchema(schemas::sapi_error), true, false },
            { 2400, "/sapi/v1/accountSnapshot", HTTPRequest::GET, false, loadSchema(schemas::SPOT::snapshot_spot), loadSchema(schemas::sapi_error), true, false },
            { 2400, "/sapi/v1/accountSnapshot", HTTPRequest::GET, false, loadSchema(schemas::SPOT::snapshot_margin), loadSchema(schemas::sapi_error), true, false },
            { 2400, "/sapi/v1/accountSnapshot", HTTPRequest::GET, false, loadSchema(schemas::SPOT::snapshot_futures), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/account/disableFastWithdrawSwitch", HTTPRequest::POST, false, loadSchema(schemas::SPOT::disable_fast_withdraw_switch), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/account/enableFastWithdrawSwitch", HTTPRequest::POST, false, loadSchema(schemas::SPOT::enable_fast_withdraw_switch), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/capital/withdraw/apply", HTTPRequest::POST, false, loadSchema(schemas::SPOT::withdraw), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/capital/deposit/hisrec", HTTPRequest::GET, false, loadSchema(schemas::SPOT::deposit_history), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/capital/withdraw/history", HTTPRequest::GET, false, loadSchema(schemas::SPOT::withdraw_history), loadSchema(schemas::sapi_error), true, false },
            { 10, "/sapi/v1/capital/deposit/address", HTTPRequest::GET, false, loadSchema(schemas::SPOT::deposit_address), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/account", HTTPRequest::GET, false, loadSchema(schemas::SPOT::account_status), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/account/apiTradingStatus", HTTPRequest::GET, false, loadSchema(schemas::SPOT::account_api_trading_status), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/asset/dribblet", HTTPRequest::GET, false, loadSchema(schemas::SPOT::dustlog), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/asset/dust-btc", HTTPRequest::POST, false, loadSchema(schemas::SPOT::assets_convert_to_bnb), loadSchema(schemas::sapi_error), true, false },
            { 10, "/sapi/v1/asset/dust", HTTPRequest::POST, false, loadSchema(schemas::SPOT::dust_transfer), loadSchema(schemas::sapi_error), true, false },
            { 10, "/sapi/v1/asset/assetDividend", HTTPRequest::GET, false, loadSchema(schemas::SPOT::asset_dividend_record), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/asset/assetDetail", HTTPRequest::GET, false, loadSchema(schemas::SPOT::asset_detail), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/asset/tradeFee", HTTPRequest::GET, false, loadSchema(schemas::SPOT::trade_fee), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/asset/transfer", HTTPRequest::POST, false, loadSchema(schemas::SPOT::user_universal_transfer), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/asset/transfer", HTTPRequest::GET, false, loadSchema(schemas::SPOT::query_user_universal_transfer), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/asset/get-funding-asset", HTTPRequest::POST, false, loadSchema(schemas::SPOT::funding_wallet), loadSchema(schemas::sapi_error), true, false },
            { 1, "/sapi/v1/account/apiRestrictions", HTTPRequest::GET, false, loadSchema(schemas::SPOT::get_api_key_permissions), loadSchema(schemas::sapi_error), true, false },

        }
    };

#if defined(__clang__) || defined(__GNUC__)
    BINAPI_WARN_POP
#endif /*defined(__clang__) || defined(__GNUC__)*/

    enum SPOTRequests
    {
        SYSTEM_STATUS = 0,
        ALL_COINS_INFORMATION,
        DAILY_ACCOUNT_SNAPSHOT_SPOT,
        DAILY_ACCOUNT_SNAPSHOT_MARGIN,
        DAILY_ACCOUNT_SNAPSHOT_FUTURES,
        DISABLE_FAST_WITHDRAW_SWITCH,
        ENABLE_FAST_WITHDRAW_SWITCH,
        WITHDRAW,
        DEPOSIT_HISTORY,
        WITHDRAW_HISTORY,
        DEPOSIT_ADDRESS,
        ACCOUNT_STATUS,
        ACCOUNT_API_TRADING_STATUS,
        DUSTLOG,
        ASSETS_CAN_BE_CONVERTED,
        DUST_TRANSFER,
        ASSET_DIVIDEND_RECORD,
        ASSET_DETAIL,
        TRADE_FEE,
        USER_UNIVERSAL_TRANSFER,
        QUERY_USER_UNIVERSAL_TRANSFER,
        FUNDING_WALLET,
        API_KEY_PERMISSION,
    };

} // namespace

BINAPI_NAMESPACE::BinanceAPISpot::BinanceAPISpot(std::string_view userAgent, BinanceKeys *keys, BinanceLimits *exl) :
    BinanceAPI("https://api.binance.com", userAgent, keys, exl)
{
}
BINAPI_NAMESPACE::BinanceAPISpot::BinanceAPISpot(BinanceKeys *keys, BinanceLimits *exl) :
    BinanceAPI("https://api.binance.com", "CPP_interface_api/curl-openssl/7.70.0/Ricardo", keys, exl)
{
}

BINAPI_NAMESPACE::BinanceAPISpot::~BinanceAPISpot() = default;

auto binapi::BinanceAPISpot::getExchangeStatus() -> bool
{
    auto doc = apiRequest(g_spotRequests[SYSTEM_STATUS]);
    return doc["status"].GetInt() == 0;
}

#define JTO_STRING(x, y) \
    std::string { x[y].GetString(), x[y].GetStringLength() }

#define JTO_DOB(x, y) \
    std::stod(JTO_STRING(x, y))

auto binapi::BinanceAPISpot::getAllCoinsInformation() -> std::vector<SPOT::CoinInformation>
{
    auto doc = apiRequest(g_spotRequests[ALL_COINS_INFORMATION],
        { { "recvWindow", fmt::to_string(getRecvWindow()) },
            { "timestamp", fmt::to_string(getTime()) } },
        true);
    std::vector<SPOT::CoinInformation> info;

    for (const auto &data : doc.GetArray())
    {
        SPOT::CoinInformation cif {
            .coin              = JTO_STRING(data, "coin"),
            .name              = JTO_STRING(data, "name"),
            .free              = JTO_DOB(data, "freeze"),
            .freeze            = JTO_DOB(data, "freeze"),
            .ipoable           = JTO_DOB(data, "ipoable"),
            .ipoing            = JTO_DOB(data, "ipoing"),
            .locked            = JTO_DOB(data, "locked"),
            .storage           = JTO_DOB(data, "storage"),
            .withdrawing       = JTO_DOB(data, "withdrawing"),
            .depositAlLEnable  = data["depositAllEnable"].GetBool(),
            .isLegalMoney      = data["isLegalMoney"].GetBool(),
            .trading           = data["trading"].GetBool(),
            .withdrawAllEnable = data["withdrawAllEnable"].GetBool()
        };

        for (const auto &list : data["networkList"].GetArray())
        {
            SPOT::CoinInformation::NetworkList ls {
                .coin               = JTO_STRING(list, "coin"),
                .addressRegEx       = JTO_STRING(list, "addressRegex"),
                .depositDescription = [&list]() -> std::string {
                    if (!list["depositEnable"].GetBool())
                        return JTO_STRING(list, "depositDesc");
                    else
                        return "";
                }(),
                .memoRegEx           = JTO_STRING(list, "memoRegex"),
                .name                = JTO_STRING(list, "name"),
                .network             = JTO_STRING(list, "network"),
                .specialTips         = JTO_STRING(list, "specialTips"),
                .withdrawDescription = [&list]() -> std::string {
                    if (!list["withdrawEnable"].GetBool())
                        return JTO_STRING(list, "withdrawDesc");
                    else
                        return "";
                }(),
                .withdrawFee             = JTO_DOB(list, "withdrawFee"),
                .withdrawIntegerMultiple = JTO_DOB(list, "withdrawIntegerMultiple"),
                .withdrawMin             = JTO_DOB(list, "withdrawMin"),
                .withdrawMax             = JTO_DOB(list, "withdrawMax"),
                .minConfirmation         = list["minConfirm"].GetInt64(),
                .unLockConfirm           = list["unLockConfirm"].GetInt64(),
                .depositEnable           = list["depositEnable"].GetBool(),
                .isDefault               = list["isDefault"].GetBool(),
                .resetAddressStatus      = list["resetAddressStatus"].GetBool(),
                .withdrawEnable          = list["withdrawEnable"].GetBool(),
                .sameAddress             = list["sameAddress"].GetBool()
            };
            cif.networkList.emplace_back(ls);
        }

        info.emplace_back(cif);
    }
    return info;
}
auto binapi::BinanceAPISpot::getDailyAccountSnapshotSpot(uint64_t limit, uint64_t startTime, uint64_t endTime) -> std::vector<SPOT::AccountSnapshotSpot>
{
    assert(limit >= 7);
    assert(limit <= 30);

    eparams params = {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "limit", fmt::to_string(limit) },
        { "type", "SPOT" }
    };

    if (startTime > 0)
        params.emplace_back("startTime", fmt::to_string(startTime));

    if (endTime > 0)
        params.emplace_back("endTime", fmt::to_string(endTime));

    auto doc = apiRequest(g_spotRequests[DAILY_ACCOUNT_SNAPSHOT_SPOT], params, true);

    std::vector<SPOT::AccountSnapshotSpot> dass;

    for (const auto &vos : doc["snapshotVos"].GetArray())
    {
        SPOT::AccountSnapshotSpot acs {
            .updateTime      = vos["updateTime"].GetUint64(),
            .totalAssetOfBtc = std::stod(vos["data"]["balances"]["totalAssetOfBtc"].GetString())
        };

        for (const auto &balances : vos["data"]["balances"].GetArray())
        {
            acs.balances[JTO_STRING(balances, "asset")] = { JTO_DOB(balances, "free"), JTO_DOB(balances, "locked") };
        }

        dass.emplace_back(acs);
    }

    return dass;
}

auto binapi::BinanceAPISpot::getDailyAccountSnapshotMargin(uint64_t limit, uint64_t startTime, uint64_t endTime) -> std::vector<SPOT::AccountSnapshotMargin>
{
    assert(limit >= 7);
    assert(limit <= 30);

    eparams params = {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "limit", fmt::to_string(limit) },
        { "type", "MARGIN" }
    };

    if (startTime > 0)
        params.emplace_back("startTime", fmt::to_string(startTime));

    if (endTime > 0)
        params.emplace_back("endTime", fmt::to_string(endTime));

    auto doc = apiRequest(g_spotRequests[DAILY_ACCOUNT_SNAPSHOT_MARGIN], params, true);

    std::vector<SPOT::AccountSnapshotMargin> acsm;

    for (const auto &vos : doc["snapshotVos"].GetArray())
    {
        const auto &data = vos["data"].GetObject();
        SPOT::AccountSnapshotMargin csm {
            .updateTime          = vos["updateTime"].GetUint64(),
            .marginLevel         = JTO_DOB(data, "marginLevel"),
            .totalAssetOfBtc     = JTO_DOB(data, "totalAssetOfBtc"),
            .totalLiabilityOfBtc = JTO_DOB(data, "totalLiabilityOfBtc"),
            .totalNetAssetOfBtc  = JTO_DOB(data, "totalNetAssetOfBtc"),
        };

        for (const auto &assets : data["userAssets"].GetArray())
        {
            csm.balances[JTO_STRING(assets, "asset")] = {
                .borrowed = JTO_DOB(assets, "borrowed"),
                .free     = JTO_DOB(assets, "free"),
                .interest = JTO_DOB(assets, "interest"),
                .locked   = JTO_DOB(assets, "locked"),
                .netAsset = JTO_DOB(assets, "netAsset")
            };
        }

        acsm.emplace_back(csm);
    }

    return acsm;
}

auto binapi::BinanceAPISpot::getDailyAccountSnapshotFutures(uint64_t limit, uint64_t startTime, uint64_t endTime) -> std::vector<SPOT::AccountSnapshotFutures>
{
    assert(limit >= 7);
    assert(limit <= 30);

    eparams params = {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "limit", fmt::to_string(limit) },
        { "type", "FUTURES" }
    };

    if (startTime > 0)
        params.emplace_back("startTime", fmt::to_string(startTime));

    if (endTime > 0)
        params.emplace_back("endTime", fmt::to_string(endTime));

    auto doc = apiRequest(g_spotRequests[DAILY_ACCOUNT_SNAPSHOT_FUTURES], params, true);

    std::vector<SPOT::AccountSnapshotFutures> vasf;

    for (const auto &vos : doc["snapshotVos"].GetArray())
    {
        SPOT::AccountSnapshotFutures asf {
            .updateTime = vos["updateTime"].GetUint64()
        };

        for (const auto &assets : vos["data"]["assets"].GetArray())
        {
            asf.balances[JTO_STRING(assets, "asset")] = {
                .marginBalance = JTO_DOB(assets, "marginBalance"),
                .walletBalance = JTO_DOB(assets, "walletBalance")
            };
        }

        for (const auto &positions : vos["data"]["position"].GetArray())
        {
            SPOT::AccountSnapshotFutures::FuturesPositionInformation fpi {
                .entryPrice  = JTO_DOB(positions, "entryPrice"),
                .markPrice   = JTO_DOB(positions, "markPrice"),
                .positionAmt = JTO_DOB(positions, "positionAmt")
            };

            if (positions.FindMember("unRealizedProfit") != positions.MemberEnd())
                fpi.unRealizedProfit = JTO_DOB(positions, "unRealizedProfit");

            asf.positions[JTO_STRING(positions, "symbol")] = fpi;
        }

        vasf.emplace_back(asf);
    }
    return vasf;
}
auto binapi::BinanceAPISpot::disableFastWithdrawSwitch() -> void
{
    auto doc = apiRequest(g_spotRequests[DISABLE_FAST_WITHDRAW_SWITCH], true);
}
auto binapi::BinanceAPISpot::enableFastWithdrawSwitch() -> void
{
    auto doc = apiRequest(g_spotRequests[ENABLE_FAST_WITHDRAW_SWITCH], true);
}

auto binapi::BinanceAPISpot::withdraw(binapi::v_asset_t coin, binapi::v_address_t address, std::string_view network, binapi::currency_t amount, std::string_view orderId, std::string_view addressTag, bool transactionFeeFlag, std::string_view name, binapi::SPOT::WalletType wType) -> std::string
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "coin", VIEW_INIT(coin) },
        { "address", VIEW_INIT(address) },
        { "amount", fmt::to_string(amount) },
        { "network", VIEW_INIT(network) },
        { "transactionFeeFlag", fmt::to_string(transactionFeeFlag) },
        { "walletType", fmt::to_string(static_cast<int>(wType)) },
    };

    if (!orderId.empty())
        parameters.emplace_back("withdrawOrderId", S_VIEW_INIT(orderId));

    if (!addressTag.empty())
        parameters.emplace_back("addressTag", S_VIEW_INIT(addressTag));

    if (!name.empty())
    {
        std::string name_ { name };
        const std::string_view replace { "%20" };
        auto space = name_.find(' ', 0);
        while (space != std::string::npos)
        {
            name_.replace(space, 1, replace);
            space = name_.find(' ', space);
        }
        parameters.emplace_back("name", "name_");
    }

    auto doc = apiRequest(g_spotRequests[WITHDRAW], true);

    return JTO_STRING(doc, "id");
}
auto binapi::BinanceAPISpot::depositHistory(binapi::v_asset_t coin, uint64_t limit, uint64_t offset, uint64_t startTime, uint64_t endTime, binapi::SPOT::TransactionDepositHistoryStatus status) -> std::vector<SPOT::DepositHistory>
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "coin", VIEW_INIT(coin) },
        { "limit", fmt::to_string(limit) },
        { "offset", fmt::to_string(offset) }
    };
    if (startTime)
        parameters.emplace_back("startTime", fmt::to_string(startTime));
    if (endTime)
        parameters.emplace_back("startTime", fmt::to_string(endTime));

    if (status != binapi::SPOT::TransactionDepositHistoryStatus::undefined)
        parameters.emplace_back("status", fmt::to_string(static_cast<int>(status)));

    auto doc = apiRequest(g_spotRequests[DEPOSIT_HISTORY], parameters, true);

    std::vector<SPOT::DepositHistory> vdh;
    vdh.reserve(limit);

    for (const auto &hist : doc.GetArray())
    {
        SPOT::DepositHistory dh {
            .coin          = JTO_STRING(hist, "coin"),
            .address       = JTO_STRING(hist, "address"),
            .addressTag    = JTO_STRING(hist, "addressTag"),
            .txId          = JTO_STRING(hist, "txId"),
            .network       = JTO_STRING(hist, "network"),
            .unlockConfirm = JTO_STRING(hist, "unlockConfirm"),
            .confirmTimes  = JTO_STRING(hist, "confirmTimes"),
            .amount        = JTO_DOB(hist, "amount"),
            .insertTime    = hist["insertTime"].GetUint64(),
            .status        = [&hist]() {
                switch (hist["status"].GetInt64())
                {
                    case 0: return SPOT::TransactionDepositHistoryStatus::pending;
                    case 1: return SPOT::TransactionDepositHistoryStatus::success;
                    case 6: return SPOT::TransactionDepositHistoryStatus::credited;
                    default: return SPOT::TransactionDepositHistoryStatus::undefined;
                }
            }(),
            .transferType = [&hist]() {
                switch (hist["status"].GetInt64())
                {
                    case 0: return SPOT::TransferType::external;
                    case 1: return SPOT::TransferType::internal;
                    default: return SPOT::TransferType::undefined;
                }
            }(),
        };
        vdh.emplace_back(dh);
    }

    return vdh;
}

auto binapi::BinanceAPISpot::withdrawHistory(binapi::v_asset_t coin, uint64_t limit, uint64_t offset, uint64_t startTime, uint64_t endTime, binapi::SPOT::TransactionWithdrawHistoryStatus status) -> std::vector<SPOT::WithdrawHistory>
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "coin", VIEW_INIT(coin) },
        { "limit", fmt::to_string(limit) }
    };
    if (offset)
        parameters.emplace_back("offset", fmt::to_string(offset));
    if (startTime)
        parameters.emplace_back("startTime", fmt::to_string(startTime));
    if (endTime)
        parameters.emplace_back("startTime", fmt::to_string(endTime));
    if (status != binapi::SPOT::TransactionWithdrawHistoryStatus::undefined)
        parameters.emplace_back("status", fmt::to_string(static_cast<int>(status)));

    auto doc = apiRequest(g_spotRequests[WITHDRAW_HISTORY], parameters, true);

    std::vector<SPOT::WithdrawHistory> vwh;
    vwh.reserve(limit);

    for (const auto &hist : doc.GetArray())
    {

        SPOT::WithdrawHistory wh {
            .coin            = JTO_STRING(hist, "coin"),
            .address         = JTO_STRING(hist, "address"),
            .applyTime       = JTO_STRING(hist, "applyTime"),
            .id              = JTO_STRING(hist, "id"),
            .withdrawOrderId = [&hist]() {
                using namespace std::string_literals;
                if (hist.FindMember("withdrawOrderId") != hist.MemberEnd())
                    return JTO_STRING(hist, "withdrawOrderId");
                return ""s;
            }(),
            .network        = JTO_STRING(hist, "network"),
            .info           = JTO_STRING(hist, "info"),
            .txId           = JTO_STRING(hist, "txId"),
            .amount         = JTO_DOB(hist, "amount"),
            .transactionFee = JTO_DOB(hist, "transactionFee"),
            .confirmNo      = hist["confirmNo"].GetInt64(),
            .status         = [&hist]() {
                switch (hist["status"].GetInt64())
                {
                    case 0:
                        return SPOT::TransactionWithdrawHistoryStatus::emailSent;
                    case 1:
                        return SPOT::TransactionWithdrawHistoryStatus::cancelled;
                    case 2:
                        return SPOT::TransactionWithdrawHistoryStatus::awaitingApproval;
                    case 3:
                        return SPOT::TransactionWithdrawHistoryStatus::rejected;
                    case 4:
                        return SPOT::TransactionWithdrawHistoryStatus::processing;
                    case 5:
                        return SPOT::TransactionWithdrawHistoryStatus::failure;
                    case 6:
                        return SPOT::TransactionWithdrawHistoryStatus::completed;
                    default:
                        return SPOT::TransactionWithdrawHistoryStatus::undefined;
                }
            }(),
            .transferType = [&hist]() {
                switch (hist["status"].GetInt64())
                {
                    case 0: return SPOT::TransferType::external;
                    case 1: return SPOT::TransferType::internal;
                    default: return SPOT::TransferType::undefined;
                }
            }(),
        };
    }

    for (const auto &hist : doc.GetArray())
    {

        SPOT::WithdrawHistory wh {
            .coin            = JTO_STRING(hist, "coin"),
            .address         = JTO_STRING(hist, "address"),
            .applyTime       = JTO_STRING(hist, "applyTime"),
            .id              = JTO_STRING(hist, "id"),
            .withdrawOrderId = [&hist]() {
                using namespace std::string_literals;
                if (hist.FindMember("withdrawOrderId") != hist.MemberEnd())
                    return JTO_STRING(hist, "withdrawOrderId");
                return ""s;
            }(),
            .network        = JTO_STRING(hist, "network"),
            .info           = JTO_STRING(hist, "info"),
            .txId           = JTO_STRING(hist, "txId"),
            .amount         = JTO_DOB(hist, "amount"),
            .transactionFee = JTO_DOB(hist, "transactionFee"),
            .confirmNo      = hist["confirmNo"].GetInt64(),
            .status         = [&hist]() {
                switch (hist["status"].GetInt64())
                {
                    case 0:
                        return SPOT::TransactionWithdrawHistoryStatus::emailSent;
                    case 1:
                        return SPOT::TransactionWithdrawHistoryStatus::cancelled;
                    case 2:
                        return SPOT::TransactionWithdrawHistoryStatus::awaitingApproval;
                    case 3:
                        return SPOT::TransactionWithdrawHistoryStatus::rejected;
                    case 4:
                        return SPOT::TransactionWithdrawHistoryStatus::processing;
                    case 5:
                        return SPOT::TransactionWithdrawHistoryStatus::failure;
                    case 6:
                        return SPOT::TransactionWithdrawHistoryStatus::completed;
                    default:
                        return SPOT::TransactionWithdrawHistoryStatus::undefined;
                }
            }(),
            .transferType = [&hist]() {
                switch (hist["status"].GetInt64())
                {
                    case 0: return SPOT::TransferType::external;
                    case 1: return SPOT::TransferType::internal;
                    default: return SPOT::TransferType::undefined;
                }
            }(),
        };
    }

    return vwh;
}

auto binapi::BinanceAPISpot::withdrawHistory(std::string_view clientOrderId, binapi::v_asset_t coin, uint64_t limit, uint64_t offset, uint64_t startTime, uint64_t endTime, binapi::SPOT::TransactionWithdrawHistoryStatus status) -> std::vector<SPOT::WithdrawHistory>
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "coin", VIEW_INIT(coin) },
        { "limit", fmt::to_string(limit) },
        { "withdrawOrderId", VIEW_INIT(clientOrderId) }
    };
    if (offset)
        parameters.emplace_back("offset", fmt::to_string(offset));
    if (startTime)
        parameters.emplace_back("startTime", fmt::to_string(startTime));
    if (endTime)
        parameters.emplace_back("endTime", fmt::to_string(endTime));
    if (status != binapi::SPOT::TransactionWithdrawHistoryStatus::undefined)
        parameters.emplace_back("status", fmt::to_string(static_cast<int>(status)));

    auto doc = apiRequest(g_spotRequests[WITHDRAW_HISTORY], parameters, true);

    std::vector<SPOT::WithdrawHistory> vwh;
    vwh.reserve(limit);

    return vwh;
}
auto binapi::BinanceAPISpot::depositAddress(binapi::v_asset_t coin, std::string_view network) -> binapi::SPOT::DepositAddress
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "coin", VIEW_INIT(coin) }
    };
    if (!network.empty())
        parameters.emplace_back("network", network);

    auto doc = apiRequest(g_spotRequests[DEPOSIT_ADDRESS], true);

    return binapi::SPOT::DepositAddress {
        .address = JTO_STRING(doc, "address"),
        .coin    = JTO_STRING(doc, "coin"),
        .tag     = JTO_STRING(doc, "tag"),
        .url     = JTO_STRING(doc, "url")
    };
}

auto binapi::BinanceAPISpot::accountStatus() -> std::string
{
    const eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    auto doc = apiRequest(g_spotRequests[ACCOUNT_STATUS], true);

    return JTO_STRING(doc, "id");
}

auto binapi::BinanceAPISpot::accountAPITradingStatus() -> binapi::SPOT::AccountAPITradingStatus
{
    const eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    auto doc = apiRequest(g_spotRequests[ACCOUNT_API_TRADING_STATUS], true);

    return binapi::SPOT::AccountAPITradingStatus {
        .updateTime  = doc["data"]["updateTime"].GetUint64(),
        .recoverTime = doc["data"]["plannedRecoverTime"].GetUint64(),
        .triggerGCR  = doc["data"]["triggerCondition"]["GCR"].GetInt64(),
        .triggerIFER = doc["data"]["triggerCondition"]["IFER"].GetInt64(),
        .triggerUFR  = doc["data"]["triggerCondition"]["UFR"].GetInt64(),
        .isLocked    = doc["data"]["isLocked"].GetBool()
    };
}
auto binapi::BinanceAPISpot::dustLog(uint64_t startTime, uint64_t endTime) -> SPOT::DustLog
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    if (startTime)
        parameters.emplace_back("startTime", fmt::to_string(startTime));
    if (endTime)
        parameters.emplace_back("endTime", fmt::to_string(endTime));

    auto doc = apiRequest(g_spotRequests[DUSTLOG], true);

    SPOT::DustLog dl { .totalCounts = doc["total"].GetInt64() };

    for (const auto &uad : doc["userAssetDribblets"].GetArray())
    {
        SPOT::DustLog::UserAssetDribblets dluad {
            .operateTime              = uad["operateTime"].GetUint64(),
            .transId                  = uad["transId"].GetUint64(),
            .totalTransferedAmount    = JTO_DOB(uad, "totalTransferedAmount"),
            .totalServiceChargeAmount = JTO_DOB(uad, "totalServiceChargeAmount")
        };

        for (const auto &uadd : uad["userAssetDribbletDetails"].GetArray())
        {
            SPOT::DustLog::UserAssetDribbletDetails dluadd {
                .fromAsset           = JTO_STRING(uadd, "fromAsset"),
                .transId             = uadd["transId"].GetUint64(),
                .operateTime         = uadd["operateTime"].GetUint64(),
                .serviceChargeAmount = JTO_DOB(uadd, "serviceChargeAmount"),
                .amount              = JTO_DOB(uadd, "amount"),
                .transferedAmount    = JTO_DOB(uadd, "transferedAmount")
            };

            dluad.dribbletDetails.emplace_back(dluadd);
        }

        dl.dribblets.emplace_back(dluad);
    }

    return dl;
}

auto binapi::BinanceAPISpot::getAssetsConvertedToBNB() -> SPOT::AssetsToBNB
{
    const eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    auto doc = apiRequest(g_spotRequests[ASSETS_CAN_BE_CONVERTED], true);

    SPOT::AssetsToBNB atb{
        .totalTransferBTC = JTO_DOB(doc, "totalTransferBtc"),
        .totalTransferBNB = JTO_DOB(doc, "totalTransferBNB"),
        .dribbletPercentage = JTO_DOB(doc, "dribbletPercentage")
    };

    for( const auto &det : doc["details"].GetArray())
    {
        SPOT::AssetsToBNB::Details details{
            .asset = JTO_STRING(det, "asset"),
            .fullName = JTO_STRING(det, "assetFullName"),
            .amountFree = JTO_DOB(det, "amountFree"),
            .toBTC = JTO_DOB(det, "toBTC"),
            .toBNB = JTO_DOB(det, "toBNB"),
            .toBNBOfExchange = JTO_DOB(det, "toBNBOffExchange"),
            .exchange = JTO_DOB(det, "exchange")
        };

        atb.details.emplace_back(details);
    }

    return atb;
}

auto binapi::BinanceAPISpot::dustTransfer(const std::vector<asset_t> &assets) -> SPOT::DustTransfer
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    for( const auto &asset : assets)
        parameters.emplace_back("asset", asset);

    auto doc = apiRequest(g_spotRequests[DUST_TRANSFER], true);

    SPOT::DustTransfer dtf{
        .totalServiceCharge = JTO_DOB(doc, "totalServiceCharge"),
        .totalTransfered = JTO_DOB(doc, "totalTransfered")
    };

    for( const auto &res : doc["transferResult"].GetArray())
    {
        SPOT::DustTransfer::TransferResult tres{
            .fromAsset = JTO_STRING(res, "fromAsset"),
            .tranId = res["tranId"].GetUint64(),
            .operateTime = res["operateTime"].GetUint64(),
            .amount = JTO_DOB(res, "amount"),
            .serviceChargeAmount = JTO_DOB(res, "serviceChargeAmount"),
            .transferedAmount = JTO_DOB(res, "transferedAmount")
        };

        dtf.transferResult.emplace_back(tres);
    }

    return dtf;
}
