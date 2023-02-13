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
#include "CentaurInterface.hpp"
#include "crc64.hpp"
#include <QFont>
#include <QIcon>
#include <QPixmapCache>
#include <QSqlQuery>
#include <QString>
#include <rapidjson/document.h>
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
    };
    /// \brief Finds the image of the specified asset, size and format (when supported).
    /// \param size Size
    /// \param asset Asset name
    /// \param source Database Origin
    /// \return A null pixmap if it fails, or a loaded pixmap, otherwise.
    /// \remarks The application only supports transparent file formats. In the database files,
    /// the application will search for the files in the order: SVG, PNG, TIFF and GIF for the symbol
    /// in case the format is not SVG, the application will append the size in the file
    /// for example: supposed the asset named GGG, and the image file exists as a PNG with sizes, 16, 32, 64
    /// The files must be named: GGG_16.png, GGG_32.png, GGG_64.png;
    extern QPixmap findAssetImage(int size, const QString &asset, CENTAUR_INTERFACE_NAMESPACE::AssetImageSource source, QWidget *caller = nullptr);

    extern Globals *g_globals;
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_GLOBALS_HPP
