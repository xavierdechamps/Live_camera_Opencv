/*
 * Copyright: Xavier Dechamps
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
    MainWindow w ;

    w.show();

    return app.exec();
}

