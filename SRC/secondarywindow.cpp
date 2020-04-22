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
    this->Window_image = new QLabel("",this);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(this->Window_image, 0, 0);

    setLayout(grid);
    setWindowTitle(tr("Secondary window"));

    this->resize(this->window_width, this->window_height);
}

void SecondaryWindow::set_image_content(QImage &new_content, int width, int height) {
    // Receive and update the new content for the image with dimensions
    this->my_QImage = new_content;
    this->window_width = width;
    this->window_height = height;
    this->Window_image->setPixmap(QPixmap::fromImage(this->my_QImage));
    this->setFixedSize(this->window_width, this->window_height);
    repaint();
}
