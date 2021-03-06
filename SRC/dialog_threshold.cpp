/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with threshold
 *   Implemented methods are:
 *      * Global threshold methods
 *          1) Binary
 *          2) Binary inverted
 *          3) Truncate
 *          4) To zero
 *          5) To zero inverted
 *          6) Otsu method
 *          7) Triangle method
 *      * Adaptive threshold methods
 *          1) Mean
 *          2) Gaussian
 *  One slider allows the adjustement of the global threshold
 *  One slider allows the adjustement of the block size for adaptive methods
*/

#include "dialog_threshold.h"

/**
 * @brief Dialog_Threshold::Dialog_Threshold
 * @param parent
 * 
 * Constructor of the class Dialog_Threshold. Set the appearance of the widget and create connections 
 * between the buttons/sliders and the functionnalities
 */
Dialog_Threshold::Dialog_Threshold(QWidget *parent):    QDialog(parent)
{
    // Group boxes for all the global/adaptive threshold methods
    QGroupBox *groupBox= new QGroupBox("Global threshold",this); //groupBox->setCheckable(true);
    QGroupBox *groupBoxAdapt= new QGroupBox("Adaptive threshold",this); //groupBox->setCheckable(true);

    // Build the buttons
    QRadioButton *radioButton_1 = new QRadioButton("Binary",          groupBox); radioButton_1->setChecked(true);
    QRadioButton *radioButton_2 = new QRadioButton("Binary inverted", groupBox);
    QRadioButton *radioButton_3 = new QRadioButton("Truncate",        groupBox);
    QRadioButton *radioButton_4 = new QRadioButton("To zero",         groupBox);
    QRadioButton *radioButton_5 = new QRadioButton("To zero inverted",groupBox);
    QRadioButton *radioButton_6 = new QRadioButton("Otsu threshold",  groupBox);
    QRadioButton *radioButton_7 = new QRadioButton("Triangle method threshold",groupBox);
    
    QCheckBox    *thresholdType = new QCheckBox(tr("Adaptive threshold type (normal/inverse)"), groupBoxAdapt); thresholdType->setChecked(true);
    QRadioButton *radioButton_8 = new QRadioButton("Adaptive threshold (mean)",                 groupBoxAdapt);
    QRadioButton *radioButton_9 = new QRadioButton("Adaptive threshold (Gaussian)",             groupBoxAdapt);
    
    // QLabel to show the value of the global threshold
    this->Slider_value = new QLabel("Threshold: 127",groupBox);
    this->Slider_value->setFixedWidth(this->Slider_value->sizeHint().width());
    this->Slider_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->value_threshold = 127;

    // QLabel to show the value of the block size for adaptive methods
    this->BlockSize_value = new QLabel("Block size: 31",groupBoxAdapt);
    this->BlockSize_value->setFixedWidth(this->Slider_value->sizeHint().width());
    this->BlockSize_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->BlockSize_value->setText("Block size: 3");
    this->value_blocksize = 3;

    // Slider to adapt the global threshold
    QSlider* Slider_threshold_value = new QSlider( Qt::Horizontal, groupBox);
    Slider_threshold_value->setTickPosition(QSlider::TicksBothSides);
    Slider_threshold_value->setTickInterval(20);
    Slider_threshold_value->setSingleStep(1);
    Slider_threshold_value->setRange(0,255);
    Slider_threshold_value->setSliderPosition(this->value_threshold);
    connect(Slider_threshold_value, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_Threshold_value(int)));
    connect(Slider_threshold_value, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_value()) );

    // Slider to adapt the block size for adaptive methods
    QSlider* Slider_adapt_thresh_blockSize = new QSlider( Qt::Horizontal,groupBoxAdapt);
    Slider_adapt_thresh_blockSize->setTickPosition(QSlider::TicksBothSides);
    Slider_adapt_thresh_blockSize->setTickInterval(2);
    Slider_adapt_thresh_blockSize->setSingleStep(2);
    Slider_adapt_thresh_blockSize->setRange(1,15);
    connect(Slider_adapt_thresh_blockSize, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_Adapt_Blocksize(int)));
    connect(Slider_adapt_thresh_blockSize, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_Adapt_BlockSize()) );
    
    // Grid layout inside the groupbox for global threshod
    QGridLayout *gridGlobal = new QGridLayout(groupBox);
    gridGlobal->addWidget(Slider_threshold_value , 0, 0);
    gridGlobal->addWidget(this->Slider_value           , 0, 1);
    gridGlobal->addWidget(radioButton_1                , 1, 0);
    gridGlobal->addWidget(radioButton_2                , 2, 0);
    gridGlobal->addWidget(radioButton_3                , 3, 0);
    gridGlobal->addWidget(radioButton_4                , 4, 0);
    gridGlobal->addWidget(radioButton_5                , 5, 0);
    gridGlobal->addWidget(radioButton_6                , 6, 0);
    gridGlobal->addWidget(radioButton_7                , 7, 0);

    // Grid layout inside the groupbox for adaptive threshod
    QGridLayout *gridAdaptive = new QGridLayout(groupBoxAdapt);
    gridAdaptive->addWidget(Slider_adapt_thresh_blockSize, 0, 0 );
    gridAdaptive->addWidget(this->BlockSize_value        , 0, 1);
    gridAdaptive->addWidget(thresholdType                , 1, 0);
    gridAdaptive->addWidget(radioButton_8                , 2, 0);
    gridAdaptive->addWidget(radioButton_9                , 3, 0);

    // Create connections between button click and function
    connect(radioButton_1, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_2, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_3, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_4, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_5, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_6, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_7, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_8, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(radioButton_9, SIGNAL(clicked()), this, SLOT(onClick_Threshold_Method()) ) ;
    connect(thresholdType, SIGNAL(stateChanged(int)), this, SLOT(onClick_Botton_Threshold_Type(int)));

    // Tool tips when hovering the buttons and sliders
    radioButton_1->setToolTip("Binary: if the intensity of the pixel is higher than threshold, then the new pixel intensity is set to a MaxVal. Otherwise, the pixels are set to 0.");
    radioButton_2->setToolTip("Binary inverted: If the intensity of the pixel is higher than threshold, then the new pixel intensity is set to a 0. Otherwise, it is set to MaxVal");
    radioButton_3->setToolTip("Truncate: the maximum intensity value for the pixels is threshold, if src(x,y) is greater, then its value is truncated");
    radioButton_4->setToolTip("To zero: if src(x,y) is lower than thresh, the new pixel value will be set to 0.");
    radioButton_5->setToolTip("To zero inverted: if src(x,y) is greater than thresh, the new pixel value will be set to 0.");
    radioButton_6->setToolTip("Otsu threshold: use Otsu algorithm to choose the optimal threshold value");
    radioButton_7->setToolTip("Triangle method threshold: use Triangle algorithm to choose the optimal threshold value");
    radioButton_8->setToolTip("Adaptive threshold (mean): the threshold is computed for small subregions of the frame");
    radioButton_9->setToolTip("Adaptive threshold (Gaussian): the threshold is computed for small subregions of the frame");
    thresholdType->setToolTip("Inverse the input for the adaptive threshold methods");

    this->RadioButtons = new QButtonGroup(this); // to have exclusive radio buttons
    this->RadioButtons->addButton(radioButton_1,1);
    this->RadioButtons->addButton(radioButton_2,2);
    this->RadioButtons->addButton(radioButton_3,3);
    this->RadioButtons->addButton(radioButton_4,4);
    this->RadioButtons->addButton(radioButton_5,5);
    this->RadioButtons->addButton(radioButton_6,6);
    this->RadioButtons->addButton(radioButton_7,7);
    this->RadioButtons->addButton(radioButton_8,8);
    this->RadioButtons->addButton(radioButton_9,9);

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(groupBox,      0,0);
    grid->addWidget(groupBoxAdapt, 1,0);

    setLayout(grid);
    setWindowTitle(tr("Threshold control window"));
    resize(400, 400);
}

