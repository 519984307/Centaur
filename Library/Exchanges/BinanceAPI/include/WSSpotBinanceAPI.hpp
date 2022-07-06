////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef WSSPOTBINANCEAPI_HPP
#define WSSPOTBINANCEAPI_HPP

#include "BinanceAPIGlobal.hpp"
//
#include "BinanceAPIDefs.hpp"
#include "WSThread.hpp"

namespace BINAPI_NAMESPACE::ws
{

    class BINAPI_EXPORT WSSpotBinanceAPI : public WSThread
    {
    public:
        WSSpotBinanceAPI(std::string endpoint);
        ~WSSpotBinanceAPI() override = default;

    protected:
        /// \brief Subscribe to a stream. thread-safe
        ///
        /// \param stream stream name
        /// \param id stream id. This id will be passed to void subscribe(const bool &result, const int &id) function to confirmed success of failure of the subscription
        /// \remark If you override subscribe, make sure to call the parent function in order to remove the id from the ids
        void subscribe(const std::string &stream, int id);

        /// \brief Unsubscribe to a streams. thread-safe
        ///
        /// \param stream stream name
        /// \param id stream id. This id will be passed to void unsubscribe(const bool &result, const int &id) function to confirmed success of failure of the unsubscription
        /// \remark If you override subscribe, make sure to call the parent function in order to remove the id from the ids
        void unsubscribe(const std::string &stream, int id);

    public:
        /// \brief Blocking function to start receiving the Exchange streams
        ///
        /// \param endpoint Initial endpoint. this will be: wss://fstream.binance.com:443/endpoint.
        ///                 see https://binance-docs.github.io/apidocs/futures/en/#websocket-market-streams for more detail
        ///                 optional you can construct the data
        /// \param multipleStreams if you try to acquire more than one stream set multipleStreams to true if only one stream is used multipleStreams must be set to false
        ///                        Setting this parameter wrong will start to produce exceptions that ultimate will slow down the whole stream JSON parsing
        ///                        causing serious latency issues
        /// \remark if endpoint will be constructed with more than one string returned by the subscribeXXXXX methods use WSFuturesBinanceAPI::constructEndpoint().
        ///         See constructEndPoint example.
        ///         if endpoint is ill formed the connection will fail.
        /// \remark std::runtime_error will be throw if run is already running
        //  void run(const std::string &endpoint, const bool &multipleStreams = false);

        /// Receiving methods
        /// Override any of this methods to process the received data
        /// Under a multithreading environment the user must provied thread protection mechanism to access user data
        /// This functions are not internally implemented and do not access internal data of the object
        /// Parameters with std::multimap<std::string, std::pair<uint64_t, ...>> goes as: multimap.first = SYMBOL, .second { EventTime, Stream Data}
    protected:
        virtual void subscribe(bool result, int id);
        virtual void unsubscribe(bool result, int id);
        virtual void kline(const std::string &symbol, uint64_t eventTime, BinanceTimeIntervals interval, const Candlestick &cs);
        virtual void individualSymbolMiniTicker(const std::string &symbol, uint64_t eventTime, const StreamIndividualSymbolMiniTicker &ticker);
        virtual void allMarketMiniTickers(const std::multimap<std::string, std::pair<uint64_t, StreamMarketMiniTickers>> &mm);
        virtual void individualSymbolTicker(const std::string &symbol, uint64_t eventTime, const StreamIndividualSymbolTicker &ticker);
        virtual void allMarketTickers(const std::multimap<std::string, std::pair<uint64_t, StreamMarketTickers>> &mm);
        // bookTicker handles 'Individual Symbol Book Ticker Streams' and 'All Book Tickers Stream'
        virtual void bookTicker(const std::string &symbol, uint64_t eventTime, const StreamBookTicker &ticker);
        /// depthUpdate handles 'Partial Book Depth Streams' and 'Diff. Book Depth Streams'
        virtual void depthUpdate(const std::string &symbol, uint64_t eventTime, const StreamDepthUpdate &sdp);

