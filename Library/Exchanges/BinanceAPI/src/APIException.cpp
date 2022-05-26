////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////


//  APIException.cpp

#include "APIException.hpp"

#include <utility>

BINAPI_NAMESPACE::APIException::APIException(Type type, std::string what) noexcept :
    m_type { type },
    m_url {},
    m_what {std::move( what )},
    m_msg {},
    m_status { 0 },
    m_code { 0 }
{
}

BINAPI_NAMESPACE::APIException::APIException(Type type, int code, cpr::Url url, std::string what) noexcept :
    m_type { type },
    m_url {std::move( url )},
    m_what {std::move( what )},
    m_msg {},
    m_status { 0 },
    m_code { code }
{
}

BINAPI_NAMESPACE::APIException::APIException(Type type, long status, int code, cpr::Url url, std::string what, std::string msg) noexcept :
    m_type { type },
    m_url {std::move( url )},
    m_what {std::move( what )},
    m_msg {std::move( msg )},
    m_status { status },
    m_code { code }
{
}

// Use for type::json and type::schema
BINAPI_NAMESPACE::APIException::APIException(Type type, cpr::Url url, std::string what) noexcept :
    m_type { type },
    m_url {std::move( url )},
    m_what {std::move( what )},
    m_msg {},
    m_status { 0 },
    m_code { 0 }
{
}

BINAPI_NAMESPACE::APIException::~APIException() = default;

T_NODISCARD const char *BINAPI_NAMESPACE::APIException::what() const noexcept
{
    return m_what.c_str();
}

T_NODISCARD BINAPI_NAMESPACE::xstr BINAPI_NAMESPACE::APIException::url() const noexcept
{
    return m_url.str().empty() ? xstr { std::nullopt } : xstr { m_url.c_str() };
}

T_NODISCARD BINAPI_NAMESPACE::xstr BINAPI_NAMESPACE::APIException::msg() const noexcept
{
    return m_msg.empty() ? xstr { std::nullopt } : xstr { m_msg.c_str() };
}

T_NODISCARD BINAPI_NAMESPACE::xint BINAPI_NAMESPACE::APIException::status() const noexcept
{
    using enum Type;
    return m_type == http2 ? xint { m_status } : xint { std::nullopt };
}

T_NODISCARD BINAPI_NAMESPACE::xint BINAPI_NAMESPACE::APIException::code() const noexcept
{
    using enum Type;
    return m_type == http1 || m_type == http2 || m_type == request || m_type == api ? xint { m_code } : xint { std::nullopt };
}

BINAPI_NAMESPACE::APIException::Type BINAPI_NAMESPACE::APIException::type() const noexcept
{
    return m_type;
}
std::tuple<std::string> BINAPI_NAMESPACE::APIException::limits() const noexcept
{
    return { m_what };
}
std::tuple<int, std::string, std::string> BINAPI_NAMESPACE::APIException::request() const noexcept
{
    return {m_code, m_url.str(), m_what};
}

std::tuple<int, std::string, std::string> BINAPI_NAMESPACE::APIException::http1() const noexcept
{
    return {m_code, m_url.str(), m_what};
}

std::tuple<int, int, std::string, std::string, std::string> BINAPI_NAMESPACE::APIException::http2() const noexcept
{
    return {m_status, m_code, m_url.str(), m_what, m_msg};
}

std::tuple<std::string, std::string> BINAPI_NAMESPACE::APIException::json() const noexcept
{
    return {m_url.str(), m_what};
}

std::tuple<int, std::string, std::string> BINAPI_NAMESPACE::APIException::api() const noexcept
{
    return {m_code, m_url.str(), m_what};
}

std::tuple<std::string, std::string> BINAPI_NAMESPACE::APIException::schema() const noexcept
{
    return {m_url.str(), m_what};
}
