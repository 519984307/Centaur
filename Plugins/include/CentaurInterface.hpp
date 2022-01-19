/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_CENTAURINTERFACE_HPP
#define CENTAUR_CENTAURINTERFACE_HPP

#ifndef DONT_INCLUDE_QT
#include <QString>

#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif /*__clang__*/

namespace cent::interface
{
    enum class LogLevel
    {
        fatal, /// Beware that posting a Fatal log will terminate the application
        error,
        info,
        warning,
        trace,
        debug
    };

    /// \brief Handles the logging window
    /// All plugin interfaces accept the ILogger interface to access the Logging window of the main UI
    struct ILogger
    {
        virtual ~ILogger() = default;

        /// \brief Send a log message to the application
        ///
        /// \param source The message source
        /// \param level Level of the diagnosis
        /// \param msg Message
        virtual void log(const QString &source, const cent::interface::LogLevel level, const QString &msg) noexcept = 0;

        /// \brief Wrapper around msg with fatal level
        ///
        /// \param source The message source
        /// \param msg Message
        virtual void fatal(const QString &source, const QString &msg) noexcept = 0;

        /// \brief Wrapper around msg with error level
        ///
        /// \param source The message source
        /// \param msg Message
        virtual void error(const QString &source, const QString &msg) noexcept = 0;

        /// \brief Wrapper around msg with warning level
        ///
        /// \param source The message source
        /// \param msg Message
        virtual void warning(const QString &source, const QString &msg) noexcept = 0;

        /// \brief Wrapper around msg with info level
        ///
        /// \param source The message source
        /// \param msg Message
        virtual void info(const QString &source, const QString &msg) noexcept = 0;

        /// \brief Wrapper around msg with trace level
        ///
        /// \param source The message source
        /// \param msg Message
        virtual void trace(const QString &source, const QString &msg) noexcept = 0;

        /// \brief Wrapper around msg with debug level
        ///
        /// \param source The message source
        /// \param msg Message
        virtual void debug(const QString &source, const QString &msg) noexcept = 0;
    };

    /// \brief Provide the methods to access the main configuration file in the plugin data
    struct IConfiguration
    {
    };

} // namespace cent::interface
#endif /*DONT_INCLUDE_QT*/

#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic pop
#endif /*__clang__*/

#endif // CENTAUR_CENTAURINTERFACE_HPP
