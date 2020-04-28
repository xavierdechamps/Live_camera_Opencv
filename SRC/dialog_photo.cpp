/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with the OpenCV module Photo
 *   Implemented methods are:
 *      * Contrast Preserving Decolorization
 *      * Denoizing (Non-Local Means)
 *      * Non-Photorealistic Rendering
 *          1) Edge preserving
 *          2) Detail enhancing
 *          3) Pencil sketch
 *          4) Stylization
 *      * Auto white balancing from module Xphoto
 *  Two sliders allow the adjustement of the parameters sigma_s and sigma_r
 *  for the non-photorealistic rendering
*/

#include "dialog_photo.h"

Dialog_Photo::Dialog_Photo(QWidget *parent):    QDialog(parent)
{
    // Group boxes for the buttons
    QGroupBox *groupBoxNPR = new QGroupBox("Non-Photorealistic Rendering",this);
    
    // Build the buttons
    QRadioButton *radioButton_1 = new QRadioButton("Contrast Preserving Decolorization",this); radioButton_1->setChecked(true);
    QRadioButton *radioButton_2 = new QRadioButton("Denoizing (Non-Local Means)",       this);
    QRadioButton *radioButton_3 = new QRadioButton("Edge preserving", groupBoxNPR); 
    QRadioButton *radioButton_4 = new QRadioButton("Detail enhancing",groupBoxNPR); 
    QRadioButton *radioButton_5 = new QRadioButton("Pencil sketch",   groupBoxNPR); 
    QRadioButton *radioButton_6 = new QRadioButton("Stylization",     groupBoxNPR); 
#ifdef withxphoto
    QRadioButton *radioButton_7 = new QRadioButton("White balancing", this); 
#endif
    
    // Create connections between button click and function
    connect(radioButton_1, SIGNAL(clicked()), this, SLOT(onClick_photo_Method()) ) ;
    connect(radioButton_2, SIGNAL(clicked()), this, SLOT(onClick_photo_Method()) ) ;
    connect(radioButton_3, SIGNAL(clicked()), this, SLOT(onClick_photo_Method()) ) ;
    connect(radioButton_4, SIGNAL(clicked()), this, SLOT(onClick_photo_Method()) ) ;
    connect(radioButton_5, SIGNAL(clicked()), this, SLOT(onClick_photo_Method()) ) ;
    connect(radioButton_6, SIGNAL(clicked()), this, SLOT(onClick_photo_Method()) ) ;
#ifdef withxphoto
    connect(radioButton_7, SIGNAL(clicked()), this, SLOT(onClick_photo_Method()) ) ;
#endif
    
    // Radio buttons (only one button is active at each moment)
    this->RadioButtons = new QButtonGroup(this);
    this->RadioButtons->addButton(radioButton_1,1);
    this->RadioButtons->addButton(radioButton_2,2);
    this->RadioButtons->addButton(radioButton_3,3);
    this->RadioButtons->addButton(radioButton_4,4);
    this->RadioButtons->addButton(radioButton_5,5);
    this->RadioButtons->addButton(radioButton_6,6);
#ifdef withxphoto
    this->RadioButtons->addButton(radioButton_7,7);
#endif
    
    // QLabel to show the value of the sigma s
    this->Slider_sigmas_qlabel = new QLabel("NPR Sigma s: 200", groupBoxNPR);
    this->Slider_sigmas_qlabel->setFixedWidth(this->Slider_sigmas_qlabel->sizeHint().width());
    this->Slider_sigmas_qlabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Slider_sigmas_qlabel->setText("NPR Sigma s: 50");
    this->value_sigmas = 50;

    // Slider to adapt the sigma s
    QSlider* Slider_sigmas_value = new QSlider( Qt::Horizontal , groupBoxNPR);
    Slider_sigmas_value->setTickPosition(QSlider::TicksBothSides);
    Slider_sigmas_value->setTickInterval(20);
    Slider_sigmas_value->setSingleStep(1);
    Slider_sigmas_value->setRange(0,200);
    Slider_sigmas_value->setValue(this->value_sigmas);
    connect(Slider_sigmas_value, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_sigmaS_value(int)));
    connect(Slider_sigmas_value, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_sigmaS_value()) );

    // QLabel to show the value of the sigma r
    this->Slider_sigmar_qlabel = new QLabel("NPR Sigma r: 1.00" , groupBoxNPR);
    this->Slider_sigmar_qlabel->setFixedWidth(this->Slider_sigmar_qlabel->sizeHint().width());
    this->Slider_sigmar_qlabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Slider_sigmar_qlabel->setText("NPR Sigma r: 0.15");
    this->value_sigmar = 0.15;
    this->sigmar_min   = 0.01;
    this->sigmar_step  = 0.01;

    // Slider to adapt the sigma r
    QSlider* Slider_sigmar_value = new QSlider( Qt::Horizontal , groupBoxNPR );
    Slider_sigmar_value->setTickPosition(QSlider::TicksBothSides);
    Slider_sigmar_value->setTickInterval(5);
    Slider_sigmar_value->setSingleStep(1);
    Slider_sigmar_value->setRange(0,99);
    Slider_sigmar_value->setValue(15);
    connect(Slider_sigmar_value, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_sigmaR_value(int)));
    connect(Slider_sigmar_value, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_sigmaR_value()) );
    
    // Tool tips when hovering the buttons and sliders
    radioButton_1->setToolTip("Transforms a color image to a grayscale image.");
    radioButton_2->setToolTip("Denoizing (Non-Local Means)");
    radioButton_3->setToolTip("Edge-preserving smoothing filter");
    radioButton_4->setToolTip("This filter enhances the details of a particular image. ");
    radioButton_5->setToolTip("Pencil-like non-photorealistic line drawing. ");
    radioButton_6->setToolTip("Stylization aims to produce digital imagery with a wide variety of effects not focused on photorealism.");
    Slider_sigmas_value->setToolTip("The size of the neighborhood is directly proportional to the parameter sigma_s.");
    Slider_sigmar_value->setToolTip("Sigma r controls the how dissimilar colors within the neighborhood will be averaged. A larger sigma_r results in large regions of constant color.");
