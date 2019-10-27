/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with geometric transformations
 *   Implemented methods are:
 *      1) Rotation
 *  One slider allows the adjustement of the rotation angle
*/

#include "dialog_transformation.h"

Dialog_Transformation::Dialog_Transformation(QWidget *parent):    QDialog(parent)
{
    // QLabel to show the value of the rotation angle
    this->Slider_rotation_qlabel = new QLabel("Rotation: -360");
    this->Slider_rotation_qlabel->setFixedWidth(this->Slider_rotation_qlabel->sizeHint().width());
    this->Slider_rotation_qlabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Slider_rotation_qlabel->setText("Rotation: 0");
    this->value_rotation = 0;

    // Slider to adapt the rotation angle
    QSlider* Slider_rotation_value = new QSlider( Qt::Horizontal );
    Slider_rotation_value->setTickPosition(QSlider::TicksBothSides);
    Slider_rotation_value->setTickInterval(20);
    Slider_rotation_value->setSingleStep(1);
    Slider_rotation_value->setRange(-179,180);
    Slider_rotation_value->setSliderPosition(this->value_rotation);
    connect(Slider_rotation_value, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_rotation_value(int)));
    connect(Slider_rotation_value, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_rotation_value()) );

    // Build the buttons
    QRadioButton *radioButton_1 = new QRadioButton("Rotation",this); radioButton_1->setChecked(true);

    // Create connections between button click and function
    connect(radioButton_1, SIGNAL(clicked()), this, SLOT(onClick_Transformation_Method()) ) ;

    // Radio buttons (only one button is active at each moment)
    this->RadioButtons = new QButtonGroup(this);
    this->RadioButtons->addButton(radioButton_1,1);

    // Tool tips when hovering the buttons and sliders
    radioButton_1->setToolTip("Rotate the frame by a given angle around the centre of the frame");
    Slider_rotation_value->setToolTip("Value of the angle required for the rotation");

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(radioButton_1,                0,0);
    grid->addWidget(Slider_rotation_value,        1,0);
    grid->addWidget(this->Slider_rotation_qlabel, 1,1);

    setLayout(grid);
    setWindowTitle(tr("Geometric transformations control window"));
    resize(400, 400);
}

void Dialog_Transformation::onClick_Slider_rotation_value(int value){
    // Function called when the slider for the rotation angle is modified.
    // Emits a signal to the external world with the rotation angle
    this->value_rotation = value;
    emit this->Signal_transformation_rotation_changed(this->value_rotation);
}

void Dialog_Transformation::onClick_Transformation_Method() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world with the method selected for geometric transformation
    emit this->Signal_transformation_method_changed( this->RadioButtons->checkedId() ) ;
}

void Dialog_Transformation::show_Slider_rotation_value(){
    this->Slider_rotation_qlabel->setText("Rotation: "+QString::number(this->value_rotation));
}
