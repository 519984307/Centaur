////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// limits.hpp

/**
 * @file limits.hpp
 * @brief Binance Limits
 *
 * @author Ricardo Romero
 * @date 2021-07-07
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#ifdef __cplusplus
#pragma once
#endif /* __cplusplus*/

#ifndef LIMITS_HPP_
#define LIMITS_HPP_

#include "BinanceAPIGlobal.hpp"
#include <chrono>
#include <mutex>

namespace trader::api
{
    /**
     * BinanceLimits is thread safe
     */
    struct BINAPI_EXPORT BinanceLimits
    {
    public:
        /**
         * Called to secure an API call
         * @return false if limits have been reached or true for otherwise
         */
        bool secureCall(const long long &callWeight);

        /**
         * Some exchanges differentiate normal api calls from orders calls
         * @return  false if limits have been reached or true for otherwise
         */
        bool secureOrderCall(const long long &callWeight);

    public:
        [[nodiscard]] inline long long getMaxAPIRequestWeight() const noexcept
        {
            return m_maxApiRequestsWeight;
        }

        [[nodiscard]] inline long long getMaxAPIRequestInterval() const noexcept
        {
            return m_maxApiRequestsTsInterval;
        }

        void setAPIRequestsLimits(const long long &max, const long long &interval) noexcept;

        [[nodiscard]] inline long long getMaxAPIOrdersLow() const noexcept
        {
            return m_maxApiOrdersLow;
        }

        [[nodiscard]] inline long long getMaxAPIOrdersLowInterval() const noexcept
        {
            return m_maxApiOrdersTsIntervalLow;
        }

        void setAPIOrderLimitLow(const long long &max, const long long &interval) noexcept;

        [[nodiscard]] inline long long getMaxAPIOrdersHigh() const noexcept
        {
            return m_maxApiOrdersHigh;
        }

        [[nodiscard]] inline long long getMaxAPIOrdersHighInterval() const noexcept
        {
            return m_maxApiOrdersTsIntervalHigh;
        }

        void setAPIOrderLimitHigh(const long long &max, const long long &interval) noexcept;

        [[nodiscard]] inline long long getAPIRequests() const noexcept
        {
            return m_apiRequests;
        }

        [[nodiscard]] inline long long getTotalAPIRequests() const noexcept
        {
            return m_totalRequests;
        }

        [[nodiscard]] inline long long getAPIRequestSeconds() const noexcept
        {
            return m_requestsSeconds;
        }

        [[nodiscard]] inline long long getAPIOrdersLow() const noexcept
        {
            return m_apiOrdersLow;
        }

        [[nodiscard]] inline long long getTotalAPIOrdersLow() const noexcept
        {
            return m_totalOrdersLow;
        }

        [[nodiscard]] inline long long getAPIOrdersLowSeconds() const noexcept
        {
            return m_lowRequestsSeconds;
        }

        [[nodiscard]] inline long long getAPIOrdersHigh() const noexcept
        {
            return m_apiOrdersHigh;
        }

        [[nodiscard]] inline long long getTotalAPIOrdersHigh() const noexcept
        {
            return m_totalOrdersHigh;
        }

        [[nodiscard]] inline long long getAPIOrdersHighSeconds() const noexcept
        {
            return m_highRequestsSeconds;
        }

    private:
        std::mutex m_secureRequest;
        std::mutex m_orderRequest;

        std::chrono::seconds m_secondsNow { 0 };

        long long m_apiRequests { 0 };
        long long m_totalRequests { 0 };
        long long m_requestsSeconds { 0 };

        std::chrono::seconds m_secondsNowOrderLow { 0 };
        long long m_apiOrdersLow { 0 };
        long long m_totalOrdersLow { 0 };
        long long m_lowRequestsSeconds { 0 };
        std::chrono::seconds m_secondsNowOrderHigh { 0 };
        long long m_apiOrdersHigh { 0 };
        long long m_totalOrdersHigh { 0 };
        long long m_highRequestsSeconds { 0 };

        // INITIAL 20 VALUE IS TEMPORARY
        long long m_maxApiRequestsWeight { 1000 };
        long long m_maxApiRequestsTsInterval { 60 };

        long long m_maxApiOrdersHigh { 20 };
        long long m_maxApiOrdersTsIntervalHigh { 20 };

        long long m_maxApiOrdersLow { 20 };
        long long m_maxApiOrdersTsIntervalLow { 20 };
    };
} // namespace trader::api

#endif /*LIMITS_HPP_*/
