/*
 * Copyright: Xavier Dechamps
*/

#ifndef SECONDARYWINDOW_H
#define SECONDARYWINDOW_H

#include <QDialog>
#include <QImage>
//#include <QLabel>
#include <QGraphicsScene> // To show the content of the camera in the Qt window
#include <QGraphicsView>  // To show the content of the camera in the Qt window
#include <QGridLayout>

#include <iostream>
using namespace std;

class SecondaryWindow: public QDialog
{
public:
    SecondaryWindow(QWidget * parent = nullptr);
    void set_image_content(QImage &new_content);
    void set_image_content(QImage &new_content, int width, int height);
    void set_window_title(QString title);
    
private:
    
    QGraphicsScene *imageScene;
    QGraphicsView *imageView;
    
    QImage my_QImage;
    int window_width, window_height;

};

#endif // SECONDARYWINDOW_H
