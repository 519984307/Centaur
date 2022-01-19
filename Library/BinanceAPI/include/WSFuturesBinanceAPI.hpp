////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WSFuturesBinanceAPI.hpp

/**
 * @file  WSFuturesBinanceAPI.hpp
 * @brief
 *
 * @author Ricardo Romero
 * @date 03/12/2021
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#ifndef WSFUTURESBINANCEAPI_HPP
#define WSFUTURESBINANCEAPI_HPP

#include "BinanceAPIDefs.hpp"
#include "BinanceAPIGlobal.hpp"

namespace trader::api::ws
{
    /// \brief WebSocket library. Subclass this class as needed
    /// IMPORTANT WebSocket functions does not use JSON schema to validate data. All data is processed as received as is documented in
    /// https://binance-docs.github.io/apidocs/futures/en/#websocket-market-streams
    /// Any exception during the JSON parser will catch as std::exception
    /// RapidJSON uses exception to catch errors, however, any exception produce in the parsing will be practically ignored
    /// it will only print data in the console as 'Stream JSON parsing error' no more than that will be handled, this is to reduce latency problems
    /// because a lot of these streams are sent either in real-time or with very low ms difference
    class BINAPI_EXPORT WSFuturesBinanceAPI
    {
    public:
        /// \brief Construct a new WSBinanceAPI object

        WSFuturesBinanceAPI();
        virtual ~WSFuturesBinanceAPI() = default;

    protected:
        /// \brief Subscribe to a stream. thread-safe
        ///
        /// \param stream stream name
        /// \param id stream id. This id will be passed to void subscribe(const bool &result, const int &id) function to confirmed success of failure of the subscription
        /// \remark If you override subscribe, make sure to call the parent function in order to remove the id from the ids
        void subscribe(const std::string &stream, const int &id);

        /// \brief Unsubscribe to a streams. thread-safe
        ///
        /// \param stream stream name
        /// \param id stream id. This id will be passed to void unsubscribe(const bool &result, const int &id) function to confirmed success of failure of the unsubscription
        /// \remark If you override subscribe, make sure to call the parent function in order to remove the id from the ids
        void unsubscribe(const std::string &stream, const int &id);

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
        void run(const std::string &endpoint, const bool &multipleStreams = false);

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

        std::string constructEndPointFromList(const std::vector<std::string> &subscriptions) noexcept;

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

        /// \brief Stop receiving. The function is thread safe
        void terminate();

    public:
        /// \brief Called when the WebSocket is connected
        virtual void connected() = 0;
        /// \brief Called when the WebSocket gets closed
        virtual void close() = 0;
        /// \brief Called when the WebSocket failed to connect
        virtual void connectionError() = 0;

        /// Receiving methods
        /// Override any of this methods to process the received data
        /// Under a multithreading environment the user must provied thread protection mechanism to access user data
        /// This functions are not internally implemented and do not access internal data of the object
        /// Parameters with std::multimap<std::string, std::pair<uint64_t, ...>> goes as: multimap.first = SYMBOL, .second { EventTime, Stream Data}
    protected:
        virtual void subscribe(const bool &result, const int &id);
        virtual void unsubscribe(const bool &result, const int &id);
        virtual void aggregateTradeStream(const std::string &symbol, const uint64_t &eventTime, const StreamAggregateTrade &as);
        virtual void markPriceStream(const std::string &symbol, const uint64_t &eventTime, const StreamMarkPrice &smp);
        virtual void markPriceStreamAllMarket(const std::vector<StreamMarkPriceAllMarket> &vsmpal);
        /// kline handles 'Kline/Candlestick Streams' and 'BLVT NAV Kline/Candlestick Streams'
        virtual void kline(const std::string &symbol, const uint64_t &eventTime, const BinanceTimeIntervals &interval, const Candlestick &cs);
        virtual void continuousKline(const std::string &symbol, const uint64_t &eventTime, const ContractType &ct, const BinanceTimeIntervals &interval, const Candlestick &cs);
        virtual void individualSymbolMiniTicker(const std::string &symbol, const uint64_t &eventTime, const StreamIndividualSymbolMiniTicker &ticker);
        virtual void allMarketMiniTickers(const std::multimap<std::string, std::pair<uint64_t, StreamMarketMiniTickers>> &mm);
        virtual void individualSymbolTicker(const std::string &symbol, const uint64_t &eventTime, const StreamIndividualSymbolTicker &ticker);
        virtual void allMarketTickers(const std::multimap<std::string, std::pair<uint64_t, StreamMarketTickers>> &mm);
        // bookTicker handles 'Individual Symbol Book Ticker Streams' and 'All Book Tickers Stream'
        virtual void bookTicker(const std::string &symbol, const uint64_t &eventTime, const StreamBookTicker &ticker);
        // liquidationOrder handles 'Liquidation Order Streams' and 'All Market Liquidation Order Streams'
        virtual void liquidationOrder(const std::string &symbol, const uint64_t &eventTime, const StreamLiquidationOrder &slo);
        /// depthUpdate handles 'Partial Book Depth Streams' and 'Diff. Book Depth Streams'
        virtual void depthUpdate(const std::string &symbol, const uint64_t &eventTime, const StreamDepthUpdate &sdp);
        virtual void blvtInfo(const std::string &blvtName, const uint64_t &eventTime, const StreamBLVTInfo &blvt);
        virtual void compositeIndex(const std::string &symbol, const uint64_t &eventTime, const currency_t &price, const std::vector<StreamCompositionIndex> &sci);

        /// All subscription methods are thread-safe
    public:
        /// \brief Subscribe Mark price and funding rate for a single symbol pushed every 3 seconds or every second.
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \param update Set to 1000 or 3000 to specified the update milliseconds interval. assert will be issue if update is not any of this values
        /// \return std::variant<std::string, int> If the Websocket is not running it will return a string to pass to WSFuturesBinanceAPI::run(). If the websocket has started to received
        ///         data the return data will be an autogenerated integer.
        /// \remarks If the WS is not receiving you must pass the string to WSFuturesBinanceAPI::run() otherwise the stream will not be received
        ///          If the WS is receiving data the stream will be subscribe by means of calling WSFuturesBinanceAPI::subscribe(const std::string &stream, const int &id)
        ///          the success or failure of the subscrition can be confirmed when void WSFuturesBinanceAPI::subscribe(const bool &result, const int &id)
        ///          the id parameter in this function must be equal to the return value of this function
        /// \note EXAMPLE:
        ///          NOT RUNNING WS
        ///          auto stream = subscribeMarkPriceStream("ETHUSDT", 1000);
        ///          try{
        ///             run(std::get<std::strint>(stream));
        ///          }catch(const std::invalid_argument &ex){...}
        ///          RUNNING WS
        ///          static int _id;
        ///          void somefunction() { _id = std::get<int>(subscribeMarkPriceStream("ETHUSDT", 1000)); };
        ///          void WSFuturesBinanceAPI_subclass::subscribe(const bool &result, const int &id)
        ///          {
        ///                if( id == _id) fmt::print("SUBSCRITION FOR MARK PRICE STREAM FOR ETHUSDT WAS {}", result);
        ///          }
        std::variant<std::string, int> subscribeMarkPrice(const std::string &symbol, const int &update = 1000);

        /// \brief Mark price and funding rate for all symbols pushed every 3 seconds or every second.
        ///
        /// \param update Set to 1000 or 3000 to specified the update milliseconds interval. assert will be issue if update is not any of this values
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeMarkPriceAllMarket(const int &update = 1000);

        /// \brief  The Aggregate Trade Streams push trade information that is aggregated for a single taker order every 100 milliseconds.
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int>
        std::variant<std::string, int> subscribeAggregateTrade(const std::string &symbol);

        /// \brief The Kline/Candlestick Stream push updates to the current klines/candlestick every 250 milliseconds (if existing)
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \param interval Time interval
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeKline(const std::string &symbol, const BinanceTimeIntervals &interval);

        /// \brief The continuous Kline/Candlestick Stream push updates to the current klines/candlestick every 250 milliseconds (if existing)
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \param ctype Contract type. ContractType::none will default to ConstracType::perpetual
        /// \param interval Time interval
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeContinuousContractKline(const std::string &symbol, const ContractType &ctype, const BinanceTimeIntervals &interval);

        /// \brief 24hr rolling window mini-ticker statistics for a single symbol. These are NOT the statistics of the UTC day, but a 24hr rolling window from requestTime to 24hrs before
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeIndividualMiniTicker(const std::string &symbol);

        ///
        /// \brief unsubscribeIndividualMiniTicker Unsuscribe the mini ticker
        /// \param symbol Symbol to unsubscribe
        /// \return id
        ///
        int unsubscribeIndividualMiniTicker(const std::string &symbol);

        /// \brief 24hr rolling window mini-ticker statistics for all symbols. These are NOT the statistics of the UTC day, but a 24hr rolling window from requestTime to 24hrs before.
        ///        Note that only tickers that have changed will be present in the array.
        ///
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeAllMarketMiniTickers();

        /// \brief 24hr rollwing window ticker statistics for a single symbol. These are NOT the statistics of the UTC day, but a 24hr rolling window from requestTime to 24hrs before.
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeIndividualSymbolTicker(const std::string &symbol);

        /// \brief 24hr rollwing window ticker statistics for all symbols. These are NOT the statistics of the UTC day, but a 24hr rolling window from requestTime to 24hrs before.
        ///        Note that only tickers that have changed will be present in the array.
        ///
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeAllMarketTickers();

        /// \brief Pushes any update to the best bids or asks price or quantity in real-time for a specified symbol.(REAL-TIME)
        ///
        /// \param symbol Symbol name. The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeIndividualSymbolBookTickerStreams(const std::string &symbol);

        /// \brief Pushes any update to the best bids or asks price or quantity in real-time for all symbols.
        ///
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeAllBookTickerStream();

        /// \brief The Liquidation Order Snapshot Streams push force liquidation order information for specific symbol.
        ///        For each symbol，only the latest one liquidation order within 1000ms will be pushed as the snapshot. If no liquidation happens in the interval of 1000ms,
        ///        no stream will be pushed.
        ///
        /// \param symbol The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeLiquidationOrder(const std::string &symbol);

        /// \brief The All Liquidation Order Snapshot Streams push force liquidation order information for all symbols in the market.
        ///        For each symbol，only the latest one liquidation order within 1000ms will be pushed as the snapshot. If no liquidation happens in the interval of 1000ms,
        ///        no stream will be pushed.
        ///
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeAllMarketLiquidationOrderStream();

        /// \brief Top bids and asks
        ///
        /// \param symbol The symbol can be uppercase, however, the method will convert to lowercase
        /// \param levels Valid are 5, 10, or 20.
        /// \param update Valid are 250, 500 or 100 and the time is in milliseconds
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribePartialBookDepth(const std::string &symbol, const int &levels, const int &update);

        /// \brief Bids and asks, pushed update milliseconds
        ///
        /// \param symbol The symbol can be uppercase, however, the method will convert to lowercase
        /// \param update 250 milliseconds, 500 milliseconds, 100 milliseconds (if existing)
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeDiffBookDepth(const std::string &symbol, const int &update);

        /// \brief BLVT Info Streams
        ///
        /// \param tokenName The symbol can be lowercase, however, the method will convert to uppercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeBLVTInfo(const std::string &tokenName);

        /// \brief BLVT NAV Kline/Candlestick Streams
        ///
        /// \param tokenName The symbol can be lowercase, however, the method will convert to uppercase
        /// \param interval Time interval
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeBLVTNavKline(const std::string &tokenName, const BinanceTimeIntervals &interval);

        /// \brief Composite index information for index symbols pushed every second.
        ///
        /// \param symbol The symbol can be uppercase, however, the method will convert to lowercase
        /// \return std::variant<std::string, int> see subscribeMarkPriceStream return and remarks documentation subsection
        std::variant<std::string, int> subscribeCompositeIndexSymbolInformation(const std::string &symbol);

    private:
        lws_context *m_context { nullptr };
        lws_protocols m_protocols[2] {};
        lws *m_lws { nullptr };
        std::atomic_bool m_running { false };
        std::atomic_bool m_terminate { false };
        std::atomic_bool m_combined { false };
        std::set<int> m_subscribeIds;
        std::set<int> m_unsubscribeIds;

    private:
        void sendData(const std::string &msg);
        void receivedData(const std::string &msg);
        int randNumber(const bool &subscribe = true);
        static int eventManager(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
    };
} // namespace trader::api::ws
#endif // WSFUTURESBINANCEAPI_HPP
