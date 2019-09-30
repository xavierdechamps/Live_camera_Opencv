/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with edge detection
 *   Implemented methods are:
 *      1) Sobel derivatives
 *      2) Laplacian
 *      3) Canny edge detector
 *  Two sliders allow the adjustement of the low threshold and ratio for the Canny edge detector
*/

#include "dialog_edge.h"

Dialog_Edge::Dialog_Edge(QWidget *parent):    QDialog(parent)
{
    this->value_canny_lowthreshold = 50;
    this->value_canny_ratio = 2.5;
    this->canny_ratio_max   = 4.;
    this->canny_ratio_min   = 2.;
    this->canny_single_step = 0.1;

    // Build the buttons
    QRadioButton* radioButton1 = new QRadioButton("Sobel derivatives"); radioButton1->setChecked(true);
    QRadioButton* radioButton2 = new QRadioButton("Laplacian");
    QRadioButton* radioButton3 = new QRadioButton("Canny edge detector");

    // Create connections between button click and function
    connect(radioButton1, SIGNAL(clicked()), this, SLOT(onClick_Edge_Method()) ) ;
    connect(radioButton2, SIGNAL(clicked()), this, SLOT(onClick_Edge_Method()) ) ;
    connect(radioButton3, SIGNAL(clicked()), this, SLOT(onClick_Edge_Method()) ) ;

    // Radio buttons (only one button is active at each moment)
    this->RadioButtons = new QButtonGroup();
    this->RadioButtons->addButton(radioButton1,1);
    this->RadioButtons->addButton(radioButton2,2);
    this->RadioButtons->addButton(radioButton3,3);

    //  QLabel to show the Canny low threshold
    this->Slider_canny_lowthreshold_value = new QLabel("Low threshold: 150"); // To have the biggest length for the string
    this->Slider_canny_lowthreshold_value->setFixedWidth(this->Slider_canny_lowthreshold_value->sizeHint().width());
    this->Slider_canny_lowthreshold_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Slider_canny_lowthreshold_value->setText("Low threshold: 50"); // Set the default string
    this->value_canny_lowthreshold = 50; // Set the default value

    // Slider to adapt the Canny low threshold
    QSlider* Slider_canny_threshold = new QSlider( Qt::Horizontal );
    Slider_canny_threshold->setTickPosition(QSlider::TicksBothSides);
    Slider_canny_threshold->setTickInterval(10);
    Slider_canny_threshold->setSingleStep(1);
    Slider_canny_threshold->setRange(0,100);
    Slider_canny_threshold->setValue(this->value_canny_lowthreshold);
    connect(Slider_canny_threshold, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_canny_lowthreshold_value(int)));
    connect(Slider_canny_threshold, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_canny_threshold_value()) );

    //  QLabel to show the Canny ratio
    this->Slider_canny_ratio_value = new QLabel("Ratio: 2.5");
    this->Slider_canny_ratio_value->setFixedWidth(this->Slider_canny_ratio_value->sizeHint().width());
    this->Slider_canny_ratio_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->value_canny_ratio = 2.5;

    // Slider to adapt the Canny ratio
    QSlider* Slider_canny_value = new QSlider( Qt::Horizontal );
    Slider_canny_value->setTickPosition(QSlider::TicksBothSides);
    Slider_canny_value->setTickInterval(5);
    Slider_canny_value->setSingleStep(1);
    Slider_canny_value->setRange(0,20);
    Slider_canny_value->setValue(5);
    connect(Slider_canny_value, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_canny_ratio_value(int)));
    connect(Slider_canny_value, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_canny_ratio_value()) );

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(radioButton1,          0, 0);
    grid->addWidget(radioButton2,          1, 0);
    grid->addWidget(radioButton3,          2, 0);
    grid->addWidget(Slider_canny_threshold,3, 0);
    grid->addWidget(Slider_canny_lowthreshold_value,3, 1);
    grid->addWidget(Slider_canny_value,4, 0);
    grid->addWidget(Slider_canny_ratio_value,4, 1);

    setLayout(grid);
    setWindowTitle(tr("Edge detector control window"));
    resize(400, 400);
}

void Dialog_Edge::onClick_Edge_Method() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world with the method selected for edge detection
    emit this->Signal_edge_method_changed( RadioButtons->checkedId() ) ;
}

void Dialog_Edge::onClick_Slider_canny_lowthreshold_value(int value) {
    // Function called when the slider for Canny low threshold is modified.
    // Emits a signal to the external world with the value of the threshold
    this->value_canny_lowthreshold = value;
    emit this->Signal_canny_lowthreshold_changed(this->value_canny_lowthreshold);
}

void Dialog_Edge::onClick_Slider_canny_ratio_value(int value) {
    // Function called when the slider for Canny ratio is modified.
    // Emits a signal to the external world with the value of the Canny ratio
    this->value_canny_ratio = this->canny_ratio_min + this->canny_single_step * value;
    emit this->Signal_canny_ratio_changed(this->value_canny_ratio);
}

void Dialog_Edge::show_Slider_canny_threshold_value() {
    this->Slider_canny_lowthreshold_value->setText("Low threshold: "+QString::number(this->value_canny_lowthreshold));
}

void Dialog_Edge::show_Slider_canny_ratio_value() {
    this->Slider_canny_ratio_value->setText("Ratio: "+QString::number(this->value_canny_ratio));
}
