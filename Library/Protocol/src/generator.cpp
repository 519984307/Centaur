/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 20/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "Protocol.hpp"
#include "openssl/sha.h"
#include <array>
#include <string_view>
#include <zlib.h>

namespace
{
    // clang-format off
    constexpr std::array<std::string_view, 256> g_toString = {
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
    // clang-format on
    constexpr unsigned int SHA256_Size = SHA256_DIGEST_LENGTH;
} // namespace

auto CENTAUR_PROTOCOL_NAMESPACE::Generator::hash(cen::protocol::ProtocolHeader *header, const std::string &data) -> void
{
    static_assert((g_hashSize / 2) == SHA256_Size); // Hash Sizes must be equal. The division is because the hash will be stringify

    auto hash = SHA256(reinterpret_cast<const unsigned char *>(data.c_str()), data.size(), nullptr);

    auto k = 0ul;
    for (uint8_t l = 0; l < SHA256_Size; ++l)
    {
        for (auto j = 0ul; j < 2; ++j, ++k)
            header->hash[k] = g_toString[hash[l]][j];
    }
    header->hash[k] = 0;
}

auto cen::protocol::Generator::testHash(cen::protocol::ProtocolHeader *header, const std::string &data) -> bool
{
    ProtocolHeader test {};
    hash(&test, data);
    return memcmp(test.hash, header->hash, g_hashSize) == 0;
}

auto CENTAUR_PROTOCOL_NAMESPACE::Generator::compress(ProtocolHeader *header, const std::string &data, std::vector<uint8_t> &outData, int level) -> std::size_t
{
    // Preallocates based on the data size
    const std::size_t blocks  = (data.size() + 16383) / 16384; // round up any fraction of a block
    const std::size_t minSize = data.size() + 6 + (blocks * 5);

    outData.resize(minSize);

    z_stream zInfo {};
    zInfo.total_in = zInfo.avail_in = static_cast<unsigned int>(data.size());
    zInfo.total_out = zInfo.avail_out = static_cast<uInt>(minSize);
    zInfo.next_in                     = reinterpret_cast<Bytef *>(const_cast<char *>(data.data()));
    zInfo.next_out                    = outData.data();

    std::size_t size = 0;

    int nErr = deflateInit(&zInfo, level);
    if (nErr == Z_OK)
    {
        nErr = deflate(&zInfo, Z_FINISH);
        if (nErr == Z_STREAM_END)
        {
            size = static_cast<std::size_t>(zInfo.total_out);
        }
        else
            size = 0;
    }
    deflateEnd(&zInfo);

    outData.resize(size);
    outData.shrink_to_fit();

    header->flags |= PFCompressed;

    return size;
}

auto CENTAUR_PROTOCOL_NAMESPACE::Generator::decompress(ProtocolHeader *header, const uint8_t *data, std::size_t dataSize) -> std::string
{
    std::string ret;
    ret.resize(header->size);
    z_stream zInfo {};

    zInfo.total_in = zInfo.avail_in = static_cast<unsigned int>(dataSize);
    zInfo.total_out = zInfo.avail_out = static_cast<uInt>(header->size);
    zInfo.next_in                     = reinterpret_cast<Bytef *>(const_cast<Bytef *>(data));
    zInfo.next_out                    = reinterpret_cast<Bytef *>(ret.data());

    int nErr;
    uLong nRet = 0;
    nErr       = inflateInit(&zInfo);
    if (nErr == Z_OK)
    {
        nErr = inflate(&zInfo, Z_FINISH);
        if (nErr == Z_STREAM_END)
        {
            nRet = zInfo.total_out;
        }
    }

    inflateEnd(&zInfo);

    if (nRet < header->size)
        return "";

    return ret;
}

auto CENTAUR_PROTOCOL_NAMESPACE::Generator::generateHeader(cen::protocol::ProtocolHeader *header, uint64_t size, uint32_t userVersion, uint32_t type) -> void
{
    constexpr auto sizeOfMagic = sizeof(header->magic) / sizeof(header->magic[0]);
    memmove(header->magic, g_magic, sizeOfMagic);
    header->size      = size;
    header->timestamp = 0;
    // header->hash        = {0};
    header->version     = CENTAUR_PROTOCOL_VERSION;
    header->userVersion = userVersion;
    header->flags       = 0;
    header->type        = type;
}

auto CENTAUR_PROTOCOL_NAMESPACE::Generator::generate(Protocol *pro, uint32_t userVersion, ProtocolBase *data, bool compressData, int level) -> void
{
    cen::protocol::ProtocolHeader header {};

    std::size_t dataSize = 0;

    const auto json = data->json();

    generateHeader(&header, json.size(), userVersion, data->type());

    CENTAUR_PROTOCOL_NAMESPACE::Generator::hash(&header, json);

    std::vector<uint8_t> compressed;
    if (compressData)
    {
        compress(&header, json, compressed, level);
        dataSize = compressed.size();
    }
    else
    {
        dataSize = json.size();
    }

    // Get the data pointer
    const uint8_t *dataPointer = compressData ? compressed.data() : reinterpret_cast<const uint8_t *>(json.data());

    header.timestamp = CENTAUR_PROTOCOL_NAMESPACE::Generator::timestamp();

    pro->setSize(dataSize + sizeof(ProtocolHeader));

    pro->appendHeader(&header);
    pro->appendData(dataPointer, dataSize);
}

auto cen::protocol::Generator::getData(ProtocolHeader *header, const uint8_t *received, std::size_t receivedSize, const uint64_t timeDifference) -> std::string
{
    std::string emptyString;

    if (receivedSize < sizeof(ProtocolHeader))
    {
        header->flags |= PFWrongSize;
        return emptyString;
    }

    memmove(header, received, sizeof(ProtocolHeader));

    if (memcmp(received, g_magic, sizeof(header->magic) / sizeof(header->magic[0])) != 0)
    {
        header->flags |= PFWrongMagic;
        return emptyString;
    }

    if (header->version > CENTAUR_PROTOCOL_VERSION)
    {
        header->flags |= PFWrongVersion;
        return emptyString;
    }

    auto timestampDifference = timestamp() < header->timestamp /*This is an inconsistency in timestamps. Probably different time zones are used*/
                                   ? timeDifference + 1
                                   : timestamp() - header->timestamp;
    if (timestampDifference > timeDifference)
    {
        header->flags |= PFWrongTimestamp;
        return emptyString;
    }

    std::string protocolData;

    if (header->flags & PFCompressed)
    {
        protocolData = decompress(header, received + sizeof(ProtocolHeader), receivedSize - sizeof(ProtocolHeader));
        if (protocolData.size() != header->size)
        {
            header->flags |= PFWrongMessageSize;
            return emptyString;
        }
    }
    else
    {
        if ((receivedSize - sizeof(ProtocolHeader)) != header->size)
        {
            if (protocolData.size() != header->size)
            {
                header->flags |= PFWrongMessageSize;
                return emptyString;
            }
        }
        protocolData = std::string { reinterpret_cast<const char *>(received + sizeof(ProtocolHeader)), header->size };
    }

    if (!testHash(header, protocolData))
    {
        header->flags |= PFWrongHash;
        return emptyString;
    }

    return protocolData;
}

auto cen::protocol::Generator::timestamp() -> uint64_t
{
    return static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

cen::protocol::Protocol::~Protocol()
{
    delete[] data;
    data = nullptr;
}

void cen::protocol::Protocol::setSize(std::size_t total)
{
    valid = false;
    // Release any previous allocations
    if (data != nullptr)
    {
        delete[] data;
        data = nullptr;
    }
    size = total;
    data = new uint8_t[size];
}

auto cen::protocol::Protocol::appendHeader(cen::protocol::ProtocolHeader *header) -> void
{
    valid = false;
    memmove(data, header, sizeof(ProtocolHeader));
}

auto cen::protocol::Protocol::appendData(const uint8_t *message, std::size_t dataSize) -> void
{
    memmove(data + sizeof(ProtocolHeader), message, dataSize);
    valid = true;
}
