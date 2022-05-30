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

namespace CENTAUR_NAMESPACE::Ui
{
    class CentaurApp;
} // namespace CENTAUR_NAMESPACE::Ui

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

            C_NODISCARD auto loadVisualsUI(Ui::CentaurApp *ui) noexcept -> ErrorDetail;

            struct VisualsFont
            {
                QString name { "Arial" };
                int size { 13 };
                int weight { 400 };
                bool italic { false };
                qreal spacing { 0.0 };
            };

        private:
            auto parseVisuals(xercesc::DOMDocument *doc, Ui::CentaurApp *ui) -> void;

        } visuals;
    };

    extern Globals *g_globals;
} // namespace CENTAUR_NAMESPACE

#define LS(x) g_globals->locale.get(CRC64_STR(x))

#endif // CENTAUR_GLOBALS_HPP
