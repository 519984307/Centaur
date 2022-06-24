/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 21/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "SpotStreams.hpp"
#include "ApplicationMessages.hpp"
#include "TraderApplication.hpp"

btrader::stream::SpotStreams::SpotStreams() :
    BINAPI_NAMESPACE::ws::WSSpotBinanceAPI("")
{
}

btrader::stream::SpotStreams::SpotStreams(TraderApplication *app, const std::string &base, const std::string &quote) :
    BINAPI_NAMESPACE::ws::WSSpotBinanceAPI(""),
    m_app { app }
{
    auto data     = subscribeIndividualMiniTicker(base + quote);
    auto endpoint = BINAPI_NAMESPACE::ws::WSSpotBinanceAPI::makeEndPoint(std::get<0>(data));
    setEndPoint(endpoint);
}

auto btrader::stream::SpotStreams::connected() -> void
{
    logInfo("SPOT Streams are connected.");
    m_started.set_value();
}

auto btrader::stream::SpotStreams::close() -> void
{
    logError("SPOT Streams are disconnected");
}

auto btrader::stream::SpotStreams::connectionError() -> void
{
    logError("SPOT Streams could not connect with the Binance Servers");
    m_started.set_value();
}

void btrader::stream::SpotStreams::pingSent(const bool &success) noexcept
{
    if (success)
        logSuccess("SPOT Streams ping send");
    else
        logFailure("SPOT Streams ping was not send");
}

void btrader::stream::SpotStreams::individualSymbolMiniTicker(const std::string &symbol, C_UNUSED const uint64_t &eventTime, const BINAPI_NAMESPACE::StreamIndividualSymbolMiniTicker &ticker)
{
    std::string asset = symbol.substr(0, symbol.size() - 4); // We can be for sure that every symbol has the USDT as quote
    // The application has the ids on upper case
    std::transform(asset.begin(), asset.end(), asset.begin(), ::toupper);
    m_app->sendMessage<TickerUpdate>(asset, ticker.closePrice);
}
