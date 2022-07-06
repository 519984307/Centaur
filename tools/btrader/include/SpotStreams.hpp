/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 21/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_SPOTSTREAMS_HPP
#define CENTAUR_SPOTSTREAMS_HPP

#include "TraderGlobals.hpp"
#include <BinanceAPI.hpp>

namespace btrader
{
    class TraderApplication;
}

namespace btrader::stream
{

    class SpotStreams : public BINAPI_NAMESPACE::ws::WSSpotBinanceAPI
    {
    public:
        /// \brief This constructor does not handles any endpoint
        SpotStreams();
        /// \brief Build a mini ticker and sets the endpoint to it
        SpotStreams(TraderApplication *app, const std::string &base, const std::string &quote);
        ~SpotStreams() override = default;

    public:
        // This function is written as is in order to use ApplicationThread::startAndReleaseWithPromise
        // However, the WSSpotBinanceAPI does not set any future or promise, so we must implement that in the connected implementation
        // Futures implementations of WSThread might override this behavior and this "trick" might cause problems later, however, as of the writing of this file
        // This is not the case
        auto getStartedPromise() const -> std::promise<void> & { return m_started; }

    protected:
        void individualSymbolMiniTicker(const std::string &symbol, uint64_t eventTime, const BINAPI_NAMESPACE::StreamIndividualSymbolMiniTicker &ticker) override;

    protected: /// WSThread base methods
        // Connected must set m_started
        auto connected() -> void override;
        auto close() -> void override;
        auto connectionError() -> void override;
        void pingSent(bool success) noexcept override;

    private:
        mutable std::promise<void> m_started;
        TraderApplication *m_app;
    };
} // namespace btrader::stream

#endif // CENTAUR_SPOTSTREAMS_HPP
