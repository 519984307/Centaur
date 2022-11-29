/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 24/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "TestDialog.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestDialog dlg;

    dlg.show();

    return app.exec();
}
