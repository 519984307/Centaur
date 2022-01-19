/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// This file contains the code to show the logs on screen

#include "CentaurApp.hpp"

void cent::CentaurApp::onLog(const unsigned long long &date, const int &session, const int &level, const QString &usr, const QString &source, const QString &msg) noexcept
{
    QTableWidget *logger = m_ui->m_logs;
    if (logger->rowCount() > 2000)
        logger->clearContents();

    auto logLevel = static_cast<cent::interface::LogLevel>(level);

    int curRow    = logger->rowCount();
    logger->insertRow(curRow);

    auto insertItem = [&](const int &col, const QString &text, const QColor &color, const int &align = Qt::AlignCenter) {
        auto item = new QTableWidgetItem(text);
        item->setFont(QFont("Arial", 10));
        item->setForeground(color);
        item->setTextAlignment(align | Qt::AlignVCenter);
        logger->setItem(curRow, col, item);
        return item;
    };

    auto insertedItem = insertItem(0,
        QDateTime::fromSecsSinceEpoch(static_cast<qint64>(date)).toString("dd-MM-yyyy hh:mm:ss"),
        QColor(0xA2A2A2), Qt::AlignLeft);

    insertItem(1, usr, Qt::white);
    insertItem(2, QString("%1").arg(session), Qt::white);

    switch (logLevel)
    {
        case cent::interface::LogLevel::fatal:
            insertItem(3, "fatal", Qt::red);
            break;
        case cent::interface::LogLevel::error:
            insertItem(3, "error", QColor(0xFF8C00ul)); // Dark orange
            break;
        case cent::interface::LogLevel::warning:
            insertItem(3, "warning", Qt::yellow); // Dark orange
            break;
        case cent::interface::LogLevel::info:
            insertItem(3, "info", QColor(0x00BFFFul)); // Deep sky blue
            break;
        case cent::interface::LogLevel::debug:
            insertItem(3, "debug", QColor(0x6A5ACDul)); // Slate blue
            break;
        case cent::interface::LogLevel::trace:
            insertItem(3, "trace", QColor(0xFF69B4ul)); // Hot pink
            break;
    }

    insertItem(4, source, Qt::white);

    QString newMsg { msg };

    newMsg.prepend("<span style=\"font-size: 11px;font-family: arial;vertical-align: middle\">");
    auto i = newMsg.indexOf("#", 0);
    if (i != -1)
    { // Honestly this is if is put here because  I wanted to use the do/while syntax. Because it's not necessary
        do
        {
            newMsg.replace(i, 1, "<span style=\"color:");
            i = newMsg.indexOf("#", i + static_cast<int>(QString("<span style=\"color:").size()) + 1);
            newMsg.replace(i, 1, "\">");
            i = newMsg.indexOf("#", i + static_cast<int>(QString("\">").size()));
            newMsg.replace(i, 1, "</span>");
        } while ((i = newMsg.indexOf("#", i)) != -1);
    }
    newMsg.append("</span>");

    insertItem(5, newMsg, Qt::white, Qt::AlignLeft);

    logger->scrollToItem(insertedItem);
}