    public:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command" // Just to get rid of the warning in these lines: /ws/btcusdt@markPrice@{}1s AND /ws/btcusdt@markPrice@{}1s
#endif                                                             /*__clang__*/
        /// \brief Construct the endpoint for one or more streams
        ///
        /// @tparam Args Variadic strings template
        /// \param args Streams
        /// \return std::string the constructed endpoint
        /// EXAMPLE:
        ///     One-stream
        ///         auto s1 = subscribeMarkPrice("BTCUSDT");
        ///         std::string ep = std::get<std::string>(constructEndPoint(s1)); // ep: /ws/btcusdt@markPrice@{}1s
        ///         WSFuturesBinanceAPI::run(ep, false); // BEWARE: multipleStreams = false
        ///     Two or more streams
        ///         auto s1 = std::get<std::string>(subscribeMarkPrice("BTCUSDT"));
        ///         auto s2 = std::get<std::string>(subscribePartialBookDepth("BTCUSDT", 5, 250));
        ///         std::string ep = constructEndPoint(s1, s2); // ep: /stream?streams=btcusdt@markPrice@{}1s/btcusdt@depth5@250ms
        ///         WSFuturesBinanceAPI::run(ep, true); // BEWARE: multipleStreams = true
        template <typename... Args>
        std::string constructEndPoint(Args &...args)
        {
            std::string ret;
            int count = 0;
            for (auto &set : { typename std::common_type<Args...>::type(args)... })
            {
                ret += std::string(set) + "/";
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

        // Make endpoint is just the same as constructEndPoint but static; constructEndPoint might be removed
        template <typename... Args>
        static std::string makeEndPoint(Args &...args)
        {
            std::string ret;
            int count = 0;
            for (auto &set : { typename std::common_type<Args...>::type(args)... })
            {
                ret += std::string(set) + "/";
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

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

        /*        /// \brief Stop receiving. The function is thread safe
                void terminate();

            public:
                /// \brief Called when the WebSocket is connected
                virtual void connected() = 0;
                /// \brief Called when the WebSocket gets closed
                virtual void close() = 0;
                /// \brief Called when the WebSocket failed to connect
                virtual void connectionError() = 0;*/

        /// All subscription methods are thread-safe
    public:
        /// \brief The Kline/Candlestick Stream push updates to the current klines/candlestick every 250 milliseconds (if existing)
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \param interval Time interval
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeKline(const std::string &symbol, BinanceTimeIntervals interval);

        /// \brief Stop receiving the symbol data
        /// \param symbol Symbol name
        /// \param interval Time interval
        /// \return The unsubscription ID
        int unsubscribeKline(const std::string &symbol, BinanceTimeIntervals interval);

        /// \brief 24hr rolling window mini-ticker statistics.
        /// These are NOT the statistics of the UTC day, but a 24hr rolling window for the previous 24hrs.
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeIndividualMiniTicker(const std::string &symbol);

        ///
        /// \brief unsubscribeIndividualMiniTicker Stop receiving data from the WS
        /// \param symbol Symbol name
        /// \return The unsubscription ID
        /// \remarks The WebSocket must be running
        ///
        int unsubscribeIndividualMiniTicker(const std::string &symbol);

        /// \brief 24hr rolling window mini-ticker statistics for all symbols that changed in an array.
        /// These are NOT the statistics of the UTC day, but a 24hr rolling window for the previous 24hrs.
        /// Note that only tickers that have changed will be present in the array.
        ///
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeAllMarketMiniTickers();

        /// \brief 24hr rolling window ticker statistics for a single symbol.
        /// These are NOT the statistics of the UTC day, but a 24hr rolling window for the previous 24hrs.
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeIndividualSymbolTicker(const std::string &symbol);

        /// \brief 24hr rolling window ticker statistics for all symbols that changed in an array.
        /// These are NOT the statistics of the UTC day, but a 24hr rolling window for the previous 24hrs.
        /// Note that only tickers that have changed will be present in the array.
        ///
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeAllMarketTickers();

        /// \brief Pushes any update to the best bids or asks price or quantity in real-time for a specified symbol.(REAL-TIME)
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeIndividualSymbolBookTickerStreams(const std::string &symbol);

        /// \brief Pushes any update to the best bid or ask's price or quantity in real-time for all symbols.
        ///
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeAllBookTickerStream();

        /// \brief Top bids and asks
        ///
        /// \param symbol The symbol can be uppercase, however, the method will convert to lowercase
        /// \param levels Valid are 5, 10, or 20.
        /// \param update Valid are 100ms or 1000ms
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribePartialBookDepth(const std::string &symbol, int levels, int update);

        /// \brief Bids and asks, pushed update milliseconds
        ///
        /// \param symbol The symbol can be uppercase, however, the method will convert to lowercase
        /// \param update 100 milliseconds, 1000 milliseconds (if existing)
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeDiffBookDepth(const std::string &symbol, int update);

        ///
        /// \brief subscribeDiffBookDepth Stop receiving the Diff. Book depth stream
        /// \param symbol Symbol to unsubscribe
        /// \param update update time
        /// \return the stream id from the exchange
        ///
        int unsubscribeDiffBookDepth(const std::string &symbol, int update);

    private:
        std::atomic_bool m_combined { false };
        std::set<int> m_subscribeIds;
        std::set<int> m_unsubscribeIds;

    private:
        //    void sendData(const std::string &msg);
        auto receivedData() -> void override;
        //    int randNumber(const bool &subscribe = true);
        //    static int eventManager(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
    };
} // namespace BINAPI_NAMESPACE::ws
#endif // WSSPOTBINANCEAPI_HPP
