////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  APIException.cpp

/**
 * @file  APIException.cpp
 * @brief APIException
 *
 * @author Ricardo Romero
 * @date 03/12/2021
 *
 * Copyright (c) 2021 - present, Ricardo Romero
 * All rights reserved.
 *
 */

#include "APIException.hpp"

#include <utility>

trader::api::APIException::APIException(Type type, std::string what) noexcept :
    m_type { type },
    m_url {},
    m_what {std::move( what )},
    m_msg {},
    m_status { 0 },
    m_code { 0 }
{
}

trader::api::APIException::APIException(Type type, int code, std::string url, std::string what) noexcept :
    m_type { type },
    m_url {std::move( url )},
    m_what {std::move( what )},
    m_msg {},
    m_status { 0 },
    m_code { code }
{
}

trader::api::APIException::APIException(Type type, int status, int code, std::string url, std::string what, std::string msg) noexcept :
    m_type { type },
    m_url {std::move( url )},
    m_what {std::move( what )},
    m_msg {std::move( msg )},
    m_status { status },
    m_code { code }
{
}

// Use for type::json and type::schema
trader::api::APIException::APIException(Type type, std::string url, std::string what) noexcept :
    m_type { type },
    m_url {std::move( url )},
    m_what {std::move( what )},
    m_msg {},
    m_status { 0 },
    m_code { 0 }
{
}

trader::api::APIException::~APIException() = default;

T_NODISCARD const char *trader::api::APIException::what() const noexcept
{
    return m_what.c_str();
}

T_NODISCARD trader::api::xstr trader::api::APIException::url() const noexcept
{
    return m_url.empty() ? xstr { std::nullopt } : xstr { m_url.c_str() };
}

T_NODISCARD trader::api::xstr trader::api::APIException::msg() const noexcept
{
    return m_msg.empty() ? xstr { std::nullopt } : xstr { m_msg.c_str() };
}

T_NODISCARD trader::api::xint trader::api::APIException::status() const noexcept
{
    using enum Type;
    return m_type == http2 ? xint { m_status } : xint { std::nullopt };
}

T_NODISCARD trader::api::xint trader::api::APIException::code() const noexcept
{
    using enum Type;
    return m_type == http1 || m_type == http2 || m_type == request || m_type == api ? xint { m_code } : xint { std::nullopt };
}

trader::api::APIException::Type trader::api::APIException::type() const noexcept
{
    return m_type;
}
std::tuple<std::string> trader::api::APIException::limits() const noexcept
{
    return { m_what };
}
std::tuple<int, std::string, std::string> trader::api::APIException::request() const noexcept
{
    return {m_code, m_url, m_what};
}

std::tuple<int, std::string, std::string> trader::api::APIException::http1() const noexcept
{
    return {m_code, m_url, m_what};
}

std::tuple<int, int, std::string, std::string, std::string> trader::api::APIException::http2() const noexcept
{
    return {m_status, m_code, m_url, m_what, m_msg};
}

std::tuple<std::string, std::string> trader::api::APIException::json() const noexcept
{
    return {m_url, m_what};
}

std::tuple<int, std::string, std::string> trader::api::APIException::api() const noexcept
{
    return {m_code, m_url, m_what};
}

std::tuple<std::string, std::string> trader::api::APIException::schema() const noexcept
{
    return {m_url, m_what};
}
