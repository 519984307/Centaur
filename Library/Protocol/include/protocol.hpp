/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PROTOCOL_HPP
#define CENTAUR_PROTOCOL_HPP

#include "../../Centaur.hpp"
#include <concepts>
#include <map>
#include <string>
#include <variant>
#include <vector>

#ifndef CENTAUR_PROTOCOL_VERSION
#define CENTAUR_PROTOCOL_VERSION_MAJOR 0
#define CENTAUR_PROTOCOL_VERSION_MINOR 1
#define CENTAUR_PROTOCOL_VERSION_PATCH 0
#define CENTAUR_PROTOCOL_VERSION       CENTAUR_VERSION_CODE(CENTAUR_PROTOCOL_VERSION_MAJOR, CENTAUR_PROTOCOL_VERSION_MINOR, CENTAUR_PROTOCOL_VERSION_PATCH)
#endif /*CENTAUR_PROTOCOL_VERSION*/

#ifndef CENTAUR_PROTOCOL_NAMESPACE
#define CENTAUR_PROTOCOL_NAMESPACE CENTAUR_NAMESPACE::protocol
#endif /*CENTAUR_PROTOCOL_NAMESPACE*/

/** ------------------------------------------------------------------------------------------
 * \brief Main protocol definitions
 *
 * Communication Protocol
 *
 * Main protocola message information:
 * |------HEADER-------||-------JSON-DATA-------|
 *
 *
 * The struct ProtocolJSONGenerator is helper for generating the JSON string from some named fields associated with valid json type
 * without the need to construct a valid JSON object
 *
 * For example
 * ProtocolJSONGenerator pjg;
 *
 */

namespace CENTAUR_PROTOCOL_NAMESPACE
{
    constexpr char g_magic[4] = { 'C', 'E', 'N', 'T' };

    enum ProtocolFlags
    {
        PFCompressed = 0x1, // Beware that only the JSON Data is compressed
    };

    struct ProtocolHeader
    {
        char magic[4];        /// Magic number
        uint32_t version;     /// Protocol version
        uint32_t userVersion; /// The version of the user data
        uint64_t size;        /// Object data
        uint64_t timestamp;   /// Message timestamp in milliseconds
        uint64_t hash;        /// Message data hash
        uint32_t flags;       /// ProtocolFlags
    };

    struct Generator
    {
        /// \brief Generate a protocol message
        /// \param userVersion User version
        /// \param data JSON data
        /// \remarks This function does not change modify the timestamp field in ProtocolHeader,
        ///          this field is set when the message is sent using the MessageSender object
        auto generate(uint32_t userVersion, const std::string &data) -> void;

    private:
        std::shared_ptr<uint8_t[]> m_data;
    };

    /// \brief Object encryption as well af the generation of public and private keys
    struct Encryption final
    {
        enum class BinaryBase
        {
            Base64,
            Base16, /// Hexadecimal format
        };

    public:
        Encryption();
        ~Encryption();

    public:
        /// \brief Loads and validates a private key. Expect an exception if an error occur
        /// \param file File name
        auto loadPrivateKey(const std::string &file) -> void;

        /// \brief Loads and validates a public key. Expect an exception if an error occur
        /// \param file File name
        auto loadPublicKey(const std::string &file) -> void;

        /// \brief Encrypt with the private key
        /// \param plainText Data to encrypt
        /// \param base Return data in this formatted string
        /// \return Return Base64 or Base156 according
        auto encryptPrivate(const std::string &plainText, const BinaryBase base) -> std::string;

        auto decryptPrivate(const std::string &cipherText, const BinaryBase base) -> std::string;

        auto encryptPublic(const std::string &plainText, const BinaryBase base) -> std::string;

        /// \brief Decrypt with the public key
        /// \param cipherText Data to decrypt
        /// \return an array of the decrypted data
        auto decryptPublic(const std::string &cipherText, const BinaryBase base) -> std::string;

    private:
        struct Impl;
        std::unique_ptr<Impl> pimpl;
    };

    /// \brief Data compression
    struct Compressor
    {
    };

    template <typename T>
    struct json_null_type
    {
        using json_type = T;
    };

    using json_null_type_t = json_null_type<void>;

    template <typename T>
        requires std::integral<T> || std::floating_point<T> || std::same_as<std::string, T> || std::same_as<json_null_type_t, T>
    struct Field
    {
        using field_type = T;

    public:
        constexpr explicit Field(std::string_view name) :
            field_name { std::move(name) } { }

        T &operator()()
        {
            return data;
        }

        T &operator()() const
        {
            return data;
        }

        auto name() const -> std::string_view { return field_name; }

    protected:
        std::string_view field_name;
        T data;
    };

    struct ProtocolJSONGenerator
    {
    private:
        // Valid types
        using generator_types = std::variant<Field<double> *, Field<float> *, Field<uint64_t> *, Field<int64_t> *, Field<uint32_t> *, Field<int32_t> *, Field<uint16_t> *, Field<int16_t> *, Field<uint8_t> *, Field<int8_t> *, Field<bool> *, Field<std::string> *>;

    public:
        template <typename T>
            requires std::derived_from<Field<typename Field<typename T::field_type>::field_type>,
                T> auto
            addField(T *f) -> void
        {
            m_types.push_back({ f });
        }

        // Generate the json string from the fields set
        C_NODISCARD auto getJSON() -> std::string;

        auto fromJSON(const char *json) -> void;

    protected:
        std::vector<generator_types> m_types;
    };

    struct ProtocolBase
    {
        C_NODISCARD auto json() -> std::string;
        auto fromJson(const char *json) -> void;

    protected:
        ProtocolJSONGenerator generator;
    };

    // Predefined protocols
    struct Protocol_AcceptConnection : public ProtocolBase
    {
        Protocol_AcceptConnection();

    public:
        Field<std::string> uuid { "uuid" };                     // Connection UUID
        Field<std::string> name { "name" };                     // Connection Name
        Field<std::string> implementation { "implementation" }; // Connection Implementation
    };

} // namespace CENTAUR_PROTOCOL_NAMESPACE

#endif // CENTAUR_PROTOCOL_HPP
