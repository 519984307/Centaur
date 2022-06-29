/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "uuid.hpp"
cen::uuid::uuid(const std::string &str) :
    data {}
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
                data.uuid_bytes[k] = static_cast<unsigned char>((t[0] << shift) | t[1]);
                k++;
            }
        }
    }
    else
        throw std::runtime_error("uuid string is empty");
}

cen::uuid::uuid(const uint32_t u1, const uint16_t w1, const uint16_t w2, const uint16_t w3, const uint8_t b1, const uint8_t b2, const uint8_t b3, const uint8_t b4, const uint8_t b5, const uint8_t b6, bool endiannessCheck) noexcept :
    data {}
{
    if (endiannessCheck)
    {
        if (std::endian::native == std::endian::big)
        {
            auto *p32 = reinterpret_cast<uint32_t *>(data.uuid_bytes);
            auto *p16 = reinterpret_cast<uint16_t *>(data.uuid_bytes);
            auto *p8  = reinterpret_cast<uint8_t *>(data.uuid_bytes);

            p32[0]    = u1;
            p16[2]    = w1;
            p16[3]    = w2;
            p16[4]    = w3;
            p8[10]    = b1;
            p8[11]    = b2;
            p8[12]    = b3;
            p8[13]    = b4;
            p8[14]    = b5;
            p8[15]    = b6;
        }
        else
        {
            auto *p8   = reinterpret_cast<uint8_t *>(data.uuid_bytes);
            auto *u8   = reinterpret_cast<uint8_t *>(const_cast<uint32_t *>(&u1));
            auto *w1_8 = reinterpret_cast<uint8_t *>(const_cast<uint16_t *>(&w1));
            auto *w2_8 = reinterpret_cast<uint8_t *>(const_cast<uint16_t *>(&w2));
            auto *w3_8 = reinterpret_cast<uint8_t *>(const_cast<uint16_t *>(&w3));

            p8[0]      = u8[3];
            p8[1]      = u8[2];
            p8[2]      = u8[1];
            p8[3]      = u8[0];
            p8[4]      = w1_8[1];
            p8[5]      = w1_8[0];
            p8[6]      = w2_8[1];
            p8[7]      = w2_8[0];
            p8[8]      = w3_8[1];
            p8[9]      = w3_8[0];
            p8[10]     = b1;
            p8[11]     = b2;
            p8[12]     = b3;
            p8[13]     = b4;
            p8[14]     = b5;
            p8[15]     = b6;
        }
    }
    else
    {
        auto *p32 = reinterpret_cast<uint32_t *>(data.uuid_bytes);
        auto *p16 = reinterpret_cast<uint16_t *>(data.uuid_bytes);
        auto *p8  = reinterpret_cast<uint8_t *>(data.uuid_bytes);

        p32[0]    = u1;
        p16[2]    = w1;
        p16[3]    = w2;
        p16[4]    = w3;
        p8[10]    = b1;
        p8[11]    = b2;
        p8[12]    = b3;
        p8[13]    = b4;
        p8[14]    = b5;
        p8[15]    = b6;
    }
}

cen::uuid::uuid(const cen::uuid &id) :
    data {}
{
    memcpy(data.uuid_bytes, id.data.uuid_bytes, sizeof(data.uuid_bytes) / sizeof(data.uuid_bytes[0]));
}

cen::uuid::uuid(cen::uuid &&id) noexcept :
    data {}
{
    memcpy(data.uuid_bytes, id.data.uuid_bytes, sizeof(data.uuid_bytes) / sizeof(data.uuid_bytes[0]));
    memset(id.data.uuid_bytes, 0, sizeof(data.uuid_bytes) / sizeof(data.uuid_bytes[0]));
}

cen::uuid &cen::uuid::operator=(const cen::uuid &id)
{
    memcpy(data.uuid_bytes, id.data.uuid_bytes, sizeof(data.uuid_bytes) / sizeof(data.uuid_bytes[0]));
    return *this;
}

cen::uuid::uuid() :
    data {}
{
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
        str += char_set[data.uuid_bytes[i]];
    }
    str += "}";
    return str;
}

bool cen::operator==(const uuid &id1, const uuid &id2) noexcept
{
    return memcmp(id1.data.uuid_bytes, id2.data.uuid_bytes, sizeof(id1.data.uuid_bytes) / sizeof(id1.data.uuid_bytes[0])) == 0;
}

bool cen::operator==(const uuid &id1, const std::string &str) noexcept
{
    try
    {
        return id1 == cen::uuid { str };
    } catch (C_UNUSED const std::exception &ex)
    {
        return false;
    }
}
