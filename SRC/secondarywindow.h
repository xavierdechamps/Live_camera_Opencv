/*
 * Copyright: Xavier Dechamps
*/

#ifndef SECONDARYWINDOW_H
#define SECONDARYWINDOW_H

#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QGridLayout>

#include <iostream>
using namespace std;

class SecondaryWindow: public QDialog
{
public:
    SecondaryWindow(QWidget * parent = 0);
    void set_image_content(QImage &new_content, int width, int height);

private:
    QLabel *Window_image;
    QImage my_QImage;
    int window_width, window_height;

};

#endif // SECONDARYWINDOW_H
