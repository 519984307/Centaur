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
        uint64_t size;        /// Object data
        uint64_t timestamp;   /// Message timestamp in milliseconds
        uint64_t hash;        /// Message data hash
        uint32_t version;     /// Protocol version
        uint32_t userVersion; /// The version of the user data
        uint32_t flags;       /// ProtocolFlags
        uint32_t type;        /// Message Protocol type.
    };

    struct Generator
    {
        /// \brief Generate a protocol message
        /// \param userVersion User version
        /// \param data JSON data
        /// \remarks This function does not change modify the timestamp field in ProtocolHeader,
        ///          this field is set when the message is sent using the MessageSender object
        auto generate(uint32_t userVersion, uint32_t type, const std::string &data) -> void;

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

    namespace message
    {
        /// Predefined protocols

        /// \brief Protocol to indicate the Main Interface to accept a connection
        /// When a WS connection is accepted send this message to the main ui identify the client
        /// DIRECTION: Client -> UI
        struct Protocol_AcceptConnection : public ProtocolBase
        {
            Protocol_AcceptConnection();

            static constexpr uint32_t protocolType = 0x20bb643b;

        public:
            static constexpr char version10[] = { "Centaur_Protocol_Version10" };

            /// \brief uuid indicate
        public:
            /// \brief Connection identifier. All message from clients must use this uuid. If the main UI does not register this UUID the message will be ignored
            Field<std::string> uuid { "uuid" }; // Connection UUID
            /// Connection Name
            Field<std::string> name { "name" }; // Connection Name
            /// \brief Protocol Implementation version. For version 1.0 set this field to Protocol_AcceptConnection::version10
            Field<std::string> implementation { "implementation" }; // Connection Implementation
        };

        /// \brief Sent to the client from the main UI to indicate the status of the connection
        /// DIRECTION: UI -> Client
        struct Protocol_AcceptedConnection : public ProtocolBase
        {
            Protocol_AcceptedConnection();

            static constexpr uint32_t protocolType = 0xe1ea5df6;

            /// \brief Status codes return from the main Ui
            enum class Status : int
            {
                connectionOk,
                uuidAlreadySet,
                wrongImplementation,
            };

        public:
            /// \brief This is the UUID sent to the UI in the Protocol_AcceptConnection::uuid
            Field<std::string> uuid { "uuid" };
            /// \brief Status code return from the main UI
            Field<int> status { "status" };
        };

        /// \brief Access the plugin metadata.
        /// When a client requires a plugin to be loaded. This is the way to check the data
        /// DIRECTION: Client -> UI -> Plugin
        struct Protocol_AccessPluginMetadata : public ProtocolBase
        {
            Protocol_AccessPluginMetadata();

            static constexpr uint32_t protocolType = 0xbb837dbf;

        public:
            /// \brief Connection identifier Protocol_AcceptConnection::uuid
            Field<std::string> uuid { "uuid" };
            /// \brief Plugin to be accessed
            Field<std::string> pluginUUID { "plugin" };
            /// \brief If the plugin has the requested metadata and succeed the response will have this id
            Field<std::string> responseId { "id" };
            /// \brief If the plugin support multiple metadata this is the request
            Field<std::string> request { "request" };
        };

        /// \brief Response of the data
        /// DIRECTION: Plugin -> UI -> Client
        struct Protocol_PluginMetadataResponse : public ProtocolBase
        {
            Protocol_PluginMetadataResponse();

            static constexpr uint32_t protocolType = 0xc00047c2;

        public:
            /// The response id sent to required the data
            Field<std::string> responseId { "id" };
            /// The metadata requested
            Field<std::string> metadata { "metadata" };
        };

        /// \brief This Struct Abstracts the uuid and responseId items. Which all messages should habe
        struct Protocol_MessageBase : public ProtocolBase
        {
            Protocol_MessageBase();

            static constexpr uint32_t protocolType = 0x2e138c8a;

        public:
            /// \brief Connection identifier Protocol_AcceptConnection::uuid
            Field<std::string> uuid { "uuid" };
            /// \brief All balances request will have this Id As response
            Field<std::string> responseId { "id" };
        };
        /// \brief Access the Balances Table. This sets a new Asset in the balances list
        /// DIRECTION: Client -> UI
        struct Protocol_BalancesAsset : public Protocol_MessageBase
        {
        public:
            Protocol_BalancesAsset();

            static constexpr uint32_t protocolType = 0x20bb643b;

        public:
            /// \brief Source of the Asset
            Field<std::string> source { "source" };
            /// \brief Asset to be set
            Field<std::string> asset { "asset" };
            /// \brief This will be the handle to update the asset status
            Field<std::string> assetId { "handle" };
        };

        /// \brief Adds a new element in the Balances Tree into an Asset.
        struct Protocol_BalancesAssetItem : public Protocol_MessageBase
        {
            Protocol_BalancesAssetItem();

            static constexpr uint32_t protocolType = 0x23007f07;

        public:
            /// \brief New item name
            Field<std::string> name { "name" };
            /// \brief SubHandle name. All subsequent access to the item must be done through this id
            Field<std::string> subHandle { "subHandle" };
        };

        struct Protocol_BalanceAssetUpdate : public Protocol_MessageBase
        {
            Protocol_BalanceAssetUpdate();

            static constexpr uint32_t protocolType = 0xf469e79e;

            enum class Type : int
            {
                name,
                source,
            };

        public:
            /// \brief Asset ID to be updated
            Field<std::string> assetId { "handle" };
            /// \brief Type of the updated
            Field<int> type { "type" };
            /// \brief New value
            Field<std::string> value { "new" };
        };

        struct Protocol_BalanceAssetItemUpdate : public Protocol_MessageBase
        {
            Protocol_BalanceAssetItemUpdate();

            static constexpr uint32_t protocolType = 0xbbf781af;

            enum class Type : int
            {
                name,
                value,
            };

        public:
            /// \brief SubHandle ID to be updated
            Field<std::string> subHandle { "subHandle" };
            /// \brief Type of the update
            Field<int> type { "type" };
            /// \brief New value
            Field<std::string> value { "new" };
        };

        /// \brief All balances request have this message structure
        struct Protocol_BalancesResponse : public ProtocolBase
        {
            Protocol_BalancesResponse();

            static constexpr uint32_t protocolType = 0x77c2d52c;

            enum class Status : int
            {
                allOk,            /// Everything is Ok
                handle,           /// The Asset Handle does not exists (assetId)
                repeatedHandle,   /// The Asset handle already exists (assetId)
                subHandle,        /// The Asset SubHandle does not exists
                repeatedSubHandle /// The asset SubHandle already exists
            };

        public:
            /// The response id sent to required the data
            Field<std::string> responseId { "id" };
            /// The metadata requested
            Field<int> status { "status" };
        };
    } // namespace message
} // namespace CENTAUR_PROTOCOL_NAMESPACE

#endif // CENTAUR_PROTOCOL_HPP
