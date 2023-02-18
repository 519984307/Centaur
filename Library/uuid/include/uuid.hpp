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

#include "Centaur.hpp"
#include <array>
#include <concepts>
#include <random>
#include <span>
#include <string>

#ifdef QT_VERSION
#include <QString>
#include <QUuid>
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
        /// \param str The string must have the dashes
        /// \param checkForBrackets Check for the { } at the beginning and at the end of the string
        explicit uuid(std::string str, bool checkForBrackets = true);

        /// \brief Generates a uuid with {u1-w1-w2-b1b2b3b4b5b6}
        uuid(uint32_t u1, uint16_t w1, uint16_t w2, uint16_t w3, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, bool endiannessCheck = true) noexcept;

        // Copy constructor
        uuid(const uuid &id);

        // Move constructor
        uuid(uuid &&id) noexcept;

        uuid &operator=(const uuid &id);

        uuid();

    protected:
        uuid(uint8_t *raw, std::size_t size);

    public:
        /// \brief Returns the string bytes
        /// \return A lower case of the uuid string
        C_NODISCARD auto to_string(bool brackets = true, bool upper = false) const -> std::string;

#ifdef QT_INCLUDED
        C_NODISCARD auto to_qstring(bool brackets = true, bool upper = false) const -> QString
        {
            return QString::fromStdString(to_string(brackets, upper));
        }
        C_NODISCARD auto to_quuid(bool upper = false) const -> QUuid
        {
            // Brackets are mandatory according to QUuid documentation
            return QUuid::fromString(to_qstring(true, upper));
        }
#endif

        /// \brief Generates a random UUID
        /// \return A random UUID

        C_NODISCARD static auto generate() -> cen::uuid;

        C_NODISCARD inline unsigned char *bytes() const
        {
            return reinterpret_cast<unsigned char *>(&data.uuid_bytes);
        }

    private:
        struct CENTAUR_UUID_ALIGNMENT data_
        {
            unsigned char uuid_bytes[16]; // not null-terminated
        } mutable data {};
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
            return id1 == str.toStdString();
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
    struct hash<CENTAUR_NAMESPACE::uuid>
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
