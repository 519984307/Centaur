////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#pragma once

#ifndef LIMITS_HPP_
#define LIMITS_HPP_

#include "BinanceAPIGlobal.hpp"
#include <chrono>
#include <mutex>

namespace BINAPI_NAMESPACE
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
        T_NODISCARD inline long long getMaxAPIRequestWeight() const noexcept
        {
            return m_maxApiRequestsWeight;
        }

        T_NODISCARD inline long long getMaxAPIRequestInterval() const noexcept
        {
            return m_maxApiRequestsTsInterval;
        }

        void setAPIRequestsLimits(const long long &max, const long long &interval) noexcept;

        T_NODISCARD inline long long getMaxAPIOrdersLow() const noexcept
        {
            return m_maxApiOrdersLow;
        }

        T_NODISCARD inline long long getMaxAPIOrdersLowInterval() const noexcept
        {
            return m_maxApiOrdersTsIntervalLow;
        }

        void setAPIOrderLimitLow(const long long &max, const long long &interval) noexcept;

        T_NODISCARD inline long long getMaxAPIOrdersHigh() const noexcept
        {
            return m_maxApiOrdersHigh;
        }

        T_NODISCARD inline long long getMaxAPIOrdersHighInterval() const noexcept
        {
            return m_maxApiOrdersTsIntervalHigh;
        }

        void setAPIOrderLimitHigh(const long long &max, const long long &interval) noexcept;

        T_NODISCARD inline long long getAPIRequests() const noexcept
        {
            return m_apiRequests;
        }

        T_NODISCARD inline long long getTotalAPIRequests() const noexcept
        {
            return m_totalRequests;
        }

        T_NODISCARD inline long long getAPIRequestSeconds() const noexcept
        {
            return m_requestsSeconds;
        }

        T_NODISCARD inline long long getAPIOrdersLow() const noexcept
        {
            return m_apiOrdersLow;
        }

        T_NODISCARD inline long long getTotalAPIOrdersLow() const noexcept
        {
            return m_totalOrdersLow;
        }

        T_NODISCARD inline long long getAPIOrdersLowSeconds() const noexcept
        {
            return m_lowRequestsSeconds;
        }

        T_NODISCARD inline long long getAPIOrdersHigh() const noexcept
        {
            return m_apiOrdersHigh;
        }

        T_NODISCARD inline long long getTotalAPIOrdersHigh() const noexcept
        {
            return m_totalOrdersHigh;
        }

        T_NODISCARD inline long long getAPIOrdersHighSeconds() const noexcept
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
