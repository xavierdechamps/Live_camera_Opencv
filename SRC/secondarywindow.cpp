/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Show the content for secondary windows. Content can be any QImage which will be converted to a QLabel
*/

#include "secondarywindow.h"

SecondaryWindow::SecondaryWindow(QWidget *parent): QDialog(parent)
{
    // Initializations
    this->window_width = 200;
    this->window_height = 200;
    
    this->imageScene = new QGraphicsScene(this);
    this->imageView = new QGraphicsView(imageScene);
//    this->setCentralWidget(this->imageView);
    
//    this->Window_image = new QLabel("",this);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(this->imageView, 0, 0);

    setLayout(grid);
    setWindowTitle(tr("Secondary window"));

    this->resize(this->window_width, this->window_height);
}

void SecondaryWindow::set_image_content(QImage &new_content) {
    // Receive and update the new content for the image with dimensions
    this->my_QImage = new_content;
    this->window_width = new_content.width() ;
    this->window_height = new_content.height();
    
    QPixmap piximage = QPixmap::fromImage(this->my_QImage);
    imageScene->clear();
    imageView->resetMatrix();
    imageScene->addPixmap(piximage);
    imageScene->update();
    imageView->setSceneRect(piximage.rect());
    
    this->resize(1.15*this->window_width, 1.15*this->window_height);
    repaint();
}

void SecondaryWindow::set_image_content(QImage &new_content, int width, int height) {
    // Receive and update the new content for the image with dimensions
    this->my_QImage = new_content;
    this->window_width = width;
    this->window_height = height;
    
    QPixmap piximage = QPixmap::fromImage(this->my_QImage);
    imageScene->clear();
    imageView->resetMatrix();
    imageScene->addPixmap(piximage);
    imageScene->update();
    imageView->setSceneRect(piximage.rect());
    
    this->resize(1.15*this->window_width, 1.15*this->window_height);
    repaint();
}

void SecondaryWindow::set_window_title(QString title){
    setWindowTitle(title);
}
