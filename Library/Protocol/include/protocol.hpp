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

#include "Centaur.hpp"
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

#include "ProtocolType.hpp"

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
    constexpr char g_hashSize = 64;

    enum ProtocolFlags
    {
        PFCompressed = 1 << 0, // Beware that only the JSON Data is compressed
        // Error Flags set when Generator::getData are called
        PFWrongSize      = 1 << 1,   // The size of the data received is less than sizeof(ProtocolHeader)
        PFWrongMagic     = 1 << 2,   // The size of the data received is consistent to the expected, however, the magic is wrong
        PFWrongTimestamp = 1 << 3,   // The time difference is higher than the set in Generator::getData
                                     // Make sure time zones are equal in the client and the server, otherwise, time inconsistencies may arise and this flag may or may not be set.
                                     // If this flag is set, communication between server and client has too much latency
        PFWrongMessageSize = 1 << 4, // The whole data size minus the header size is not what ProtocolHeader::size was expected
        PFWrongHash        = 1 << 5, // The hash was not consistent, therefore, the data was corrupted
        PFWrongVersion     = 1 << 6, // The version of the message is not supported by the current library version
    };

    struct ProtocolHeader
    {
        char magic[4];           /// Magic number
        uint64_t size;           /// Object data. This is the size of the uncompressed data. This is used by decompress if compression was set
        uint64_t timestamp;      /// Message timestamp in milliseconds
        int8_t hash[g_hashSize]; /// Message data hash. IMPORTANT: This is not a null-terminated string
        uint32_t version;        /// Protocol version
        uint32_t userVersion;    /// The version of the user data
        uint32_t flags;          /// ProtocolFlags
        uint32_t type;           /// Message Protocol type.
    };

    /// \brief This structs contains the whole message
    /// \remarks Only call get(). The data is set by Generator::generate()
    struct Protocol
    {
        ~Protocol();

    public:
        auto appendHeader(ProtocolHeader *header) -> void;
        auto appendData(const uint8_t *message, std::size_t dataSize) -> void;
        auto setSize(std::size_t total) -> void;

    public:
        /// Data will only be valid once all data is set in the buffer
        C_NODISCARD inline auto isValid() const -> bool { return valid; }
        C_NODISCARD inline auto getSize() const -> std::size_t { return size; }
        C_NODISCARD inline auto get() const -> uint8_t * { return data; }

    protected:
        bool valid { false };
        uint8_t *data { nullptr };
        std::size_t size { 0 };
    };

    struct ProtocolBase;
    struct Generator
    {

        /// \brief For consistency in timestamps use this function
        /// \return timestamp
        C_NODISCARD static auto timestamp() -> uint64_t; // TODO: MAKE THIS FUNCTION UTC-0

        /// \brief Calculate the hash (SHA256) of the data to be sent
        /// \param header This is the main basic header. This function will set the hash value onto de header
        /// \param data Data to be hashed
        static auto hash(ProtocolHeader *header, const std::string &data) -> void;

        /// \brief Computes the SHA-256 of data and compares the value to the provided by the header->hash
        /// \param header Header containing a valid hash
        /// \param data Data to be compared
        /// \return True if are equal, false otherwise
        static auto testHash(ProtocolHeader *header, const std::string &data) -> bool;

        /// \brief Compress the data to be sent
        /// \param header This is the main basic header. This function will set flags |= ProtocolFlags::PFCompressed
        /// \param data Data to be compressed
        /// \param level Is the compression level is a number between -1 and 9. see zlib compression levels for more details
        /// \return The compression size
        /// \remarks Consider
        static auto compress(ProtocolHeader *header, const std::string &data, std::vector<uint8_t> &outData, int level = 6) -> std::size_t;

        /// \brief Decompress the data to be sent or sent
        /// \param data Data to be decompressed
        /// \return A string with the compressed data
        C_NODISCARD static auto decompress(ProtocolHeader *header, const uint8_t *data, std::size_t dataSize) -> std::string;

        /// \brief This function will set the basic data to the header
        /// \param header This is the main basic header.
        /// \param size Size of the data
        /// \param userVersion user version
        /// \param type Message Protocol type
        static auto generateHeader(ProtocolHeader *header, uint64_t size, uint32_t userVersion, uint32_t type) -> void;

        /// \brief Generate a protocol message.
        /// \param pro A ready to send to data
        /// \param userVersion User version
        /// \param data Message real payload
        /// \param compressData Set  to true if you want data to be compressed. The level parameter becomes relevant if compressData is set to true
        /// \param level Is the compression level is a number between -1 and 9. see zlib compression levels for more details
        /// \remarks The timestamp will set to the time this function is called and just before return.
        /// \remarks THIS IS THE FUNCTION YOU WILL NORMALLY CALL TO GENERATE A MESSAGE PROTOCOL
        static auto generate(Protocol *pro, uint32_t userVersion, ProtocolBase *data, bool compressData = false, int level = 6) -> void;

        /// \brief This function will separate the data
        /// \param header General package information. Useful to see the error flags
        /// \param received This is an IN parameter and it is the data received
        /// \param receivedSize This is an IN parameter and it is the size of the data received
        /// \param timeDifference Checks whether the timestamps difference is ok
        /// \remarks All problems are set in the header->flags parameter. See ProtocolFlags for further details.
        /// \remarks If the data is compressed. This function decompress it
        C_NODISCARD static auto getData(ProtocolHeader *header, const uint8_t *received, std::size_t receivedSize, const uint64_t timeDifference) -> std::string;

    private:
        std::shared_ptr<uint8_t[]> m_data;
    };

    /// \brief Object encryption as well af the generation of public and private keys
    struct Encryption final
    {
    public:
        Encryption();
        ~Encryption();

    public:
        auto loadKeyPair(const std::string &privateFile, const std::string &publicFile, const std::string &passphrase) -> void;

        /// \brief Encrypt with the private key
        /// \param plainText Data to encrypt
        /// \return Return Base64 or Base156 according
        auto encryptPrivate(const std::string &plainText) -> std::string;

        auto decryptPrivate(const std::string &cipherText) -> std::string;

        auto encryptPublic(const std::string &plainText) -> std::string;

        /// \brief Decrypt with the public key
        /// \param cipherText Data to decrypt
        /// \return an array of the decrypted data
        auto decryptPublic(const std::string &cipherText) -> std::string;

    public:
        /// \throws std::runtime_error when an error occur
        static auto generateRSAPrivatePublicKeys(const std::string &privateFile, const std::string &publicFile, const std::string &passphrase) -> void;

        static auto toBase64(const std::string &str) -> std::string;

        static auto toBase16(const std::string &str) -> std::string;

        static auto fromBase64(const std::string &str) -> std::string;

        static auto fromBase16(const std::string &str) -> std::string;

    private:
        struct Impl;
        std::unique_ptr<Impl> pimpl;
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
            field_name { name } { }

        T &operator()()
        {
            return data;
        }

        T &operator()() const
        {
            return data;
        }

        C_NODISCARD auto name() const -> std::string_view { return field_name; }

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
                T>
        auto
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
        explicit ProtocolBase(uint32_t type) noexcept;

        C_NODISCARD auto json() -> std::string;
        auto fromJson(const char *json) -> void;
        C_NODISCARD auto type() const -> uint32_t;

    protected:
        uint32_t protocolType {};

    protected:
        ProtocolJSONGenerator generator;
    };

    namespace message
    {
        /// Predefined protocols

        /// \brief This message is sent by the server, the moment, the connection is stablished
        /// You must use this uuid when communication back to the server.
        /// When the client receive this message, the client must respond with a Protocol_AcceptConnection
        /// If the client send data with the wrong Id or with non Protocol_AcceptConnection message
        /// The connection will be closed
        /// DIRECTION: UI -> Client
        struct Protocol_IncomingConnection : ProtocolBase
        {
            Protocol_IncomingConnection();

        public:
            Field<std::string> uuid { "uuid" };
        };

        /// \brief Protocol to indicate the Main Interface to accept a connection
        /// When a WS connection is accepted send this message to the main ui identify the client
        /// DIRECTION: Client -> UI
        struct Protocol_AcceptConnection : public ProtocolBase
        {
            Protocol_AcceptConnection();

            /// \brief uuid indicate
        public:
            /// \brief Connection identifier. All message from clients must use this uuid. If the main UI does not register this UUID the message will be ignored
            Field<std::string> uuid { "uuid" }; // Connection UUID
            /// Connection Name
            Field<std::string> name { "name" }; // Connection Name
        };

        /// \brief Sent to the client from the main UI to indicate the status of the connection
        /// DIRECTION: UI -> Client
        struct Protocol_AcceptedConnection : public ProtocolBase
        {
            Protocol_AcceptedConnection();

        public:
            /// \brief This is the UUID sent to the UI in the Protocol_AcceptConnection::uuid
            Field<std::string> uuid { "uuid" };
        };

        /// \brief Access the plugin metadata.
        /// When a client requires a plugin to be loaded. This is the way to check the data
        /// DIRECTION: Client -> UI -> Plugin
        struct Protocol_AccessPluginMetadata : public ProtocolBase
        {
            Protocol_AccessPluginMetadata();

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

        public:
            /// The response id sent to required the data
            Field<std::string> responseId { "id" };
            /// The metadata requested
            Field<std::string> metadata { "metadata" };
        };

        /// \brief This Struct Abstracts the uuid and responseId items. Which all messages should habe
        struct Protocol_MessageBase : public ProtocolBase
        {
            explicit Protocol_MessageBase(uint32_t type);

        public:
            /// \brief Connection identifier Protocol_AcceptConnection::uuid
            Field<std::string> uuid { "uuid" };
            /// \brief All balances request will have this Id As response
            Field<std::string> responseId { "id" };
        };

        /// \brief This messages send an icon encoded on Base64
        struct Protocol_Icon : public Protocol_MessageBase
        {
            Protocol_Icon();

            enum IconFormat : int
            {
                IF_BMP, // Windows Bitmap
                IF_GIF, // Graphic Interchange Format (optional)
                IF_JPG, // Joint Photographic Experts Group
                IF_PNG, // Portable Network Graphics
                IF_PBM, // Portable Bitmap
                IF_PGM, // Portable Graymap
                IF_PPM, // Portable Pixmap
                IF_XBM, // X11 Bitmap
                IF_XPM, // X11 Bitmap
                IF_SVG  // Scalable Vector Graphics
            };

        public:
            /// \brief Icon identifier. This will be later be used to reference to this icon
            Field<std::string> iconId { "icon" };
            /// \brief Icon data. Be sure is Base64 encoded
            Field<std::string> data { "data" };
            /// \brief Format, based on IconFormat enum
            Field<int> format { "format" };
        };

        /// \brief Access the Balances Table. This sets a new Asset in the balances list
        /// DIRECTION: Client -> UI
        struct Protocol_BalancesAsset : public Protocol_MessageBase
        {
        public:
            Protocol_BalancesAsset();

        public:
            /// \brief A user can have multiple "instances" of the same asset with the different amount
            /// So next to the asset name, in the tree, this is the total of an asset balance. Beware, that interface
            /// assumes this value is in USD so, the interface can calculate the totals in a uniform manner.
            /// If a client sends this value in a non-usd related value the values displayed in the total value will not be correct
            Field<std::string> total { "total" };
            /// \brief Asset to be set
            Field<std::string> asset { "asset" };
            /// \brief This will be the handle to update the asset status
            Field<std::string> assetId { "handle" };
            /// \brief Set this variable if you want the UI server automatically use the internal icons. Otherwise set this value to empty
            /// The value must be an uppercase valid asset name
            Field<std::string> assetIcon { "icon" };
        };

        /// \brief Adds a new element in the Balances Tree into an Asset
        /// DIRECTION: Client -> UI
        struct Protocol_BalancesAssetItem : public Protocol_MessageBase
        {
            Protocol_BalancesAssetItem();

        public:
            /// \brief This is the name of the value
            Field<std::string> name { "name" };
            /// \brief This is the value of the field
            Field<std::string> value { "value" };
            /// \brief This is parent item handle
            Field<std::string> handle { "handle" };
            /// \brief SubHandle name. All subsequent access to the item must be done through this id
            Field<std::string> subHandle { "subHandle" };
            /// \brief Icon identifier. Send empty if no icon is to be used
            Field<std::string> icon { "icon" };
        };

        /// \brief Updates an asset item in the tree
        struct Protocol_BalanceAssetUpdate : public Protocol_MessageBase
        {
            Protocol_BalanceAssetUpdate();

        public:
            /// \brief Asset ID to be updated
            Field<std::string> assetId { "handle" };
            /// \brief New value
            Field<std::string> value { "new" };
        };

        struct Protocol_BalanceTotalUpdate : public Protocol_MessageBase
        {
            Protocol_BalanceTotalUpdate();

        public:
            /// \brief Asset ID to be updated
            Field<std::string> assetId { "handle" };
            /// \brief Field value to be displayed
            Field<std::string> display { "display" };
            /// \brief Total value in floating point format
            Field<double> total { "total" };
        };

        struct Protocol_BalanceAssetItemUpdate : public Protocol_MessageBase
        {
            Protocol_BalanceAssetItemUpdate();

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
