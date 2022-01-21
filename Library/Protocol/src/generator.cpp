/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 20/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "protocol.hpp"

auto CENTAUR_NAMESPACE::protocol::Generator::generate(uint32_t userVersion, const std::string &data) -> void
{
    ProtocolHeader ph {
        .magic       = { g_magic[0], g_magic[1], g_magic[2], g_magic[3] },
        .version     = CENTAUR_PROTOCOL_VERSION,
        .userVersion = userVersion,
        .size        = data.size(),
        .timestamp   = 0,
        .hash        = 0
    };

    const uint64_t totalSize = ph.size + sizeof(ProtocolHeader);

    std::shared_ptr<uint8_t[]> local { new uint8_t[totalSize] };

    // Copy the data
    memmove(local.get(), &ph, sizeof(ph));
    memmove(local.get() + sizeof(ph), data.data(), data.size());

    m_data = local;
}
