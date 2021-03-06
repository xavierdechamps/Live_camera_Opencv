/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with motion detection
 *   Implemented methods are:
 *      1) Farneback dense optical flow (desactived because does not work)
 *      2) Foreground extraction
 *
*/

#include "dialog_motion_detection.h"

/**
 * @brief Dialog_Motion_Detection::Dialog_Motion_Detection
 * @param parent
 * 
 * Constructor of the class Dialog_Motion_Detection. Set the appearance of the widget and create connections between
 * the buttons and the functionnalities
 */
Dialog_Motion_Detection::Dialog_Motion_Detection(QWidget *parent): QDialog(parent)
{
    // Build the buttons
//    QRadioButton* radioButton1 = new QRadioButton("Farneback dense optical flow"); radioButton1->setChecked(true);
    QRadioButton* radioButton2 = new QRadioButton("Background extraction"); radioButton2->setChecked(true);

    // Radio buttons (only one button is active at each moment)
    this->RadioButtons = new QButtonGroup();
//    this->RadioButtons->addButton(radioButton1,1);
    this->RadioButtons->addButton(radioButton2,2);

    // Create connections between button click and function
//    connect(radioButton1, SIGNAL(clicked()), this, SLOT(onClick_Radio_Motion_Method()) ) ;
    connect(radioButton2, SIGNAL(clicked()), this, SLOT(onClick_Radio_Motion_Method()) ) ;

    // Tool tips when hovering the buttons and sliders
//    radioButton1->setToolTip("Farneback dense optical flow computes the optical flow for all the points in the frame");
    radioButton2->setToolTip("Background extraction: create a foreground mask that contains the moving objects. It performs a subtraction between the current frame and a background model");

    QGridLayout *grid = new QGridLayout;
//    grid->addWidget(radioButton1,                 0, 0);
    grid->addWidget(radioButton2,                 1, 0);

    setLayout(grid);
    setWindowTitle(tr("Motion detection control window"));
    //resize(400, 400);
}

/**
 * @brief Dialog_Motion_Detection::onClick_Radio_Motion_Method
 * 
 * Function called when the buttons are clicked
 * Emits a signal to the external world with the method selected for motion detection
 */
void Dialog_Motion_Detection::onClick_Radio_Motion_Method() {
    emit this->Signal_motion_detection_method_changed(this->RadioButtons->checkedId());
}
