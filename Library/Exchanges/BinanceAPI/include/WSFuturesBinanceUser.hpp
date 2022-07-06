////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef WSFUTURESBINANCEUSER_HPP
#define WSFUTURESBINANCEUSER_HPP

#include "BinanceAPIGlobal.hpp"
//
#include "BinanceAPIDefs.hpp"
#include "WSThread.hpp"

namespace BINAPI_NAMESPACE::ws
{
    /// \brief User data stream WebSocket. Subclass as needed
    /// If the WebSocket expires the WebSocket will be close. In this scenario you can use run() again.
    class BINAPI_EXPORT WSFuturesBinanceUser : public WSThread
    {
    public:
        WSFuturesBinanceUser(const std::string &listenKey, uint64_t pingTimer);
        ~WSFuturesBinanceUser() override;

        virtual void listenKeyExpired(uint64_t eventTime) noexcept = 0;

    protected:
        virtual void eventMarginCall(uint64_t eventTime, const BINAPI_NAMESPACE::UserStreamMarginCallFutures &marginCall) noexcept                                                       = 0;
        virtual void eventBalanceAndPositionUpdate() noexcept                                                                                                                            = 0;
        virtual void eventOrderUpdate(uint64_t eventTime, uint64_t transactionTime, const BINAPI_NAMESPACE::sym_t &symbol, const BINAPI_NAMESPACE::UserStreamOrderUpdate &data) noexcept = 0;
        virtual void eventLeverageUpdate(uint64_t eventTime, uint64_t transactionTime, const std::string &symbol, int64_t newLeverage) noexcept                                          = 0;
        virtual void eventMultiAssetsModeUpdate(uint64_t eventTime, uint64_t transactionTime, bool multiAssetsMode) noexcept                                                             = 0;

    private:
        auto receivedData() -> void override;
    };

} // namespace BINAPI_NAMESPACE::ws

#endif // WSFUTURESBINANCEUSER_HPP
