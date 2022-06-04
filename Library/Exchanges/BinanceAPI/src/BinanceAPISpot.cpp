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
    // clang-format off
    BINAPI_WARN_PUSH
    BINAPI_WARN_OFF(exit-time-destructors)
    BINAPI_WARN_OFF(global-constructors)
    // clang-format on
#endif /*defined(__clang__) || defined(__GNUC__)*/

    using namespace BINAPI_NAMESPACE::local;
    const std::array<BinanceAPIRequest, 30> g_spotRequests {
        {
            // Wallet Endpoints
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
            // Market Data Endpoints
            { 1, "/api/v3/ping", HTTPRequest::GET, false, loadSchema(schemas::SPOT::test_connectivity), loadSchema(schemas::api_error), false, false },
            { 1, "/api/v3/time", HTTPRequest::GET, false, loadSchema(schemas::SPOT::check_server_time), loadSchema(schemas::api_error), false, false },
            { 10, "/api/v3/exchangeInfo", HTTPRequest::GET, false, loadSchema(schemas::SPOT::exchange_info), loadSchema(schemas::api_error), false, false },
            { 1, "/api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::SPOT::order_book), loadSchema(schemas::api_error), false, false },
            { 5, "/api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::SPOT::order_book), loadSchema(schemas::api_error), false, false },
            { 10, "/api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::SPOT::order_book), loadSchema(schemas::api_error), false, false },
            { 50, "/api/v3/depth", HTTPRequest::GET, false, loadSchema(schemas::SPOT::order_book), loadSchema(schemas::api_error), false, false },
        }
    };

#if defined(__clang__) || defined(__GNUC__)
    BINAPI_WARN_POP
#endif /*defined(__clang__) || defined(__GNUC__)*/

    enum SPOTRequests
    {
        // Wallet Endpoints
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
        // Market Data Endpoints
        TEST_CONNECTIVITY,
        CHECK_SERVER_TIME,
        EXCHANGE_INFORMATION,
        ORDER_BOOK_1,
        ORDER_BOOK_5,
        ORDER_BOOK_10,
        ORDER_BOOK_50,
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

#define JTO_STRING(x, y)                         \
    std::string                                  \
    {                                            \
        x[y].GetString(), x[y].GetStringLength() \
    }

#define JTO_DOB(x, y) \
    std::stod(JTO_STRING(x, y))

auto binapi::BinanceAPISpot::getAllCoinsInformation() -> std::unordered_map<sym_t, SPOT::CoinInformation>
{
    auto doc = apiRequest(g_spotRequests[ALL_COINS_INFORMATION],
        { { "recvWindow", fmt::to_string(getRecvWindow()) },
            { "timestamp", fmt::to_string(getTime()) } },
        true);
    std::unordered_map<sym_t, SPOT::CoinInformation> info;

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
                .memoRegEx   = JTO_STRING(list, "memoRegex"),
                .name        = JTO_STRING(list, "name"),
                .network     = JTO_STRING(list, "network"),
                .specialTips = [&]() -> std::string {
                    auto iter = list.FindMember("specialTips");
                    if (iter != list.MemberEnd())
                        return JTO_STRING(list, "specialTips");
                    return "";
                }(),
                .withdrawDescription = [&list]() -> std::string {
                    if (!list["withdrawEnable"].GetBool())
                        return JTO_STRING(list, "withdrawDesc");
                    else
                        return "";
                }(),
                .specialWithdrawTips = [&]() -> std::string {
                    auto iter = list.FindMember("specialWithdrawTips");
                    if (iter != list.MemberEnd())
                        return JTO_STRING(list, "specialWithdrawTips");
                    return "";
                }(),
                .addressRule             = JTO_STRING(list, "addressRule"),
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

        info[cif.coin] = cif;
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

    SPOT::AssetsToBNB atb {
        .totalTransferBTC   = JTO_DOB(doc, "totalTransferBtc"),
        .totalTransferBNB   = JTO_DOB(doc, "totalTransferBNB"),
        .dribbletPercentage = JTO_DOB(doc, "dribbletPercentage")
    };

    for (const auto &det : doc["details"].GetArray())
    {
        SPOT::AssetsToBNB::Details details {
            .asset           = JTO_STRING(det, "asset"),
            .fullName        = JTO_STRING(det, "assetFullName"),
            .amountFree      = JTO_DOB(det, "amountFree"),
            .toBTC           = JTO_DOB(det, "toBTC"),
            .toBNB           = JTO_DOB(det, "toBNB"),
            .toBNBOfExchange = JTO_DOB(det, "toBNBOffExchange"),
            .exchange        = JTO_DOB(det, "exchange")
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

    for (const auto &asset : assets)
        parameters.emplace_back("asset", asset);

    auto doc = apiRequest(g_spotRequests[DUST_TRANSFER], true);

    SPOT::DustTransfer dtf {
        .totalServiceCharge = JTO_DOB(doc, "totalServiceCharge"),
        .totalTransfered    = JTO_DOB(doc, "totalTransfered")
    };

    for (const auto &res : doc["transferResult"].GetArray())
    {
        SPOT::DustTransfer::TransferResult tres {
            .fromAsset           = JTO_STRING(res, "fromAsset"),
            .tranId              = res["tranId"].GetUint64(),
            .operateTime         = res["operateTime"].GetUint64(),
            .amount              = JTO_DOB(res, "amount"),
            .serviceChargeAmount = JTO_DOB(res, "serviceChargeAmount"),
            .transferedAmount    = JTO_DOB(res, "transferedAmount")
        };

        dtf.transferResult.emplace_back(tres);
    }

    return dtf;
}

auto binapi::BinanceAPISpot::assetDividendRecord(binapi::v_asset_t asset, uint64_t limit, uint64_t startTime, uint64_t endTime) -> std::pair<int64_t, std::vector<SPOT::AssetDividend>>
{
    assert(limit > 500);
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    if (startTime > 0)
        parameters.emplace_back("startTime", fmt::to_string(startTime));
    if (endTime > 0)
        parameters.emplace_back("endTime", fmt::to_string(endTime));
    if (!asset.empty())
        parameters.emplace_back("asset", asset);

    auto doc = apiRequest(g_spotRequests[ASSET_DIVIDEND_RECORD], true);

    std::pair<int64_t, std::vector<SPOT::AssetDividend>> piva;
    piva.first = doc["total"].GetInt64();

    for (const auto &row : doc["rows"].GetArray())
    {
        SPOT::AssetDividend adiv {
            .asset   = JTO_STRING(row, "asset"),
            .id      = row["id"].GetUint64(),
            .divTime = row["divTime"].GetUint64(),
            .transId = row["tranId"].GetUint64(),
            .amount  = JTO_DOB(row, "amount"),
            .enInfo  = JTO_STRING(row, "enInfo")
        };
        piva.second.push_back(adiv);
    }

    return piva;
}
auto binapi::BinanceAPISpot::assetDetail(binapi::v_asset_t asset) -> binapi::SPOT::AssetDetail
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    parameters.emplace_back("asset", asset);

    auto doc = apiRequest(g_spotRequests[ASSET_DETAIL], true);

    SPOT::AssetDetail adl;

    auto ctr                        = doc["CTR"].GetObject();
    auto sky                        = doc["SKY"].GetObject();

    adl.ctrDetail.depositTip        = JTO_STRING(ctr, "depositTip");
    adl.ctrDetail.withdrawFee       = ctr["withdrawFee"].GetDouble();
    adl.ctrDetail.minWithdrawAmount = JTO_DOB(ctr, "minWithdrawAmount");
    adl.ctrDetail.depositStatus     = ctr["depositStatus"].GetBool();
    adl.ctrDetail.withdrawStatus    = ctr["withdrawStatus"].GetBool();

    adl.skyDetail.minWithdrawAmount = JTO_DOB(sky, "minWithdrawAmount");
    adl.skyDetail.withdrawFee       = sky["withdrawFee"].GetDouble();
    adl.skyDetail.depositStatus     = sky["depositStatus"].GetBool();
    adl.skyDetail.withdrawStatus    = sky["withdrawStatus"].GetBool();

    return adl;
}

auto binapi::BinanceAPISpot::tradeFee(binapi::v_sym_t symbol) -> std::map<sym_t, SPOT::TradeFee>
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    parameters.emplace_back("symbol", symbol);

    auto doc = apiRequest(g_spotRequests[TRADE_FEE], true);

    std::map<sym_t, SPOT::TradeFee> mstf;

    for (const auto &fee : doc.GetArray())
    {
        sym_t sym = JTO_STRING(fee, "symbol");
        SPOT::TradeFee tf {
            .makerCommission = JTO_DOB(fee, "makerCommission"),
            .takerCommission = JTO_DOB(fee, "takerCommission")
        };
        mstf[sym] = tf;
    }

    return mstf;
}

auto binapi::BinanceAPISpot::userUniversalTransfer(binapi::SPOT::UniversalTransferType type, binapi::v_asset_t asset, binapi::currency_t amount, int64_t precision, binapi::v_sym_t fromSymbol, binapi::v_sym_t toSymbol) -> uint64_t
{
    eparams parameters {
        { "asset", std::string(asset) },
        { "amount", fmt::format("{:{}}", amount, precision) },
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    switch (type)
    {
        case SPOT::UniversalTransferType::MAIN_UMFUTURE: parameters.emplace_back("type", "MAIN_UMFUTURE"); break;
        case SPOT::UniversalTransferType::MAIN_CMFUTURE: parameters.emplace_back("type", "MAIN_CMFUTURE"); break;
        case SPOT::UniversalTransferType::MAIN_MARGIN: parameters.emplace_back("type", "MAIN_MARGIN"); break;
        case SPOT::UniversalTransferType::UMFUTURE_MAIN: parameters.emplace_back("type", "UMFUTURE_MAIN"); break;
        case SPOT::UniversalTransferType::UMFUTURE_MARGIN: parameters.emplace_back("type", "UMFUTURE_MARGIN"); break;
        case SPOT::UniversalTransferType::CMFUTURE_MAIN: parameters.emplace_back("type", "CMFUTURE_MAIN"); break;
        case SPOT::UniversalTransferType::CMFUTURE_MARGIN: parameters.emplace_back("type", "CMFUTURE_MARGIN"); break;
        case SPOT::UniversalTransferType::MARGIN_MAIN: parameters.emplace_back("type", "MARGIN_MAIN"); break;
        case SPOT::UniversalTransferType::MARGIN_UMFUTURE: parameters.emplace_back("type", "MARGIN_UMFUTURE"); break;
        case SPOT::UniversalTransferType::MARGIN_CMFUTURE: parameters.emplace_back("type", "MARGIN_CMFUTURE"); break;
        case SPOT::UniversalTransferType::ISOLATEDMARGIN_MARGIN:
            parameters.emplace_back("type", "ISOLATEDMARGIN_MARGIN");
            parameters.emplace_back("fromSymbol", fromSymbol);
            break;
        case SPOT::UniversalTransferType::MARGIN_ISOLATEDMARGIN:
            parameters.emplace_back("type", "MARGIN_ISOLATEDMARGIN");
            parameters.emplace_back("toSymbol", toSymbol);
            break;
        case SPOT::UniversalTransferType::ISOLATEDMARGIN_ISOLATEDMARGIN:
            parameters.emplace_back("type", "ISOLATEDMARGIN_ISOLATEDMARGIN");
            parameters.emplace_back("fromSymbol", fromSymbol);
            parameters.emplace_back("toSymbol", toSymbol);
            break;
        case SPOT::UniversalTransferType::MAIN_FUNDING: parameters.emplace_back("type", "MAIN_FUNDING"); break;
        case SPOT::UniversalTransferType::FUNDING_MAIN: parameters.emplace_back("type", "FUNDING_MAIN"); break;
        case SPOT::UniversalTransferType::FUNDING_UMFUTURE: parameters.emplace_back("type", "FUNDING_UMFUTURE"); break;
        case SPOT::UniversalTransferType::UMFUTURE_FUNDING: parameters.emplace_back("type", "UMFUTURE_FUNDING"); break;
        case SPOT::UniversalTransferType::MARGIN_FUNDING: parameters.emplace_back("type", "MARGIN_FUNDING"); break;
        case SPOT::UniversalTransferType::FUNDING_MARGIN: parameters.emplace_back("type", "FUNDING_MARGIN"); break;
        case SPOT::UniversalTransferType::FUNDING_CMFUTURE: parameters.emplace_back("type", "FUNDING_CMFUTURE"); break;
        case SPOT::UniversalTransferType::CMFUTURE_FUNDING: parameters.emplace_back("type", "CMFUTURE_FUNDING"); break;
    }

    auto doc = apiRequest(g_spotRequests[USER_UNIVERSAL_TRANSFER], true);

    return doc["tranId"].GetUint64();
}

auto binapi::BinanceAPISpot::queryUserUniversalTransferHistory(binapi::SPOT::UniversalTransferType type, binapi::v_sym_t fromSymbol, binapi::v_sym_t toSymbol, int64_t current, int64_t size, uint64_t startTime, uint64_t endTime) -> std::pair<int64_t, std::vector<SPOT::UniversalTransferQuery>>
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
        { "current", fmt::to_string(current) },
        { "size", fmt::to_string(size) }
    };

    if (startTime > 0)
        parameters.emplace_back("startTime", fmt::to_string(startTime));
    if (endTime > 0)
        parameters.emplace_back("endTime", fmt::to_string(endTime));

    switch (type)
    {
        case SPOT::UniversalTransferType::MAIN_UMFUTURE: parameters.emplace_back("type", "MAIN_UMFUTURE"); break;
        case SPOT::UniversalTransferType::MAIN_CMFUTURE: parameters.emplace_back("type", "MAIN_CMFUTURE"); break;
        case SPOT::UniversalTransferType::MAIN_MARGIN: parameters.emplace_back("type", "MAIN_MARGIN"); break;
        case SPOT::UniversalTransferType::UMFUTURE_MAIN: parameters.emplace_back("type", "UMFUTURE_MAIN"); break;
        case SPOT::UniversalTransferType::UMFUTURE_MARGIN: parameters.emplace_back("type", "UMFUTURE_MARGIN"); break;
        case SPOT::UniversalTransferType::CMFUTURE_MAIN: parameters.emplace_back("type", "CMFUTURE_MAIN"); break;
        case SPOT::UniversalTransferType::CMFUTURE_MARGIN: parameters.emplace_back("type", "CMFUTURE_MARGIN"); break;
        case SPOT::UniversalTransferType::MARGIN_MAIN: parameters.emplace_back("type", "MARGIN_MAIN"); break;
        case SPOT::UniversalTransferType::MARGIN_UMFUTURE: parameters.emplace_back("type", "MARGIN_UMFUTURE"); break;
        case SPOT::UniversalTransferType::MARGIN_CMFUTURE: parameters.emplace_back("type", "MARGIN_CMFUTURE"); break;
        case SPOT::UniversalTransferType::ISOLATEDMARGIN_MARGIN:
            parameters.emplace_back("type", "ISOLATEDMARGIN_MARGIN");
            parameters.emplace_back("fromSymbol", fromSymbol);
            break;
        case SPOT::UniversalTransferType::MARGIN_ISOLATEDMARGIN:
            parameters.emplace_back("type", "MARGIN_ISOLATEDMARGIN");
            parameters.emplace_back("toSymbol", toSymbol);
            break;
        case SPOT::UniversalTransferType::ISOLATEDMARGIN_ISOLATEDMARGIN:
            parameters.emplace_back("type", "ISOLATEDMARGIN_ISOLATEDMARGIN");
            parameters.emplace_back("fromSymbol", fromSymbol);
            parameters.emplace_back("toSymbol", toSymbol);
            break;
        case SPOT::UniversalTransferType::MAIN_FUNDING: parameters.emplace_back("type", "MAIN_FUNDING"); break;
        case SPOT::UniversalTransferType::FUNDING_MAIN: parameters.emplace_back("type", "FUNDING_MAIN"); break;
        case SPOT::UniversalTransferType::FUNDING_UMFUTURE: parameters.emplace_back("type", "FUNDING_UMFUTURE"); break;
        case SPOT::UniversalTransferType::UMFUTURE_FUNDING: parameters.emplace_back("type", "UMFUTURE_FUNDING"); break;
        case SPOT::UniversalTransferType::MARGIN_FUNDING: parameters.emplace_back("type", "MARGIN_FUNDING"); break;
        case SPOT::UniversalTransferType::FUNDING_MARGIN: parameters.emplace_back("type", "FUNDING_MARGIN"); break;
        case SPOT::UniversalTransferType::FUNDING_CMFUTURE: parameters.emplace_back("type", "FUNDING_CMFUTURE"); break;
        case SPOT::UniversalTransferType::CMFUTURE_FUNDING: parameters.emplace_back("type", "CMFUTURE_FUNDING"); break;
    }

    std::pair<int64_t, std::vector<SPOT::UniversalTransferQuery>> pisutq;

    auto doc     = apiRequest(g_spotRequests[QUERY_USER_UNIVERSAL_TRANSFER], true);

    pisutq.first = doc["total"].GetInt64();

    for (const auto &row : doc["rows"].GetArray())
    {
        SPOT::UniversalTransferQuery utq {
            .asset     = JTO_STRING(row, "asset"),
            .status    = JTO_STRING(row, "status"),
            .timestamp = row["timestamp"].GetUint64(),
            .tranId    = row["tranId"].GetUint64(),
            .amount    = JTO_DOB(row, "amount"),
            .type      = [type = JTO_STRING(row, "type")]() -> SPOT::UniversalTransferType {
                if (type == "MAIN_UMFUTURE")
                    return SPOT::UniversalTransferType::MAIN_UMFUTURE;
                else if (type == "MAIN_CMFUTURE")
                    return SPOT::UniversalTransferType::MAIN_CMFUTURE;
                else if (type == "MAIN_MARGIN")
                    return SPOT::UniversalTransferType::MAIN_MARGIN;
                else if (type == "UMFUTURE_MAIN")
                    return SPOT::UniversalTransferType::UMFUTURE_MAIN;
                else if (type == "UMFUTURE_MARGIN")
                    return SPOT::UniversalTransferType::UMFUTURE_MARGIN;
                else if (type == "CMFUTURE_MAIN")
                    return SPOT::UniversalTransferType::CMFUTURE_MAIN;
                else if (type == "CMFUTURE_MARGIN")
                    return SPOT::UniversalTransferType::CMFUTURE_MARGIN;
                else if (type == "MARGIN_MAIN")
                    return SPOT::UniversalTransferType::MARGIN_MAIN;
                else if (type == "MARGIN_UMFUTURE")
                    return SPOT::UniversalTransferType::MARGIN_UMFUTURE;
                else if (type == "MARGIN_CMFUTURE")
                    return SPOT::UniversalTransferType::MARGIN_CMFUTURE;
                else if (type == "ISOLATEDMARGIN_MARGIN")
                    return SPOT::UniversalTransferType::ISOLATEDMARGIN_MARGIN;
                else if (type == "MARGIN_ISOLATEDMARGIN")
                    return SPOT::UniversalTransferType::MARGIN_ISOLATEDMARGIN;
                else if (type == "ISOLATEDMARGIN_ISOLATEDMARGIN")
                    return SPOT::UniversalTransferType::ISOLATEDMARGIN_ISOLATEDMARGIN;
                else if (type == "MAIN_FUNDING")
                    return SPOT::UniversalTransferType::MAIN_FUNDING;
                else if (type == "FUNDING_MAIN")
                    return SPOT::UniversalTransferType::FUNDING_MAIN;
                else if (type == "FUNDING_UMFUTURE")
                    return SPOT::UniversalTransferType::FUNDING_UMFUTURE;
                else if (type == "UMFUTURE_FUNDING")
                    return SPOT::UniversalTransferType::UMFUTURE_FUNDING;
                else if (type == "MARGIN_FUNDING")
                    return SPOT::UniversalTransferType::MARGIN_FUNDING;
                else if (type == "FUNDING_MARGIN")
                    return SPOT::UniversalTransferType::FUNDING_MARGIN;
                else if (type == "FUNDING_CMFUTURE")
                    return SPOT::UniversalTransferType::FUNDING_CMFUTURE;
                else // if (type == "CMFUTURE_FUNDING")
                    return SPOT::UniversalTransferType::CMFUTURE_FUNDING;
            }()
        };
        pisutq.second.push_back(utq);
    }

    return pisutq;
}

auto binapi::BinanceAPISpot::fundingWallet(binapi::v_asset_t asset, bool needBtcValuation) -> std::vector<SPOT::FundingWallet>
{
    eparams parameters {
        { "asset", std::string(asset) },
        { "needBtcValuation", needBtcValuation ? "true" : "false" },
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    auto doc = apiRequest(g_spotRequests[FUNDING_WALLET], true);

    std::vector<SPOT::FundingWallet> fwlt;
    for (const auto &fund : doc.GetArray())
    {
        SPOT::FundingWallet fwl {
            .asset        = JTO_STRING(fund, "asset"),
            .free         = JTO_DOB(fund, "free"),
            .locked       = JTO_DOB(fund, "locked"),
            .freeze       = JTO_DOB(fund, "freeze"),
            .withdrawing  = JTO_DOB(fund, "withdrawing"),
            .btcValuation = JTO_DOB(fund, "btcValuation")
        };

        fwlt.push_back(fwl);
    }

    return fwlt;
}

auto binapi::BinanceAPISpot::fundingWallet(bool needBtcValuation) -> std::vector<SPOT::FundingWallet>
{
    eparams parameters {
        { "needBtcValuation", needBtcValuation ? "true" : "false" },
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    auto doc = apiRequest(g_spotRequests[FUNDING_WALLET], true);

    std::vector<SPOT::FundingWallet> fwlt;
    for (const auto &fund : doc.GetArray())
    {
        SPOT::FundingWallet fwl {
            .asset        = JTO_STRING(fund, "asset"),
            .free         = JTO_DOB(fund, "free"),
            .locked       = JTO_DOB(fund, "locked"),
            .freeze       = JTO_DOB(fund, "freeze"),
            .withdrawing  = JTO_DOB(fund, "withdrawing"),
            .btcValuation = JTO_DOB(fund, "btcValuation")
        };

        fwlt.push_back(fwl);
    }

    return fwlt;
}

auto binapi::BinanceAPISpot::getAPIKeyPermission() -> binapi::SPOT::APIKeyPermissions
{
    eparams parameters {
        { "recvWindow", fmt::to_string(getRecvWindow()) },
        { "timestamp", fmt::to_string(getTime()) },
    };

    auto doc = apiRequest(g_spotRequests[API_KEY_PERMISSION], true);

    SPOT::APIKeyPermissions akp {
        .tradingAuthorityExpirationTime = doc["tradingAuthorityExpirationTime"].GetUint64(),
        .createTime                     = doc["createTime"].GetUint64(),
        .ipRestrict                     = doc["ipRestrict"].GetBool(),
        .enableWithdrawals              = doc["enableWithdrawals"].GetBool(),
        .enableInternalTransfer         = doc["enableInternalTransfer"].GetBool(),
        .permitsUniversalTransfer       = doc["permitsUniversalTransfer"].GetBool(),
        .enableVanillaOptions           = doc["enableVanillaOptions"].GetBool(),
        .enableReading                  = doc["enableReading"].GetBool(),
        .enableFutures                  = doc["enableFutures"].GetBool(),
        .enableMargin                   = doc["enableMargin"].GetBool(),
        .enableSpotAndMarginTrading     = doc["enableSpotAndMarginTrading"].GetBool(),
    };

    return akp;
}
auto binapi::BinanceAPISpot::ping() -> void
{
    apiRequest(g_spotRequests[TEST_CONNECTIVITY]);
}

auto binapi::BinanceAPISpot::checkServerTime() -> uint64_t
{
    auto doc = apiRequest(g_spotRequests[CHECK_SERVER_TIME]);
    return doc["serverTime"].GetUint64();
}

auto binapi::BinanceAPISpot::getExchangeInformation() -> binapi::SPOT::ExchangeInformation
{
    auto doc = apiRequest(g_spotRequests[EXCHANGE_INFORMATION]);

    SPOT::ExchangeInformation exchangeInfo;
    // Parse all data
    exchangeInfo.timezone = std::string { doc["timezone"].GetString(), doc["timezone"].GetStringLength() };

    // Extract Limits
    for (const auto &limit : doc["rateLimits"].GetArray())
    {
        std::string type { limit["rateLimitType"].GetString(), limit["rateLimitType"].GetStringLength() };
        std::string interval { limit["interval"].GetString(), limit["interval"].GetStringLength() };
        uint32_t number         = limit["intervalNum"].GetUint();
        uint32_t rateLimit      = limit["limit"].GetUint();

        uint32_t intervalNumber = 0;

        if (interval == "MINUTE")
            intervalNumber = 60;
        else if (interval == "SECOND")
            intervalNumber = 1;
        else if (interval == "DAY")
            intervalNumber = 24 * 60 * 60;

        if (type == "REQUEST_WEIGHT")
        {
            exchangeInfo.limitWeight.seconds = intervalNumber * number;
            exchangeInfo.limitWeight.limit   = rateLimit;
        }
        else if (type == "ORDERS")
        {
            if (intervalNumber == 1)
            {
                // SECOND
                exchangeInfo.limitOrderSecond.seconds = intervalNumber * number;
                exchangeInfo.limitOrderSecond.limit   = rateLimit;
            }
            else
            {
                exchangeInfo.limitOrderDay.seconds = intervalNumber * number;
                exchangeInfo.limitOrderDay.limit   = rateLimit;
            }
        }
        else
        {
            // POSSIBLE BUG IF BINANCE CHANGE THE FORMAT OF THE OUTPUT DATA
        }
    }

    // Symbols
    for (const auto &symbol : doc["symbols"].GetArray())
    {
        SPOT::ExchangeInformationSymbol sym;

        sym.symbol = { symbol["symbol"].GetString(), symbol["symbol"].GetStringLength() };

        // Strings are limited by the Schema
        if (std::string status = symbol["status"].GetString(); status == "TRADING")
            sym.status = SPOT::TradingStatus::Trading;
        else
            sym.status = SPOT::TradingStatus::Break;

        sym.baseAsset                = { symbol["baseAsset"].GetString(), symbol["baseAsset"].GetStringLength() };

        sym.baseAssetPrecision       = symbol["baseAssetPrecision"].GetInt();

        sym.quoteAsset               = { symbol["quoteAsset"].GetString(), symbol["quoteAsset"].GetStringLength() };

        sym.quotePrecision           = symbol["quotePrecision"].GetInt();
        sym.quoteAssetPrecision      = symbol["quoteAssetPrecision"].GetInt();
        sym.baseCommissionPrecision  = symbol["baseCommissionPrecision"].GetInt();
        sym.quoteCommissionPrecision = symbol["quoteCommissionPrecision"].GetInt();
        // Order Types
        for (const auto &orderTypes : symbol["orderTypes"].GetArray())
        {
            std::string type = orderTypes.GetString();
            if (type == "LIMIT")
                sym.orderType.set(static_cast<uint64_t>(OrderType::limit));
            else if (type == "LIMIT_MAKER")
                sym.orderType.set(static_cast<uint64_t>(OrderType::limitMaker));
            else if (type == "MARKET")
                sym.orderType.set(static_cast<uint64_t>(OrderType::market));
            else if (type == "STOP_LOSS_LIMIT")
                sym.orderType.set(static_cast<uint64_t>(OrderType::stopLossLimit));
            else if (type == "TAKE_PROFIT_LIMIT")
                sym.orderType.set(static_cast<uint64_t>(OrderType::takeProfitLimit));
            else
            {
                // The schema forbids this scenario
            }
        }
        sym.icebergAllowed             = symbol["icebergAllowed"].GetBool();
        sym.ocoAllowed                 = symbol["ocoAllowed"].GetBool();
        sym.quoteOrderQtyMarketAllowed = symbol["quoteOrderQtyMarketAllowed"].GetBool();
        // Filters
        for (const auto &filters : symbol["filters"].GetArray())
        {
            std::string filterType = filters["filterType"].GetString();

            if (filterType == "PRICE_FILTER")
            {
                sym.eifPrice.present  = true;
                sym.eifPrice.minPrice = std::stod(filters["minPrice"].GetString());
                sym.eifPrice.maxPrice = std::stod(filters["maxPrice"].GetString());
                sym.eifPrice.tickSize = std::stod(filters["tickSize"].GetString());
            }
            else if (filterType == "PERCENT_PRICE")
            {
                sym.eifPercentPrice.present          = true;
                sym.eifPercentPrice.multiplierUp     = std::stod(filters["multiplierUp"].GetString());
                sym.eifPercentPrice.multiplierDown   = std::stod(filters["multiplierDown"].GetString());
                sym.eifPercentPrice.averagePriceMins = filters["avgPriceMins"].GetInt();
            }
            else if (filterType == "LOT_SIZE")
            {
                sym.eifLotSize.present  = true;
                sym.eifLotSize.minQty   = std::stod(filters["minQty"].GetString());
                sym.eifLotSize.maxQty   = std::stod(filters["maxQty"].GetString());
                sym.eifLotSize.stepSize = std::stod(filters["stepSize"].GetString());
            }
            else if (filterType == "MIN_NOTIONAL")
            {
                sym.eifMinNotional.present          = true;
                sym.eifMinNotional.minNotional      = std::stod(filters["minNotional"].GetString());
                sym.eifMinNotional.applyToMarket    = filters["applyToMarket"].GetBool();
                sym.eifMinNotional.averagePriceMins = filters["avgPriceMins"].GetInt();
            }
            else if (filterType == "ICEBERG_PARTS")
            {
                sym.eifIcebergParts.present = true;
                sym.eifIcebergParts.limit   = filters["limit"].GetInt();
            }
            else if (filterType == "MARKET_LOT_SIZE")
            {
                sym.eifMarketLotSize.present  = true;
                sym.eifMarketLotSize.minQty   = std::stod(filters["minQty"].GetString());
                sym.eifMarketLotSize.maxQty   = std::stod(filters["maxQty"].GetString());
                sym.eifMarketLotSize.stepSize = std::stod(filters["stepSize"].GetString());
            }
            else if (filterType == "MAX_NUM_ALGO_ORDERS")
            {
                sym.eifMaxNumAlgoOrders.present          = true;
                sym.eifMaxNumAlgoOrders.maxNumAlgoOrders = filters["maxNumAlgoOrders"].GetInt();
            }
            else if (filterType == "MAX_NUM_ORDERS")
            {
                sym.eifMaxNumOrders.present      = true;
                sym.eifMaxNumOrders.maxNumOrders = filters["maxNumOrders"].GetInt();
            }
            else if (filterType == "MAX_NUM_ICEBERG_ORDERS")
            {
                sym.eifMaxNumIcerbergOrders.present             = true;
                sym.eifMaxNumIcerbergOrders.maxNumIcebergOrders = filters["maxNumIcebergOrders"].GetInt();
            }
            else if (filterType == "MAX_POSITION")
            {
                sym.eifMaxPosition.present     = true;
                sym.eifMaxPosition.maxPosition = std::stod(filters["maxPosition"].GetString());
            }
        }
        // Permissions
        for (const auto &permissions : symbol["permissions"].GetArray())
        {
            std::string type { permissions.GetString(), permissions.GetStringLength() };
            if (type == "SPOT")
                sym.permissions.set(SPOT::AccountPermissions::spot);
            else if (type == "MARGIN")
                sym.permissions.set(SPOT::AccountPermissions::margin);
            else if (type == "LEVERAGED")
                sym.permissions.set(SPOT::AccountPermissions::leveraged);
            else
            {
                // The schema forbids this scenario
            }
        }

        // Add to list
        exchangeInfo.symbols[sym.symbol] = sym;
    }

    return exchangeInfo;
}
auto binapi::BinanceAPISpot::getOrderBook(binapi::v_sym_t symbol, uint32_t limit) -> binapi::SPOT::OrderBook
{
    const eparams parameters {
        { "symbol", VIEW_INIT(symbol) },
        { "limit", limit == 0 ? "1" : (limit > 5000 ? "5000" : fmt::to_string(limit)) }
    };

    rapidjson::Document doc;

    if (limit >= 1 && limit <= 100)
        doc = apiRequest(g_spotRequests[ORDER_BOOK_1], parameters);
    else if (limit >= 101 && limit <= 500)
        doc = apiRequest(g_spotRequests[ORDER_BOOK_5], parameters);
    else if (limit >= 501 && limit <= 1000)
        doc = apiRequest(g_spotRequests[ORDER_BOOK_10], parameters);
    else if (limit >= 1001 && limit <= 5000)
        doc = apiRequest(g_spotRequests[ORDER_BOOK_50], parameters);
    else
        return {};

    SPOT::OrderBook ob;
    ob.lastUpdateId = doc["lastUpdateId"].GetUint64();

    for (const auto &bids : doc["bids"].GetArray())
    {
        ob.bids.push_back(
            { std::stod({ bids[0].GetString(), bids[0].GetStringLength() }),
                std::stod({ bids[1].GetString(), bids[1].GetStringLength() }) });
    }
    for (const auto &asks : doc["asks"].GetArray())
    {
        ob.asks.push_back(
            { std::stod({ asks[0].GetString(), asks[0].GetStringLength() }),
                std::stod({ asks[1].GetString(), asks[1].GetStringLength() }) });
    }

    return ob;
}
