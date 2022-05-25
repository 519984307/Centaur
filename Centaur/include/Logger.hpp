/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_LOGGER_HPP
#define CENTAUR_LOGGER_HPP

#include <QString>
#include <QThread>
#include <mutex>
#include <queue>
#include <sqlite3.h>
#include <string>

// local
#include "CentaurApp.hpp"
#include "CentaurPlugin.hpp"

struct LogMessage
{
    std::size_t date;
    int session;
    CENTAUR_INTERFACE_NAMESPACE::LogLevel level;
    QString user;
    QString source;
    QString msg;
};

namespace CENTAUR_NAMESPACE
{
    class CentaurApp;
    class CentaurLogger : public CENTAUR_NAMESPACE::interface::ILogger
    {
    public:
        CentaurLogger();
        ~CentaurLogger() override;

    public:
        void run() noexcept;
        void terminate() noexcept;

    protected:
        void process(const LogMessage &log) noexcept;
        void dispatch() noexcept;

    public:
        void setApplication(CentaurApp *app);
        inline void setUser(const QString &user) { m_user = user; }
        inline void setSession(const int &session) { m_session = session; }

    public:
        /// \brief Send a log message to the application
        ///
        /// \param source The message source
        /// \param level Level of the diagnosis
        /// \param msg Message
        void log(const QString &source, CENTAUR_NAMESPACE::interface::LogLevel level, const QString &msg) noexcept override;

        /// \brief Wrapper around log with fatal level
        ///
        /// \param source The message source
        /// \param msg Message
        inline void fatal(const QString &source, const QString &msg) noexcept override { log(source, CENTAUR_NAMESPACE::interface::LogLevel::fatal, msg); }

        /// \brief Wrapper around log with error level
        ///
        /// \param source The message source
        /// \param msg Message
        inline void error(const QString &source, const QString &msg) noexcept override { log(source, CENTAUR_NAMESPACE::interface::LogLevel::error, msg); }

        /// \brief Wrapper around msg with warning level
        ///
        /// \param source The message source
        /// \param msg Message
        inline void warning(const QString &source, const QString &msg) noexcept override { log(source, CENTAUR_NAMESPACE::interface::LogLevel::warning, msg); }

        /// \brief Wrapper around log with info level
        ///
        /// \param source The message source
        /// \param msg Message
        inline void info(const QString &source, const QString &msg) noexcept override { log(source, CENTAUR_NAMESPACE::interface::LogLevel::info, msg); }

#ifndef NDEBUG
        /// \brief Wrapper around log with trace level
        ///
        /// \param source The message source
        /// \param msg Message
        inline void trace(const QString &source, const QString &msg) noexcept override { log(source, CENTAUR_NAMESPACE::interface::LogLevel::trace, msg); }

        /// \brief Wrapper around log with debug level
        ///
        /// \param source The message source
        /// \param msg Message
        inline void debug(const QString &source, const QString &msg) noexcept override { log(source, CENTAUR_NAMESPACE::interface::LogLevel::debug, msg); }
#else
        inline void trace() noexcept override
        {
        }

        inline void debug() noexcept override
        {
        }
#endif

    private:
        CENTAUR_NAMESPACE::CentaurApp *m_app { nullptr };
        sqlite3 *m_sql { nullptr };
        QString m_user;
        int m_session { 0 };

        // Messagging thread
    private:
        std::queue<LogMessage> m_messages;
        std::condition_variable m_waitCondition;
        std::mutex m_dataProtect;
        std::atomic_bool m_terminateSignal { false };
    };
    extern CentaurLogger *g_logger;
} // namespace CENTAUR_NAMESPACE

// Helper macros
#define logInfo(x, y) \
    g_logger->info(x, y)

#define logWarn(x, y) \
    g_logger->warning(x, y)

#if !defined(NDEBUG)
#define logTrace(x, y) \
    g_logger->trace(x, y)
#define logDebug(x, y) \
    g_logger->debug(x, y)
#else
#define logTrace(x, y) \
    ((void *)0)
#define logDebug(x, y) \
    ((void *)0)
#endif /**/

#define logError(x, y) \
    g_logger->error(x, y)

#define logFatal(x, y) \
    g_logger->fatal(x, y)
#endif // CENTAUR_LOGGER_HPP
