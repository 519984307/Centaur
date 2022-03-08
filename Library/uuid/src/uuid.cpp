/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "uuid.hpp"
cen::uuid::uuid(const std::string &str) :
    uuid_bytes {}
{
    if (!str.empty())
    {
        if (str.front() != '{')
            throw std::runtime_error("uuid is missing '{'");
        else if (str.back() != '}')
            throw std::runtime_error("uuid is missing '}'");
        else if (str.size() != 38)
            throw std::runtime_error("uuid invalid size");
        else
        {

            for (std::size_t i = 1l; i < str.size() - 1; ++i)
            {
                auto end = std::find_if(
                    std::begin(uuid::valid_chars),
                    std::end(uuid::valid_chars),
                    [&ch0 = str[i]](char ch) { return ch == ch0; });
                if (end == std::end(uuid::valid_chars))
                    throw std::runtime_error("invalid character in the uuid string");
                else
                {
                    //  static size_t valid_pos[2] = { 9, 14 };
                    if (str[i] == '-' && (!(i == 9 || i == 14 || i == 19 || i == 24)))
                        throw std::runtime_error("invalid format in the uuid string");
                }
            }

            std::size_t k = 0;
            for (std::size_t i = 1l; i < str.size() - 1; i += 2)
            {
                if (str[i] == '-')
                {

                    i -= 1;
                    continue;
                }
                constexpr uint32_t shift = 4;
                constexpr uint32_t digit = '0';
                constexpr uint32_t upper = 'A';
                constexpr uint32_t lower = 'a';

                uint32_t t[2]            = { 0, 0 };
                for (std::size_t j = 0; j < 2; ++j)
                {
                    auto ch = static_cast<uint32_t>(str[i + j]);
                    if (ch >= '0' && ch <= '9')
                        t[j] = ch - digit;
                    else if (ch >= 'A' && ch <= 'F')
                        t[j] = ch - upper + 10;
                    else if (ch >= 'a' && ch <= 'f')
                        t[j] = ch - lower + 10;
                }
                uuid_bytes[k] = static_cast<unsigned char>((t[0] << shift) | t[1]);
                k++;
            }
        }
    }
    else
        throw std::runtime_error("uuid string is empty");
}

auto cen::uuid::to_string(bool upper) const -> std::string
{
    std::string str;
    str.reserve(40);
    str                  = "{";

    const auto &char_set = [&upper] {
        if (upper)
            return uuid_chars_upper;
        return uuid_chars_lower;
    }();

    for (uint32_t i = 0; i < 16; ++i)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            str += '-';
        str += char_set[uuid_bytes[i]];
    }
    str += "}";
    return str;
}

