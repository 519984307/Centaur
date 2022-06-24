/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 17/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_PROTOCOLTYPE_HPP
#define CENTAUR_PROTOCOLTYPE_HPP

#include <cstdint>

#ifndef CENTAUR_PROTOCOL_NAMESPACE
#define CENTAUR_PROTOCOL_NAMESPACE CENTAUR_NAMESPACE::protocol
#endif /*CENTAUR_PROTOCOL_NAMESPACE*/

namespace CENTAUR_PROTOCOL_NAMESPACE::type
{
    constexpr uint32_t Protocol_IncomingConnection     = 0x908ef910;
    constexpr uint32_t Protocol_AcceptConnection       = 0x1898f0a6;
    constexpr uint32_t Protocol_AcceptedConnection     = 0x7cef3eec;
    constexpr uint32_t Protocol_AccessPluginMetadata   = 0x0691be71;
    constexpr uint32_t Protocol_Icon                   = 0x105bef56;
    constexpr uint32_t Protocol_BalanceAssetItemUpdate = 0x780bce24;
    constexpr uint32_t Protocol_BalanceAssetUpdate     = 0x5c41657d;
    constexpr uint32_t Protocol_BalancesAsset          = 0x128a795a;
    constexpr uint32_t Protocol_BalancesAssetItem      = 0x44da76cf;
    constexpr uint32_t Protocol_BalanceTotalUpdate     = 0x45891093;
    constexpr uint32_t Protocol_BalancesResponse       = 0x66976a79;
    constexpr uint32_t Protocol_PluginMetadataResponse = 0x9a6c444f;

} // namespace CENTAUR_PROTOCOL_NAMESPACE::type

#endif // CENTAUR_PROTOCOLTYPE_HPP
