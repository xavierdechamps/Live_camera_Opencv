/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *   Show the content for secondary windows. Content can be any QImage which will be inserted to a QGraphicsScene
*/

#include "secondarywindow.h"

/**
 * @brief SecondaryWindow::SecondaryWindow
 * @param parent
 * 
 * Constructor of the class SecondaryWindow. The main content of the window is the QGraphicsScene
 */
SecondaryWindow::SecondaryWindow(QWidget *parent): QDialog(parent)
{
    // Initializations
    this->window_width = 200;
    this->window_height = 200;
    
    this->imageScene = new QGraphicsScene(this);
    this->imageView = new QGraphicsView(imageScene);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(this->imageView, 0, 0);

    setLayout(grid);
    setWindowTitle(tr("Secondary window"));

    this->resize(this->window_width, this->window_height);
}

/**
 * @brief SecondaryWindow::set_image_content
 * @param new_content: QImage the new content of the QGraphicsScene
 * 
 * Receives and updates the new content for the image
 */
void SecondaryWindow::set_image_content(QImage &new_content) {
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

/**
 * @brief SecondaryWindow::set_window_title
 * @param title: QString the new title of the window
 * 
 * Sets a new title for the window
 */
void SecondaryWindow::set_window_title(QString title){
    setWindowTitle(title);
}
