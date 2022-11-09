/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_TOOLSTHREAD_HPP
#define CENTAUR_TOOLSTHREAD_HPP

#include <any>
#include <chrono>
#include <concepts>
#include <future>
#include <optional>
#include <queue>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace cen::tools
{

    template <typename U, uint64_t Ts>
    struct TimerThread
    {
        static_assert(!(Ts == 0));
        virtual ~TimerThread() = default;

    public:
        auto terminate() -> void
        {
            m_terminateSignal = true;
            m_waitCondition.notify_one();
        }

        auto run() -> void
        {
            m_started.set_value();

            while (!m_terminateSignal)
            {
                // Protect Race Conditions
                std::unique_lock<std::mutex> lock { m_dataProtect };
                m_waitCondition.wait_for(lock, m_duration, [&]() { return m_terminateSignal; });

                if (!m_terminateSignal)
                {
                    timerEvent();
                }
            }
        }

    public:
        virtual auto timerEvent() noexcept -> void = 0;
        auto getStartedPromise() const -> std::promise<void> & { return m_started; }

    private:
        U m_duration { Ts };
        std::mutex m_dataProtect;
        std::condition_variable m_waitCondition;
        bool m_terminateSignal { false };
        mutable std::promise<void> m_started;
    };

    template <uint64_t Ts>
    using TimerEventSecs = TimerThread<std::chrono::seconds, Ts>;
    template <uint64_t Ts>
    using TimerEventMS = TimerThread<std::chrono::milliseconds, Ts>;
    template <uint64_t Ts>
    using TimerEventUS = TimerThread<std::chrono::microseconds, Ts>;
    template <uint64_t Ts>
    using TimerEventNS = TimerThread<std::chrono::nanoseconds, Ts>;
    template <uint64_t Ts>
    using TimerEventMinutes = TimerThread<std::chrono::minutes, Ts>;
    template <uint64_t Ts>
    using TimerEventHours = TimerThread<std::chrono::hours, Ts>;

    template <typename... Args>
    struct Thread
    {
        virtual ~Thread() = default;

        using messages    = std::variant<Args...>;

    public:
        auto preventStartedAtRun() -> void { m_startedAtRun = false; }

        auto run() -> void
        {
            if (m_startedAtRun)
                m_started.set_value();

            while (!m_terminateSignal)
            {
                // Protect Race Conditions
                std::unique_lock<std::mutex> lock { m_dataProtect };
                m_waitCondition.wait(lock, [&]() { return m_terminateSignal || !m_messages.empty(); });

                while (!m_messages.empty())
                {
                    acquire(m_messages.front());
                    m_messages.pop();
                }
            }

            onTerminate();
        }
        template <typename U>
        auto sendMessage(U &&type) -> void
        {
            std::lock_guard<std::mutex> lock { m_dataProtect };
            m_messages.push(type);
            m_waitCondition.notify_one();
        }

        template <typename U, typename... UArgs>
        auto sendMessage(UArgs &&...args) -> void
        {
            std::lock_guard<std::mutex> lock { m_dataProtect };
            m_messages.push(U { args... });
            m_waitCondition.notify_one();
        }

        template <typename U>
        auto localSend(U &&type) -> void
        {
            m_messages.push(type);
            m_waitCondition.notify_one();
        }

        template <typename U, typename... UArgs>
        auto localSend(UArgs &&...args) -> void
        {
            m_messages.push(U { args... });
            m_waitCondition.notify_one();
        }

        template <typename U>
        auto getMessage(const messages &t) -> std::optional<U>
        {
            return std::visit([&](auto &&arg) {
                using N = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<N, U>)
                    return std::make_optional<U>(arg);
                return std::optional<U>(std::nullopt_t(std::nullopt));
            },
                t);
        }

        auto terminate() -> void
        {
            std::lock_guard<std::mutex> lock { m_dataProtect };
            m_terminateSignal = true;
            m_waitCondition.notify_one();
        }

        virtual auto acquire(const messages &s) noexcept -> void = 0;
        virtual auto onTerminate() noexcept -> void              = 0;

        auto getStartedPromise() const -> std::promise<void> & { return m_started; }

    private:
        mutable std::promise<void> m_started;
        bool m_startedAtRun { true };
        std::queue<messages> m_messages {};
        std::mutex m_dataProtect {};
        std::condition_variable m_waitCondition {};
        std::atomic_bool m_terminateSignal { false };
    };

    template <typename T>
    struct ApplicationThread
    {
        using Tptr          = T *;
        ApplicationThread() = default;
        inline ~ApplicationThread() noexcept
        {
            release();
        }

        inline auto release()
        {
            if (object)
            {
                object->terminate();
                if (thisThread.joinable())
                    thisThread.join();
                object.reset();
            }
        }

        // Start the thread releasing the memory and closing the thread if there was a prior thread running
        template <typename... Args>
        inline auto startAndRelease(Args &&...args)
        {
            release();
            object = std::make_unique<T>(std::forward<Args>(args)...);
            if (object)
            {
                thisThread = std::thread([&] {
                    object->run();
                });
            }
        }

        template <typename... Args>
        inline auto startAndReleaseWithPromise(Args &&...args)
        {
            release();
            object = std::make_unique<T>(std::forward<Args>(args)...);
            if (object)
            {
                std::future<void> started = object->getStartedPromise().get_future();
                thisThread                = std::thread([&] {
                    object->run();
                });
                started.wait();
            }
        }

        inline auto isValid() const noexcept -> bool
        {
            return object.operator bool();
        }

        inline auto thisObject() noexcept -> Tptr
        {
            return object.get();
        }

    private:
        std::thread thisThread;
        std::unique_ptr<T> object;
    };

} // namespace cen::tools

#define DELEGATE(r, p, ...) SA::delegate<r(__VA_ARGS__)>::create<thisClass, p>(this)
#endif // CENTAUR_TOOLSTHREAD_HPP
