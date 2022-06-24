/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 21/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_APPLICATIONMESSAGES_HPP
#define CENTAUR_APPLICATIONMESSAGES_HPP

namespace btrader
{
    enum class TimeoutType
    {
        acceptConnection = 0,
    };

    template <typename T>
    struct CentaurConnectionMessageT
    {
        using message_type = T;
    };
    template <typename T>
    struct FailedToConnectT
    {
        using message_type = T;
    };
    template <typename T>
    struct ClosedConnectionT
    {
        using message_type = T;
    };
    template <typename T>
    struct TimeoutMessageT
    {
        using message_type = T;
    };

    struct TickerUpdate
    {
        std::string asset;
        double value;
    };

    using CentaurConnectionMessage = CentaurConnectionMessageT<void>;
    using CentaurConnectedMessage  = CentaurConnectionMessageT<bool>;
    using FailedToConnect          = FailedToConnectT<void>;
    using ClosedConnection         = ClosedConnectionT<void>;
    using TimeoutMessage           = TimeoutMessageT<void>;
} // namespace btrader

#define APPLICATION_MESSAGES \
    CentaurConnectionMessage, CentaurConnectedMessage, FailedToConnect, ClosedConnection, TimeoutMessage, TickerUpdate

#endif // CENTAUR_APPLICATIONMESSAGES_HPP
