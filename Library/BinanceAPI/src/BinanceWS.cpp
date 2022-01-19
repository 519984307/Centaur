/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "BinanceWS.hpp"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wduplicate-enum"
#endif /*__clang__*/

#include <libwebsockets.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

namespace trader::api::ws::internal
{
    struct WSInfo
    {
        lws_context *m_context { nullptr };
        lws_protocols m_protocols[2];
        lws *m_lws { nullptr };
    };
} // namespace trader::api::ws::internal

trader::api::ws::WSBaseInfo::WSBaseInfo() :
    m_info { std::make_unique<internal::WSInfo>() }
{
}
