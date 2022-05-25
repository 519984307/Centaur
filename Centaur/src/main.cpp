/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "CentaurApp.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("CentaurProject");
    QCoreApplication::setOrganizationDomain("centaur.com");
    QCoreApplication::setApplicationName("Centaur");

    auto app = new CENTAUR_NAMESPACE::CentaurApp;
    app->show();

    return QApplication::exec();
}