#ifdef withxphoto
    radioButton_7->setToolTip("Auto white balancing from module xphoto");
#endif
    
    // Grid layout for the buttons, associated to a group
    QGridLayout *gridNPR = new QGridLayout(groupBoxNPR);
    gridNPR->addWidget(Slider_sigmas_value       , 0, 0);
    gridNPR->addWidget(this->Slider_sigmas_qlabel, 0, 1);
    gridNPR->addWidget(Slider_sigmar_value       , 1, 0);
    gridNPR->addWidget(this->Slider_sigmar_qlabel, 1, 1);
    gridNPR->addWidget(radioButton_3             , 2, 0);
    gridNPR->addWidget(radioButton_4             , 3, 0);
    gridNPR->addWidget(radioButton_5             , 4, 0);
    gridNPR->addWidget(radioButton_6             , 5, 0);
    
    // Grid layout
    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(radioButton_1,                0,0);
    grid->addWidget(radioButton_2,                1,0);
    grid->addWidget(groupBoxNPR,                  2,0);
#ifdef withxphoto
    grid->addWidget(radioButton_7,                3,0);
#endif
    
    setLayout(grid);
    setWindowTitle(tr("OpenCV Module Photo control window"));
    resize(400, 400);
}

void Dialog_Photo::onClick_photo_Method(){
    // Function called when the buttons are clicked
    // Emits a signal to the external world with the method selected for photo module
    emit this->Signal_photo_method_changed( this->RadioButtons->checkedId() ) ;
}

void Dialog_Photo::onClick_Slider_sigmaS_value(int value){
    // Function called when the slider for sigma s is modified.
    // Emits a signal to the external world with the value of the sigma s.
    this->value_sigmas = value;
    emit this->Signal_photo_sigmaS_changed(this->value_sigmas);
}

void Dialog_Photo::onClick_Slider_sigmaR_value(int value){
    // Function called when the slider for sigma r is modified.
    // Emits a signal to the external world with the value of the sigma r
    this->value_sigmar = this->sigmar_min + this->sigmar_step * value;
    emit this->Signal_photo_sigmaR_changed(this->value_sigmar);
}

void Dialog_Photo::show_Slider_sigmaS_value(){
    this->Slider_sigmas_qlabel->setText("NPR Sigma s: "+QString::number(this->value_sigmas));
}

void Dialog_Photo::show_Slider_sigmaR_value(){
    this->Slider_sigmar_qlabel->setText("NPR Sigma r: "+QString::number(this->value_sigmar));
}
