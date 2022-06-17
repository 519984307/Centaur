/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 15/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

/// \remarks THIS CLASS USES Google RE2 and {FMT} libraries

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_TOOLSLOGGER_HPP
#define CENTAUR_TOOLSLOGGER_HPP

#include "Tools.hpp"
#include "ToolsThread.hpp"
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <re2/re2.h>

namespace cen::tools
{
    struct LoggerEvent
    {
        uint64_t dateTime;
        uint32_t eventId;
        uint32_t coinId;
        std::string message;
        bool preventCoinFile; // When coindId != 0. This flags prevents logs to be flushed to the coin file handle, instead is logged into the general log file
    };

    typedef struct EventName_t
    {
        std::string name;
        fmt::rgb color;
    } EventName, CoinName;

    template <uint32_t FlushSeconds>
    class ToolLogger final : public Thread<LoggerEvent>
    {
    protected:
        static constexpr uint32_t flushId = 0x0F003113;

    public:
        explicit ToolLogger(const std::string outputPath) :
            m_outputPath { std::move(outputPath) },
            m_timer { [&fe = m_flushEvent] {
                fe.run();
            } }
        {
            openFile();

            // Make 1K of space for the buffer
            // Rarely this size will be exceeded
            m_consoleBuffer.reserve(1024);
            m_fileBuffer.reserve(1024);
        }
        ~ToolLogger() override
        {
            assert(m_logFile not_eq nullptr);

            m_flushEvent.terminate();
            m_timer.join();
            fclose(m_logFile);
            for (const auto &[id, file] : m_coinFiles)
            {
                assert(file not_eq nullptr);
                fflush(file);
            }
        }

    public:
        auto addEventName(const uint32_t &id, EventName &&name) -> std::size_t
        {
            m_eventNames.insert({ id, name });
            return m_eventNames.size();
        }

        auto addCoinName(const uint32_t &id, CoinName &&name) -> std::size_t
        {
            // Open local files
            openFile(true, id, name.name);

            m_coinNames.insert({ id, name });

            return m_eventNames.size();
        }

    private:
        struct FlushTimeEvent : public TimerEventSecs<FlushSeconds>
        {
            explicit FlushTimeEvent(ToolLogger *log) :
                m_logger { log }
            {
                m_logger->addEventName(flushId, { "flush", fmt::color::orchid });
            }

            ~FlushTimeEvent() override = default;

        protected:
            auto timerEvent() noexcept -> void override
            {
                assert(m_logger not_eq nullptr);

                m_logger->flush();
                m_logger->log(LoggerEvent {
                    .dateTime = static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
                    .eventId  = flushId,
                    .coinId   = 0,
                    .message  = "File handles were flushed" });
            }

        private:
            ToolLogger *m_logger { nullptr };
        };

    protected:
        auto openFile(const bool &isCoin = false, const uint32_t &id = 0, const std::string &coin = "") -> void
        {
            // Open a Log file everyday
            const std::string path = m_outputPath;
            m_currentLogFileTime   = ToolLogger::getTime();
            if (!isCoin)
            {
                std::string logFile = fmt::format("{}/{:%d-%m-%Y}.log", path, m_currentLogFileTime);
                m_logFile           = fopen(logFile.c_str(), "a+");
                // Ignore failure (for now)
            }
            else
            {
                std::string logFile = fmt::format("{}/{:%d-%m-%Y}.{}.log", path, m_currentLogFileTime, coin);
                m_coinFiles.insert({ id, fopen(logFile.c_str(), "a+") });
                // Ignore failure (for now)
            }
        }
        auto isDateChanged() -> bool
        {
            auto currentTime = getTime();
            // Just check if the day is different
            return m_currentLogFileTime.tm_yday != currentTime.tm_yday;
        }

    private:
        auto acquire(const messages &s) noexcept -> void override
        {
            if (const auto &msgData = getMessage<LoggerEvent>(s); msgData != std::nullopt)
            {
                log(*msgData);
            }
        }

        auto onTerminate() noexcept -> void override
        {
            // Handle termination flushing on the destructor
        }

