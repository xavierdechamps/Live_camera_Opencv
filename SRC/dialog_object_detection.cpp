/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with object detections
 *   Implemented methods are:
 *      1) Hough line transform
 *      2) Hough circle transform
 *      3) Harris corner detection
 *  One slider allows the adjustement of the Hough line transform threshold
*/

#include "dialog_object_detection.h"

Dialog_Object_Detection::Dialog_Object_Detection(QWidget *parent): QDialog(parent)
{
    // Build the buttons
    QRadioButton* radioButton1 = new QRadioButton("Hough line transform"); radioButton1->setChecked(true);
    QRadioButton* radioButton2 = new QRadioButton("Hough circle transform");
    QRadioButton* radioButton3 = new QRadioButton("Harris corner detection");

    // Radio buttons (only one button is active at each moment)
    RadioButtons = new QButtonGroup();
    RadioButtons->addButton(radioButton1,1);
    RadioButtons->addButton(radioButton2,2);
    RadioButtons->addButton(radioButton3,3);

    // Create connections between button click and function
    connect(radioButton1, SIGNAL(clicked()), this, SLOT(onClick_Object_Detection_Method()) ) ;
    connect(radioButton2, SIGNAL(clicked()), this, SLOT(onClick_Object_Detection_Method()) ) ;
    connect(radioButton3, SIGNAL(clicked()), this, SLOT(onClick_Object_Detection_Method()) ) ;

    // QLabel to show the Hough line transform threshold
    this->Slider_hough_line_threshold_value = new QLabel("Threshold: 100");
    this->Slider_hough_line_threshold_value->setFixedWidth(this->Slider_hough_line_threshold_value->sizeHint().width());
    this->Slider_hough_line_threshold_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->value_hough_line_threshold = 100;

    // Slider for Hough line transform threshold
    QSlider* Slider_hough_line_threshold = new QSlider( Qt::Horizontal );
    Slider_hough_line_threshold->setTickPosition(QSlider::TicksBothSides);
    Slider_hough_line_threshold->setTickInterval(5);
    Slider_hough_line_threshold->setSingleStep(1);
    Slider_hough_line_threshold->setRange(1,300);
    Slider_hough_line_threshold->setValue(this->value_hough_line_threshold);
    connect(Slider_hough_line_threshold, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_hough_line_treshold_value(int)));
    connect(Slider_hough_line_threshold, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_hough_line_treshold_value()) );

    // Tool tips when hovering the buttons and sliders
    radioButton1->setToolTip("Hough line transform: detect lines in the frame");
    radioButton2->setToolTip("Hough circle transform: detect circles in the frame");
    radioButton3->setToolTip("Harris corner detection: corners are regions in the image with large variation in intensity in all the directions.");
    Slider_hough_line_threshold->setToolTip("Set the threshold value for the object detectors.");

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(radioButton1,                           0, 0);
    grid->addWidget(radioButton2,                           1, 0);
    grid->addWidget(radioButton3,                           2, 0);
    grid->addWidget(Slider_hough_line_threshold,            3, 0);
    grid->addWidget(this->Slider_hough_line_threshold_value,3, 1);

    setLayout(grid);
    setWindowTitle(tr("Object detection control window"));
    resize(400, 400);
}

void Dialog_Object_Detection::onClick_Object_Detection_Method() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world with the method selected for object detection
    emit this->Signal_object_detection_method_changed( this->RadioButtons->checkedId() ) ;
}

void Dialog_Object_Detection::onClick_Slider_hough_line_treshold_value(int value) {
    // Function called when the slider for Hough line transform threshold is modified
    // Emits a signal to the external world with the threshold
    this->value_hough_line_threshold = value;
    emit this->Signal_hough_line_threshold_changed(this->value_hough_line_threshold);
}

void Dialog_Object_Detection::show_Slider_hough_line_treshold_value() {
    this->Slider_hough_line_threshold_value->setText("Threshold: "+QString::number(this->value_hough_line_threshold));
}
