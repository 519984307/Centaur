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
#include "XMLHelper.hpp"
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
            QString installPath;
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

        struct Fonts
        {
            struct SymbolsDock
            {
                QFont headerFont;
                QFont tableFont;
            } symbolsDock;
            struct OrderBookDock
            {
                struct AsksSide
                {
                    QFont tableFont;
                    QFont headerFont;
                } asksSide;
                struct BidsSide
                {
                    QFont tableFont;
                    QFont headerFont;
                } bidsSide;
            } orderBookDock;
        } fonts;

        struct Colors
        {
            struct SymbolsDock
            {
                QColor latencyLow;
                QColor latencyMedium;
                QColor latencyHigh;
                QColor priceUp;
                QColor priceDown;
                QColor priceNeutral;
            } symbolsDockColors;

            struct OrderBookDock
            {
                struct AsksSide
                {
                    QColor latencyLow;
                    QColor latencyMedium;
                    QColor latencyHigh;
                } asksSide;

                struct BidsSide
                {
                    QColor latencyLow;
                    QColor latencyMedium;
                    QColor latencyHigh;
                } bidsSide;
            } orderBookDockColors;
        } colors;

        struct Parameters
        {
            struct SymbolsDock
            {
                uint64_t latencyLowMin;
                uint64_t latencyLowMax;
                uint64_t latencyMediumMin;
                uint64_t latencyMediumMax;
                uint64_t latencyHighMin;
                uint64_t latencyHighMax;
            } symbolsDockParameters;

            struct OrderBookDock
            {
                struct AsksSide
                {
                    uint64_t latencyLowMin;
                    uint64_t latencyLowMax;
                    uint64_t latencyMediumMin;
                    uint64_t latencyMediumMax;
                    uint64_t latencyHighMin;
                    uint64_t latencyHighMax;
                } asksSide;
                struct BidsSide
                {
                    uint64_t latencyLowMin;
                    uint64_t latencyLowMax;
                    uint64_t latencyMediumMin;
                    uint64_t latencyMediumMax;
                    uint64_t latencyHighMin;
                    uint64_t latencyHighMax;
                } bidsSide;
            } orderBookParameters;
        } params;

        struct Locale
        {
            enum class ErrorDetail
            {
                noError = 0,
                localeFileError,
                xmlDocumentInvalid,
                emptyDocument,
                langError
            };
            C_NODISCARD auto loadLocale(const QString &language) noexcept -> ErrorDetail;

            C_NODISCARD auto get(uint64_t crc) const -> const QString &;

        protected:
            std::unordered_map<uint64_t, QString> m_locale;
        } locale;

        struct VisualsUI
        {
            enum class ErrorDetail
            {
                noError = 0,
                visualsFileError,
                xmlDocumentInvalid
            };

            struct VisualsFont
            {
                QString name { "Arial" };
                int size { 13 };
                int weight { 400 };
                bool italic { false };
                qreal spacing { 0.0 };
            };

        } visuals;
    };

    extern Globals *g_globals;
} // namespace CENTAUR_NAMESPACE

#define LS(x) g_globals->locale.get(CRC64_STR(x))

#endif // CENTAUR_GLOBALS_HPP
