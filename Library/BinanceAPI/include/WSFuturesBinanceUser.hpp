////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WSFuturesBinanceUser.hpp

/**
 * @file  WSFuturesBinanceUser.hpp
 * @brief
 *
 * @author Ricardo Romero
 * @date 03/12/2021
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#ifndef WSFUTURESBINANCEUSER_HPP
#define WSFUTURESBINANCEUSER_HPP

#include "BinanceAPIDefs.hpp"
#include "BinanceAPIGlobal.hpp"

namespace trader::api::ws
{
    /// \brief User data stream WebSocket. Subclass as needed
    /// If the WebSocket expires the WebSocket will be close. In this scenario you can use run() again.
    class BINAPI_EXPORT WSFuturesBinanceUser
    {
    public:
        WSFuturesBinanceUser();
        virtual ~WSFuturesBinanceUser();

    public:
        /// \brief Called when the WebSocket is connected
        virtual void connected() = 0;
        /// \brief Called when the WebSocket gets closed
        virtual void close() = 0;
        /// \brief Called when the WebSocket failed to connect
        virtual void connectionError() = 0;
        /// \brief Called when the websocket expires
        /// \remark Once the function returns the blocking loop will terminate
        virtual void listenKeyExpired(const uint64_t &eventTime) noexcept = 0;
        /// \brief Called when the ping frame was sent successfully or unsuccessfully
        virtual void pingSent(const bool &success) noexcept = 0;

    public:
        /// \brief Stop receiving. The function is thread safe
        void terminate();

    public:
        /// \brief Blocking function to start receiving the Exchange streams
        ///
        /// \param listenKey User listen key. Get one from BinanceAPI::fapiStartUserDataStream()
        /// \param ping a timer will be schedule to send a ping frame every ping seconds. Set to 0 to disable
        /// \remark When the expired event is received listenKeyExpired will be called. After listenKeyExpired() returns
        ///         The blocking loop will be terminate. If you are using this function in std::thread the thread will probably end if you are using join.
        ///         You can use run again but in a different thread from where run was running
        /// \remark std::runtime_error will be throw if run is already running
        void run(const std::string &listenKey, const uint64_t &ping);

    protected:
        virtual void eventMarginCall(const uint64_t &eventTime, const trader::api::UserStreamMarginCallFutures &marginCall) noexcept                                     = 0;
        virtual void eventBalanceAndPositionUpdate() noexcept                                                                                                            = 0;
        virtual void eventOrderUpdate(const uint64_t &eventTime, const uint64_t &transactionTime, const std::string &symbol, const UserStreamOrderUpdate &data) noexcept = 0;
        virtual void eventLeverageUpdate(const uint64_t &eventTime, const uint64_t &transactionTime, const std::string &symbol, const int64_t &newLeverage) noexcept     = 0;
        virtual void eventMultiAssetsModeUpdate(const uint64_t &eventTime, const uint64_t &transactionTime, const bool &multiAssetsMode) noexcept                        = 0;

    private:
        uint64_t m_ping { 0 };
        lws_context *m_context { nullptr };
        lws_protocols m_protocols[2];
        lws *m_lws { nullptr };
        std::atomic_bool m_running { false };
        std::atomic_bool m_terminate { false };

    private:
        void receivedData(const std::string &msg);
        static int eventManager(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
    };

} // namespace trader::api::ws

#endif // WSFUTURESBINANCEUSER_HPP
