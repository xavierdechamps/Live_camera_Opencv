/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *   Launch the Qt Window
 *   nothing special done here
*/

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow *window =new MainWindow() ;

    window->show();

    return app.exec();
}

