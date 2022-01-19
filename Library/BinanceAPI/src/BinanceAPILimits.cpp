////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// limits.cpp

/**
 * @file limits.cpp
 * @brief Binnace limits
 *
 * @author Ricardo Romero
 * @date 2021-07-07
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#include "BinanceAPILimits.hpp"

bool trader::api::BinanceLimits::secureCall(const long long &callWeight)
{
    // Protect all reads and writes from other threads
    std::lock_guard<std::mutex> lock(m_secureRequest);

    // No requests. so m_secondsNow = 0, and we must initialize it to a valid time
    if (m_secondsNow.count() == 0)
        m_secondsNow = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    // This variable sets the time right now
    std::chrono::seconds seconds_right_now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    // These Operations calculate the time elapsed since right now from the m_secondsNow initialization
    auto span         = seconds_right_now - m_secondsNow;
    m_requestsSeconds = span.count();
    // If time limit is reached this is
    if (span.count() < m_maxApiRequestsTsInterval && m_apiRequests + callWeight > m_maxApiRequestsWeight)
    {
        return false;
    }

    // If the operation is done after the limit and limit request is not reached reset api requests and reset the timer
    if (span.count() > m_maxApiRequestsTsInterval && m_apiRequests < m_maxApiRequestsWeight)
    {
        m_apiRequests = 0;
        m_secondsNow  = std::chrono::seconds(0ll);
    }

    // Update the request for the time span (m_apiRequests) and the total api requests
    m_apiRequests += callWeight;
    m_totalRequests += callWeight;

    // Allow the operation
    return true;
}

bool trader::api::BinanceLimits::secureOrderCall(const long long &callWeight)
{
    // Protect all reads and writes from other threads
    std::lock_guard<std::mutex> lock(m_orderRequest);

    // No requests. so m_seconds_now = 0 and we must initialized it to a valid time
    if (m_secondsNowOrderLow.count() == 0)
        m_secondsNowOrderLow = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    if (m_secondsNowOrderHigh.count() == 0)
        m_secondsNowOrderHigh = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    // This variable sets the time right now
    std::chrono::seconds secondsRightNow = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    // This Operations calculates the time elpased since right now from the m_seconds_now initialization
    auto span_low         = secondsRightNow - m_secondsNowOrderLow;
    auto span_high        = secondsRightNow - m_secondsNowOrderHigh;

    m_lowRequestsSeconds  = span_low.count();
    m_highRequestsSeconds = span_high.count();

    // If time limit is reached this is
    if (span_low.count() < m_maxApiOrdersTsIntervalLow && m_apiOrdersLow + callWeight > m_maxApiOrdersLow)
    {
        return false;
    }
    // If the operation is done after the limit and limit request is not reached reset api requests and reset the timer
    if (span_low.count() > m_maxApiOrdersTsIntervalLow && m_apiOrdersLow < m_maxApiOrdersLow)
    {
        m_apiOrdersLow       = 0;
        m_secondsNowOrderLow = std::chrono::seconds(0ll);
    }

    bool bLimitHighExceeded = false;
    // If time limit is reached this is
    if (span_high.count() < m_maxApiOrdersTsIntervalHigh && m_apiOrdersHigh > m_maxApiOrdersHigh)
    {
        bLimitHighExceeded = true;
    }
    // If the operation is done after the limit and limit request is not reached reset api requests and reset the timer
    if (span_high.count() > m_maxApiOrdersTsIntervalHigh)
    {
        ;
        // Only when the count is higher than m_maxApiOrdersTsIntervalHigh reset the counters
        m_apiOrdersHigh       = 0;
        m_secondsNowOrderHigh = std::chrono::seconds(0ll);
        bLimitHighExceeded    = false;
    }

    // Do the operation taking care of Server Requests
    if (bLimitHighExceeded)
        return false;

    // Update the request for the time span (m_api_requests) and the total api requests
    m_apiOrdersLow += callWeight;
    m_totalOrdersLow += callWeight;

    m_apiOrdersHigh += callWeight;
    m_totalOrdersHigh += callWeight;

    // Return
    return true;
}

void trader::api::BinanceLimits::setAPIRequestsLimits(const long long &max, const long long &interval) noexcept
{
    // Timer counter is in seconds
    m_maxApiRequestsWeight     = max;
    m_maxApiRequestsTsInterval = interval;
}

void trader::api::BinanceLimits::setAPIOrderLimitLow(const long long &max, const long long &interval) noexcept
{
    // Timer counter is in seconds
    m_maxApiOrdersLow           = max;
    m_maxApiOrdersTsIntervalLow = interval;
}
void trader::api::BinanceLimits::setAPIOrderLimitHigh(const long long &max, const long long &interval) noexcept
{
    // Timer counter is in seconds
    m_maxApiOrdersHigh           = max;
    m_maxApiOrdersTsIntervalHigh = interval;
}