        auto log(const LoggerEvent &log) -> void
        {
            // Print to console and store to file
            const auto &coinId              = log.coinId;
            const auto &eventId             = log.eventId;

            const auto &eventString         = m_eventNames[eventId].name;
            const auto &coinString          = m_coinNames[coinId].name;

            const uint32_t timeMilliseconds = log.dateTime % 1000;
            const time_t tt                 = static_cast<time_t>(log.dateTime) / 1000;

            // Check the current date
            if (isDateChanged())
            {
                // Close all buffers
                fclose(m_logFile);
                openFile(); // Reopen the main log file
                for (const auto &[id, file] : m_coinFiles)
                {
                    fclose(file);
                    const auto &cnstr = m_coinNames[id].name;
                    openFile(true, id, cnstr); // Reopen the coins file
                }
            }

            //////
            // First store to the log file
            std::string toLogMsg = log.message;
            // Replace all coloring
            re2::RE2::GlobalReplace(&toLogMsg, m_escape0, "");
            re2::RE2::GlobalReplace(&toLogMsg, m_escape1, "");

            m_fileBuffer = fmt::format("[{:%d-%m-%Y %H:%M:%S}.{:03d}]: ", *std::localtime(&tt), timeMilliseconds);
            if (coinId > 0)
            {
                m_fileBuffer += fmt::format("{:<12}: ", coinString);
                if (!log.preventCoinFile)
                    m_fileBuffer += fmt::format("{:<25}: ", eventString);
                else
                    m_fileBuffer += fmt::format("{:<10}: ", eventString);

                m_fileBuffer += fmt::format("{}", toLogMsg);

                const auto &coinLogFile = m_coinFiles[coinId];

                if (!log.preventCoinFile)
                {
                    if (coinLogFile == nullptr)
                    {
                        fmt::print("[{}]: {:<15}: Log File handle for {} is not opened\n",
                            fmt::format(fmt::fg(fmt::color::light_gray), "{:%d-%m-%Y %H:%M:%S}.{:03d}", *std::localtime(&tt), timeMilliseconds),
                            fmt::format(fmt::fg(fmt::color::indian_red), "handle"),
                            coinString);
                        return;
                    }
                    else
                        fmt::print(coinLogFile, "{}\n", m_fileBuffer);
                }
                else
                {
                    if (m_logFile == nullptr)
                    {
                        fmt::print("[{}]: {:<10}: Log File handle is not opened\n",
                            fmt::format(fmt::fg(fmt::color::light_gray), "{:%d-%m-%Y %H:%M:%S}.{:03d}", *std::localtime(&tt), timeMilliseconds),
                            fmt::format(fmt::fg(fmt::color::indian_red), "handle"));
                    }
                    else
                        fmt::print(m_logFile, "{}\n", m_fileBuffer);
                }
            }
            else
            {
                m_fileBuffer += fmt::format("{:<10}: ", eventString);
                m_fileBuffer += fmt::format("{}", toLogMsg);
                if (m_logFile == nullptr)
                {
                    fmt::print("[{}]: {:<10}: Log File handle is not opened\n",
                        fmt::format(fmt::fg(fmt::color::light_gray), "{:%d-%m-%Y %H:%M:%S}.{:03d}", *std::localtime(&tt), timeMilliseconds),
                        fmt::format(fmt::fg(fmt::color::indian_red), "handle"));
                }
                else
                    fmt::print(m_logFile, "{}\n", m_fileBuffer);
            }

            // Second, print to stdout
            const auto &eventColor = m_eventNames[eventId].color;
            const auto &coinColor  = m_coinNames[coinId].color;

            m_consoleBuffer        = fmt::format("[{}]: ", fmt::format(fmt::fg(fmt::color::light_gray), "{:%d-%m-%Y %H:%M:%S}.{:03d}", *std::localtime(&tt), timeMilliseconds));
            if (coinId > 0)
            {
                m_consoleBuffer += fmt::format("{}: ", fmt::format(fmt::fg(coinColor), "{:<12}", coinString));
                if (!log.preventCoinFile)
                    m_consoleBuffer += fmt::format("{}: ", fmt::format(fmt::fg(eventColor), "{:<25}", eventString));
                else
                    m_consoleBuffer += fmt::format("{}: ", fmt::format(fmt::fg(eventColor), "{:<10}", eventString));
            }
            else
                m_consoleBuffer += fmt::format("{}: ", fmt::format(fmt::fg(eventColor), "{:<10}", eventString));

            m_consoleBuffer += fmt::format("{}", log.message);

            fmt::print("{}\n", m_consoleBuffer);
        }

    public:
        auto flush() -> void
        {
            assert(m_logFile not_eq nullptr);
            fflush(m_logFile);
            for (const auto &[id, file] : m_coinFiles)
            {
                assert(file not_eq nullptr);
                fflush(file);
            }
        }
        static auto getTime() -> tm
        {
            time_t tt = time(nullptr);
            return static_cast<const tm &>(*localtime(&tt));
        }

    private:
        re2::RE2 m_escape0 { R"((\x1b\[0m))" };
        re2::RE2 m_escape1 { R"((\x1b\[38;2;(\d{1,2}|(0|1)\d{2}|2[0-4]\d|25[0-5]);(\d{1,2}|(0|1)\d{2}|2[0-4]\d|25[0-5]);(\d{1,2}|(0|1)\d{2}|2[0-4]\d|25[0-5])m))" };
        std::string m_consoleBuffer;
        std::string m_fileBuffer;
        std::string m_outputPath;
        std::map<uint32_t, EventName> m_eventNames;
        std::map<uint32_t, CoinName> m_coinNames;
        std::map<uint32_t, FILE *> m_coinFiles;
        FlushTimeEvent m_flushEvent { this };
        std::thread m_timer;
        FILE *m_logFile { nullptr };
        tm m_currentLogFileTime {};
    };
    namespace def
    {
        /// \brief Definition of a basic
    }
} // namespace cen::tools

#endif // CENTAUR_TOOLSLOGGER_HPP
