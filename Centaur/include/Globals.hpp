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

#include "../../Centaur.hpp"
#include "XMLHelper.hpp"
#include "crc64.hpp"
#include <QFont>
#include <QIcon>
#include <QString>
#include <unordered_map>

namespace CENTAUR_NAMESPACE
{
    struct Globals
    {
        struct Paths
        {
            QString appPath;
            QString installPath;
            QString pluginsPath;
            QString resPath;
        } paths;

        struct Icons
        {
            QIcon upArrow { ":/img/res/img/upArrow.svg" };
            QIcon downArrow { ":/img/res/img/downArrow.svg" };
            QIcon searchIcon { ":/svg/edit/search_gray" };
        } icons;

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

            C_NODISCARD auto loadVisualsUI() noexcept -> ErrorDetail;

            static auto allocateVisuals() noexcept -> void;
            static auto releaseVisuals() noexcept -> void;

            struct VisualsFont
            {
                QString name { "Arial" };
                int size { 13 };
                int weight { 400 };
                bool italic { false };
                qreal spacing { 0.0 };
            };

            struct DockSymbols
            {
                VisualsFont headerFont;
                VisualsFont tableFont;
                VisualsFont searchFont;
                QString headerCSS;
                QString tableCSS;
                QString searchCSS;
            } * dockSymbols { nullptr };

        private:
            auto parseVisuals(xercesc::DOMDocument *doc) -> void;

        } visuals;
    };

    extern Globals *g_globals;
} // namespace CENTAUR_NAMESPACE

#define LS(x) g_globals->locale.get(CRC64_STR(x))

#endif // CENTAUR_GLOBALS_HPP
