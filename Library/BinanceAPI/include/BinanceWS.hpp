/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/01/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_BINANCEWS_HPP
#define CENTAUR_BINANCEWS_HPP

#include <concepts>
#include <memory>
#include <string>

namespace trader::api::ws
{

    namespace internal
    {
        struct WSInfo;
    };

    struct WSBaseInfo
    {
        WSBaseInfo();

    private:
        std::unique_ptr<internal::WSInfo> m_info;
    };

    class WSBase
    {
    public:
        virtual ~WSBase() = default;

    public:
        virtual void run()             = 0;
        virtual void connected()       = 0;
        virtual void connectionError() = 0;

    private:
        WSBaseInfo m_baseInfo;
    };

} // namespace trader::api::ws

#endif // CENTAUR_BINANCEWS_HPP
