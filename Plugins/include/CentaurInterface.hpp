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

#include "../../Centaur.hpp"

#ifndef DONT_INCLUDE_QT
#include <QString>
#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif /*__clang__*/

#ifndef CENTAUR_INTERFACE_NAMESPACE
#define CENTAUR_INTERFACE_NAMESPACE CENTAUR_NAMESPACE::interface
#endif /*CENTAUR_INTERFACE_NAMESPACE*/

namespace CENTAUR_INTERFACE_NAMESPACE
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
        virtual void log(const QString &source, CENTAUR_INTERFACE_NAMESPACE::LogLevel level, const QString &msg) noexcept = 0;

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

#ifndef NDEBUG
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
#else
        /// \brief Wrapper around msg with trace level
        virtual void trace() noexcept = 0;

        /// \brief Wrapper around msg with debug level
        virtual void debug() noexcept = 0;
#endif /*NDEBUG*/
    };

    /// \brief Provide the methods to access the main configuration file in the plugin data
    ///        In case you want to store an encrypted password you must use the libProtocol interfaces.
    ///        The data must be encrypted with the plugin private key (stored on the Resources/Private folder
    ///        And the plugin must use the public key to decrypt the data
    struct IConfiguration
    {
        virtual ~IConfiguration() = default;

    public:
        /// \brief Returns the file name containing the plugin key
        /// \return The file containing the data
        virtual auto getPluginPublicKeyPath() -> std::string = 0;

    public:
        /// \brief Get a value from the configuration file as string
        /// \param key Key name value
        /// \param error If the key does not exists error will be set to true
        /// \return The value.
        virtual auto getValue(const std::string &key, bool *error) -> std::string = 0;

        /// \brief Get a value from the configuration file as uint32_t
        /// \param key Key name value
        /// \param error If the key does not exists error will be set to true
        virtual auto getUint32(const std::string &key, bool *error) -> uint32_t = 0;

        /// \brief Get a value from the configuration file as string
        /// \param key Key name value
        /// \param error If the key does not exists error will be set to true
        virtual auto getUint64(const std::string &key, bool *error) -> uint64_t = 0;

        /// \brief Get a value from the configuration file as string
        /// \param key Key name value
        /// \param error If the key does not exists error will be set to true
        virtual auto getInt32(const std::string &key, bool *error) -> int32_t = 0;

        /// \brief Get a value from the configuration file as string
        /// \param key Key name value
        /// \param error If the key does not exists error will be set to true
        virtual auto getInt64(const std::string &key, bool *error) -> int64_t = 0;

        /// \brief Get a value from the configuration file as string
        /// \param key Key name value
        /// \param error If the key does not exists error will be set to true
        virtual auto getBool(const std::string &key, bool *error) -> bool = 0;
    };

} // namespace CENTAUR_INTERFACE_NAMESPACE
#endif /*DONT_INCLUDE_QT*/

#if defined(__clang__) || defined(__GNUC__)
#pragma clang diagnostic pop
#endif /*__clang__*/

#endif // CENTAUR_CENTAURINTERFACE_HPP
