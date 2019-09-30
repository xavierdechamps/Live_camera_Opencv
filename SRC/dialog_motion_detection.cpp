/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with motion detection
 *   Implemented methods are:
 *      1) Farneback dense optical flow
 *      2) Foreground extraction
 *
*/

#include "dialog_motion_detection.h"

Dialog_Motion_Detection::Dialog_Motion_Detection(QWidget *parent): QDialog(parent)
{
    // Build the buttons
    QRadioButton* radioButton1 = new QRadioButton("Farneback dense optical flow"); radioButton1->setChecked(true);
    QRadioButton* radioButton2 = new QRadioButton("Foreground extraction");

    // Radio buttons (only one button is active at each moment)
    this->RadioButtons = new QButtonGroup();
    this->RadioButtons->addButton(radioButton1,1);
    this->RadioButtons->addButton(radioButton2,2);

    // Create connections between button click and function
    connect(radioButton1, SIGNAL(clicked()), this, SLOT(onClick_Radio_Motion_Method()) ) ;
    connect(radioButton2, SIGNAL(clicked()), this, SLOT(onClick_Radio_Motion_Method()) ) ;

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(radioButton1,                 0, 0);
    grid->addWidget(radioButton2,                 1, 0);

    setLayout(grid);
    setWindowTitle(tr("Motion detection control window"));
    //resize(400, 400);
}

void Dialog_Motion_Detection::onClick_Radio_Motion_Method() {
    emit this->Signal_motion_detection_method_changed(this->RadioButtons->checkedId());
}
