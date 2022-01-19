/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_UUID_HPP
#define CENTAUR_UUID_HPP

#ifndef CENTAUR_NAMESPACE
#define CENTAUR_NAMESPACE cen
#endif /*CENTAUR_NAMESPACE*/

#include <array>
#include <random>
#include <string>

//{12121212-1212-1212-1212-141414141414}

namespace CENTAUR_NAMESPACE
{
    struct uuid
    {
        /// \brief Construct an uuid from a string
        /// \param str The string must have the curly braces and dashes
        explicit inline constexpr uuid(const std::string_view &str) :
            uuid_bytes {}
        {
            if (!str.empty())
            {
                if (str.front() != '{')
                    throw std::runtime_error("uuid is missing '{'");
                else if (str.back() == '}')
                    throw std::runtime_error("uuid is missing '}'");
                else if (str.size() != 39)
                    throw std::runtime_error("uuid invalid size");
                else
                {
                    for (std::size_t i = 1l; i < str.size() - 1; ++i)
                    {
                        auto end = std::find_if_not(
                            std::begin(uuid::valid_chars),
                            std::end(uuid::valid_chars),
                            [&ch0 = str[i]](char ch) { return ch == ch0; });
                        if (end != std::end(uuid::valid_chars))
                            throw std::runtime_error("invalid character in the uuid string");
                    }
                }
            }
        }

    private:
        unsigned char uuid_bytes[17];

    private:
        static char valid_chars[23];
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_UUID_HPP
