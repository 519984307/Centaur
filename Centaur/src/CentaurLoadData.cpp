/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 25/05/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "../ui/ui_CentaurApp.h"
#include "CentaurApp.hpp"
#include <QMessageBox>
#include <xercesc/util/PlatformUtils.hpp>

namespace CENTAUR_NAMESPACE
{
    namespace
    {
        void errorDelay(const int ms)
        {
            QTime dieTime = QTime::currentTime().addMSecs(ms);
            while (QTime::currentTime() < dieTime)
                QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
        }

        C_NORETURN void delayAndExit(const int ms)
        {
            errorDelay(ms);
            std::exit(EXIT_FAILURE);
        }
    } // namespace
} // namespace CENTAUR_NAMESPACE

void CENTAUR_NAMESPACE::CentaurApp::loadConfigurationData() noexcept
{
    try
    {
        xercesc::XMLPlatformUtils::Initialize();
    } catch (const xercesc::XMLException &ex)
    {
        char *message = xercesc::XMLString::transcode(ex.getMessage());
        qDebug() << "Error during initialization!" << message << "\n";
        xercesc::XMLString::release(&message);
    }

    // Load all strings
    loadLocaleData();

    try
    {
        xercesc::XMLPlatformUtils::Terminate();
    } catch (const xercesc::XMLException &ex)
    {
        char *message = xercesc::XMLString::transcode(ex.getMessage());
        qDebug() << "Error during termination!" << message << "\n";
        xercesc::XMLString::release(&message);
    }
}

void CENTAUR_NAMESPACE::CentaurApp::loadLocaleData() noexcept
{
    auto res = g_globals->locale.loadLocale("en-US");

    switch (res)
    {

        case Globals::Locale::ErrorDetail::noError:
            logTrace("app", "Locale data loaded");
            break;
        case Globals::Locale::ErrorDetail::localeFileError:
            logFatal("app", "Could not load the language data file");
            QMessageBox::critical(this, "Error", "Could not load the language data file");
            delayAndExit(100);
        case Globals::Locale::ErrorDetail::xmlDocumentInvalid:
            logFatal("app", "Could not load the language data file. The file is ill-formed");
            QMessageBox::critical(this, "Error", "Could not load the language data file. The file it is ill-formed");
            delayAndExit(100);
        case Globals::Locale::ErrorDetail::emptyDocument:
            logFatal("app", "Locale file is empty");
            QMessageBox::critical(this, "Error", "Locale file is empty");
            delayAndExit(100);

        case Globals::Locale::ErrorDetail::langError:
            logFatal("app", "Locale file inner lang is missing or does not match the spected");
            QMessageBox::critical(this, "Error", "Locale file inner lang is missing or does not match the spected");
            delayAndExit(100);
    }
}