/**
 * @brief Dialog_Threshold::onClick_Slider_Threshold_value
 * @param value: integer value from the slider that manages the global threshold
 * 
 * Function called when the slider for the global threshold is modified.
 * Emits a signal to the external world with the threshold
 */
void Dialog_Threshold::onClick_Slider_Threshold_value(int value) {
    this->value_threshold = value;
    emit this->Signal_threshold_value_changed(this->value_threshold);
}

/**
 * @brief Dialog_Threshold::onClick_Slider_Adapt_Blocksize
 * @param value: integer value from the slider that manages the block size for adaptive methods
 * 
 * Function called when the slider for the block size for adaptive methods is modified.
 * Emits a signal to the external world with the block size
 */
void Dialog_Threshold::onClick_Slider_Adapt_Blocksize(int value) {
    this->value_blocksize = 2*value + 1;
    emit this->Signal_blocksize_changed(this->value_blocksize);
}

/**
 * @brief Dialog_Threshold::onClick_Threshold_Method
 * 
 * Function called when the buttons are clicked
 * Emits a signal to the external world with the method selected for threshold
 */
void Dialog_Threshold::onClick_Threshold_Method() {
    emit this->Signal_threshold_method_changed( this->RadioButtons->checkedId() ) ;
}

/**
 * @brief Dialog_Threshold::onClick_Botton_Threshold_Type
 * @param value: integer value from the checkbox that manages the type (normal/inverted) for adaptive threshold methods
 * 
 * Function called when the checkbox is clicked
 * Emits a signal to the external world with the the type (normal/inverted) for adaptive threshold methods
 */
void Dialog_Threshold::onClick_Botton_Threshold_Type(int value) {
    emit Signal_adaptive_threshold_type(value);
}

/**
 * @brief Dialog_Threshold::show_Slider_value
 * 
 * Function called when the slider for the global threshold is modified.
 * Set the text accordingly next to the slider
 */
void Dialog_Threshold::show_Slider_value() {
    this->Slider_value->setText("Threshold: "+QString::number(this->value_threshold));
}

/**
 * @brief Dialog_Threshold::show_Slider_Adapt_BlockSize
 * 
 * Function called when the slider for the block size for adaptive methods is modified.
 * Set the text accordingly next to the slider
 */
void Dialog_Threshold::show_Slider_Adapt_BlockSize() {
    this->BlockSize_value->setText("Block size: "+QString::number(this->value_blocksize));
}
