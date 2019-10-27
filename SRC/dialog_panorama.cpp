/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with image stitching to create a panorama
 *   Possibility to add a new image to the panorama
 *                  remove the last image from the panorama
 *                  update the panorama
 *                  reset the panorama
 *                  save the result to a local file
*/

#include "dialog_panorama.h"

Dialog_Panorama::Dialog_Panorama(QWidget *parent):    QDialog(parent)
{
    // QLabel to show the number of images stored in the panorama
    this->Panorama_value = new QLabel("Number of images in the panorama: 11");
    this->Panorama_value->setFixedWidth(this->Panorama_value->sizeHint().width());
    this->Panorama_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Panorama_value->setText("Number of images in the panorama: 0");

    // QLabel to show the status of the stitching operation
    this->Panorama_status = new QLabel("");

    // Build the buttons
    QPushButton *button1 = new QPushButton("Pick up image for the panorama", this);
    QPushButton *button2 = new QPushButton("Remove the last image", this);
    QPushButton *button3 = new QPushButton("Update the panorama", this);
    QPushButton *button4 = new QPushButton("Reset the panorama", this);
    QPushButton *button5 = new QPushButton("Save the panorama", this);

    // Create connections between button click and function
    connect(button1, SIGNAL(clicked()), this, SLOT(onClick_Pick_Up_Image()) ) ;
    connect(button2, SIGNAL(clicked()), this, SLOT(onClick_Pop_Up_Image()) ) ;
    connect(button3, SIGNAL(clicked()), this, SLOT(onClick_Update_Panorama()) ) ;
    connect(button4, SIGNAL(clicked()), this, SLOT(onClick_Reset_Panorama()) ) ;
    connect(button5, SIGNAL(clicked()), this, SLOT(onClick_Save_Panorama()) ) ;

    // Tool tips when hovering the buttons and sliders
    button1->setToolTip("Pick up a new frame to insert in the panorama");
    button2->setToolTip("Delete the last frame inserted in the panorama");
    button3->setToolTip("Update the panorama");
    button4->setToolTip("Reset the panorama");
    button5->setToolTip("Save the panorama as an image on the local disk");

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(this->Panorama_value, 0, 0);
    grid->addWidget(button1, 1, 0);
    grid->addWidget(button2, 2, 0);
    grid->addWidget(button3, 3, 0);
    grid->addWidget(button4, 4, 0);
    grid->addWidget(button5, 5, 0);
    grid->addWidget(this->Panorama_status, 6, 0);

    setLayout(grid);
    setWindowTitle(tr("Panorama control window"));
}

void Dialog_Panorama::set_QLabel_number_images(int value) {
    // Set the number of images stored in the panorama
    this->Panorama_value->setText("Number of images in the panorama: "+QString::number(value));
}

void Dialog_Panorama::set_QLabel_string(std::string label) {
    // Set the status of the stitching operation
    this->Panorama_status->setText(QString::fromStdString(label));
}

void Dialog_Panorama::onClick_Pick_Up_Image() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world so that a picture is picked up
    emit this->Signal_pick_up_image_panorama() ;
}

void Dialog_Panorama::onClick_Pop_Up_Image() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world so that the last picture is removed
    emit this->Signal_pop_up_image_panorama() ;
}

void Dialog_Panorama::onClick_Update_Panorama() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world so that the panorama is updated
    emit this->Signal_update_panorama() ;
}

void Dialog_Panorama::onClick_Reset_Panorama() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world so that the panorama is resetted
    emit this->Signal_reset_panorama() ;
}

void Dialog_Panorama::onClick_Save_Panorama() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world so that the panorama is saved to a file
    emit this->Signal_save_panorama() ;
}
