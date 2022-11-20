/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_GLOBALS_HPP
#define CENTAUR_GLOBALS_HPP

#include "Centaur.hpp"
#include "crc64.hpp"
#include <QFont>
#include <QIcon>
#include <QPixmapCache>
#include <QSqlQuery>
#include <QString>
#include <unordered_map>

namespace CENTAUR_NAMESPACE::Ui
{
    class CentaurApp;
} // namespace CENTAUR_NAMESPACE::Ui

namespace CENTAUR_NAMESPACE
{
    struct AESSym
    {
        static auto decrypt(const QString &text, const QByteArray &key) -> QString;
        static auto encrypt(const QByteArray &data, const QByteArray &key) -> QString;
        static auto createUniqueId(int rounds, std::size_t maxSize) -> std::string;
    };

    struct Globals
    {
        struct SessionData
        {
            QString user;
            QString display;
            QString email;
            QString userTFA;
            QImage image;
            bool tfa;
        } session;

        struct Paths
        {
            QString appPath;
            QString pluginsPath;
            QString resPath;
        } paths;

        struct Icons
        {
            QIcon upArrow { ":/img/arrow-up" };
            QIcon downArrow { ":/img/arrow-down" };
            QIcon searchIcon { ":/svg/edit/search_gray" };
            QIcon favoritesIcon { ":/svg/favorites/star" };
        } icons;

        struct SymbolsIcons
        {
            SymbolsIcons();
            ~SymbolsIcons() = default;
            /// \brief Finds the image of the specified symbol, size and format (when supported).
            ///        This class will automatically let QPixmapCache handle the memory. That is will delete items when full and insert them as needed
            /// \param size Size (when format is SvG it will be ignored)
            /// \param symbol Symbol name
            /// \param px The image data structure
            /// \param format 0: for PNG, 1: for SVG
            /// \return true on success false on failure
            bool find(int size, const QString &symbol, QPixmap *px, int format = 0);

        } symIcons;


    };

    extern Globals *g_globals;
} // namespace CENTAUR_NAMESPACE


#endif // CENTAUR_GLOBALS_HPP
