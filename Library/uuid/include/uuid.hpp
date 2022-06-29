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

#include "../../Centaur.hpp"
#include <array>
#include <concepts>
#include <random>
#include <string>

#ifdef QT_VERSION
#include <QString>
#define QT_INCLUDED
#endif /*QT_VERSION*/

#define CENTAUR_VALID_UUID_GENERATORS std::same_as<std::mt19937, generator> || std::same_as<std::mt19937_64, generator> || std::same_as<std::minstd_rand, generator> || std::same_as<std::minstd_rand0, generator> || std::same_as<std::ranlux24, generator> || std::same_as<std::ranlux48, generator>

#ifndef CENTAUR_UUID_ALIGNMENT
#if defined(__clang__) || defined(__GNUC__)
#define CENTAUR_UUID_ALIGNMENT __attribute__((aligned(sizeof(uint64_t))))
#elif defined MSVC
#define CENTAUR_UUID_ALIGNMENT __declspec(align(8))
#else
#define CENTAUR_UUID_ALIGNMENT
#endif /*defined(__clang__) || defined(__GNUC__)*/
#endif /*CENTAUR_UUID_ALIGNMENT*/

namespace CENTAUR_NAMESPACE
{

    struct uuid
    {
        /// \brief Construct an uuid from a string
        /// \param str The string must have the curly braces and dashes
        explicit uuid(const std::string &str);

        uuid(const uint32_t u1, const uint16_t w1, const uint16_t w2, const uint16_t w3, const uint8_t b1, const uint8_t b2, const uint8_t b3, const uint8_t b4, const uint8_t b5, const uint8_t b6, bool endiannessCheck = true) noexcept;

        // Copy constructor
        uuid(const uuid &id);

        // Move constructor
        uuid(uuid &&id) noexcept;

        uuid &operator=(const uuid &id);

        uuid();

    public:
        /// \brief Returns the string bytes
        /// \return A lower case of the uuid string
        C_NODISCARD auto to_string(bool upper = false) const -> std::string;

        /// \brief Generates a random UUID
        /// \tparam generator A valid c++ random generator (mt19937, mt19937_64, minstd_rand, minstd_rand0, ranlux24, ranlux48)
        /// \return A random UUID
        template <typename generator>
        C_NODISCARD static auto generate() -> cen::uuid
            requires CENTAUR_VALID_UUID_GENERATORS
        {
            std::random_device rd;
            generator gen(rd());
            std::uniform_int_distribution<std::size_t> distrib_u(0, 0xffffffffffff);

            return cen::uuid {
                static_cast<const uint32_t>(distrib_u(gen)),
                static_cast<const uint16_t>(distrib_u(gen)),
                static_cast<const uint16_t>(distrib_u(gen)),
                static_cast<const uint16_t>(distrib_u(gen)),
                static_cast<const uint8_t>(distrib_u(gen)),
                static_cast<const uint8_t>(distrib_u(gen)),
                static_cast<const uint8_t>(distrib_u(gen)),
                static_cast<const uint8_t>(distrib_u(gen)),
                static_cast<const uint8_t>(distrib_u(gen)),
                static_cast<const uint8_t>(distrib_u(gen)),
                false
            };
        }

        C_NODISCARD inline unsigned char *bytes() const
        {
            return reinterpret_cast<unsigned char *>(&data.uuid_bytes);
        }

    private:
        // clang-format off
        static constexpr std::array<std::string_view, 256> uuid_chars_lower = {
            "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
            "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
            "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
            "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
            "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
            "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
            "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
            "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
            "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
            "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
            "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
            "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
            "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
            "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df",
            "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
            "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
        };
        static constexpr std::array<std::string_view, 256> uuid_chars_upper = {
            "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
            "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
            "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
            "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
            "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
            "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
            "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
            "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
            "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
            "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
            "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
            "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
            "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
            "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
            "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
            "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
        };
        static constexpr std::array<char, 25> valid_chars = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', '-'

        };
        // clang-format on

    private:
        struct CENTAUR_UUID_ALIGNMENT data_
        {
            unsigned char uuid_bytes[16]; // not null-terminated
        } mutable data;
        friend bool operator==(const uuid &id1, const uuid &id2) noexcept;

        // Make sure reinterpret_cast<const uint64_t *>(id.bytes()) in the std::hash<> does not misbehave
        static_assert(std::alignment_of<data_>::value >= std::alignment_of<std::uint64_t>::value, "Invalid UUID data alignment");

    public:
        inline bool operator<(const uuid &id) const
        {
            auto thisBytes = reinterpret_cast<const uint64_t *>(bytes());
            auto u1        = reinterpret_cast<const uint64_t *>(id.bytes());
            return std::tie(thisBytes[0], thisBytes[1]) < std::tie(u1[0], u1[1]);
        }
    };

    bool operator==(const uuid &id1, const uuid &id2) noexcept;

    bool operator==(const uuid &id1, const std::string &str) noexcept;

#ifdef QT_INCLUDED
    inline bool operator==(const uuid &id1, const QString &str) noexcept
    {
        try
        {
            return id1 == cen::uuid { str.toStdString() };
        } catch (C_UNUSED const std::exception &ex)
        {
            return false;
        }
    }
#endif

} // namespace CENTAUR_NAMESPACE

#ifndef C_NO_UUID_HASH
namespace std
{
    template <>
    struct hash<CENTAUR_NAMESPACE::uuid> : public std::unary_function<const CENTAUR_NAMESPACE::uuid &, std::size_t>
    {
        inline std::size_t operator()(const CENTAUR_NAMESPACE::uuid &id) const
        {
            auto u1 = reinterpret_cast<const uint64_t *>(id.bytes());
            std::hash<uint64_t> hs;
            return hs(u1[0]) ^ hs(u1[1]);
        }
    };
} // namespace std
#endif /*C_NO_UUID_HASH*/

#endif // CENTAUR_UUID_HPP
