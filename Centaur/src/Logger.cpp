/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//
#include "Logger.hpp"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTranslator>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif /*__clang__*/

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif /*__clang__*/

cent::CentaurLogger *cent::g_logger { nullptr };

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

static constexpr char insertDb[] = { R"(INSERT INTO log (date, session, user, level, source, message) VALUES ('{}', {}, '{}', {}, '{}', '{}'); )" };

static auto sqlExec(void *, int, char **, char **) -> int
{
    // The database function does not really care about selecting data
    // We only do inserts
    return 0;
}

cent::CentaurLogger::CentaurLogger()
{
    sqlite3_initialize();
}

cent::CentaurLogger::~CentaurLogger()
{
    if (m_sql != nullptr)
        sqlite3_close(m_sql);

    sqlite3_shutdown();
}

void cent::CentaurLogger::run() noexcept
{
    while (!m_terminateSignal)
    {
        // Protect Race Conditions
        std::unique_lock<std::mutex> lock { m_dataProtect };
        // Wait for the termination flag or input data
        m_waitCondition.wait(lock, [&]() { return m_terminateSignal || !m_messages.empty(); });

        dispatch();
    }
}
void cent::CentaurLogger::terminate() noexcept
{
    m_terminateSignal = true;
    m_waitCondition.notify_one();
}

void cent::CentaurLogger::process(const LogMessage &log) noexcept
{
    QMetaObject::invokeMethod(m_app, "onLog",
        Qt::QueuedConnection,
        Q_ARG(unsigned long long, log.date),
        Q_ARG(int, log.session),
        Q_ARG(int, static_cast<int>(log.level)),
        Q_ARG(QString, log.user),
        Q_ARG(QString, log.source),
        Q_ARG(QString, log.msg));

    const auto thisTime = static_cast<std::time_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    // Insert into database
    std::string query = fmt::format(insertDb,
        fmt::format("{:%d-%m-%Y %H:%M:%S}", *std::localtime(&thisTime)),
        log.session,
        log.user.toStdString(),
        log.level,
        log.source.toStdString(),
        log.msg.toStdString());

    char *errStr;
    int err = sqlite3_exec(m_sql, query.c_str(), sqlExec, nullptr, &errStr);

    if (err != SQLITE_OK)
    {
        QMetaObject::invokeMethod(m_app, "onLog",
            Qt::QueuedConnection,
            Q_ARG(unsigned long long, log.date),
            Q_ARG(int, log.session),
            Q_ARG(int, static_cast<int>(log.level)),
            Q_ARG(QString, QString { "logger" }),
            Q_ARG(QString, QString { "insert" }),
            Q_ARG(QString, QString { errStr }));
        sqlite3_free(errStr);
    }
}

void cent::CentaurLogger::dispatch() noexcept
{
    // Process all queries
    while (!m_messages.empty())
    {
        // Get Data
        const LogMessage msg = m_messages.front();
        // Process Data
        process(msg);
        // Remove it from the queries queue
        m_messages.pop();
    }
}

void cent::CentaurLogger::setApplication(cent::CentaurApp *app)
{
    auto logFile   = g_globals->installPath + "/Log/log0.db";
    m_app          = app;

    bool recoverDb = false;

    if (!QFileInfo::exists(logFile))
    {
        int iResult = QMessageBox::warning(app,
            QApplication::tr("Error"),
            QApplication::tr("Could not locate logging file\nWould you like to try to recover?"),
            QMessageBox::Yes | QMessageBox::No);

        if (iResult == QMessageBox::Yes)
            recoverDb = true;
        else
            throw std::runtime_error("file not located");
    }

    if (int err = sqlite3_open(logFile.toStdString().c_str(), &m_sql); err != SQLITE_OK)
        throw(std::runtime_error(sqlite3_errstr(err)));
    else
    {
        if (recoverDb)
        {
            auto recoveryFile = g_globals->resPath + "/Repair/logdb.sql";
            QFile file(recoveryFile);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                throw std::runtime_error("recovery file not located");

            QTextStream text(&file);
            QString query = text.readAll();

            char *errStr;
            int err = sqlite3_exec(m_sql, query.toStdString().c_str(), sqlExec, nullptr, &errStr);

            if (err != SQLITE_OK)
            {
                std::string str { errStr };
                sqlite3_free(errStr);
                throw std::runtime_error(str);
            }
        }
    }
}

void cent::CentaurLogger::log(const QString &source, const cent::interface::LogLevel level, const QString &msg) noexcept
{
    std::lock_guard<std::mutex> lock { m_dataProtect };
    m_messages.push({ static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
        m_session,
        level,
        m_user,
        source,
        msg });
    m_waitCondition.notify_one();
}
