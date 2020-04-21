/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with smoothing filters and morphological transformations.
 *   Implemented methods are:
 *      * Smoothing filters
 *          1) Simple blur
 *          2) Gaussian blur
 *          3) Median blur
 *          4) Bilateral blur
 *      * Morphological transformations
 *          1) Erode
 *          2) Dilate
 *          3) Opening
 *          4) Closing
 *          5) Morphological gradient
 *          6) Top hat
 *          7) Black hat
 *          8) Hit / Miss
 *  Two sliders allow the adjustement of the filter size and the type of element used for the
 *  morphological transformations (rectangular, cross or elliptical)
*/

#include "dialog_blur.h"

Dialog_Blur::Dialog_Blur(QWidget *parent):    QDialog(parent)
{
    // QLabel to display the size of the filter
    this->Slider_value = new QLabel("Range of filter: 151"); // To have the biggest length for the string
    this->Slider_value->setFixedWidth(this->Slider_value->sizeHint().width());
    this->Slider_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Slider_value->setText("Range of filter: 3"); // Set the default string
    this->value_Blur_Range = 3; // Set the default value

    // QLabel to display the type of element in the morphological transformation
    this->Slider_element = new QLabel("Element: rectangle");// To have the biggest length for the string
    this->Slider_element->setFixedWidth(this->Slider_value->sizeHint().width());
    this->Slider_element->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->value_Element = 1; // Set the default value

    // Slider for blur range
    QSlider* Slider_range_filter = new QSlider( Qt::Horizontal );
    Slider_range_filter->setTickPosition(QSlider::TicksBothSides);
    Slider_range_filter->setTickInterval(2);
    Slider_range_filter->setSingleStep(2);
    Slider_range_filter->setRange(1,15);
    connect(Slider_range_filter, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_Blur_Range(int)));
    connect(Slider_range_filter, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_value()) );

    // Slider for element type in morphological transformations
    QSlider* Slider_type_element = new QSlider( Qt::Horizontal );
    Slider_type_element->setTickPosition(QSlider::TicksBothSides);
    Slider_type_element->setTickInterval(1);
    Slider_type_element->setSingleStep(1);
    Slider_type_element->setRange(1,3);
    connect(Slider_type_element, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_Element(int)));
    connect(Slider_type_element, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_element()) );

    // Group box for the blur buttons
    QGroupBox *groupBoxBlurs  = new QGroupBox("Bluring",this);
    QGroupBox *groupBoxMorpho = new QGroupBox("Morphological",this);

    // Build the buttons for blur filters (only one button is active at each moment)
    QRadioButton* radioButton1 = new QRadioButton("Blur",            groupBoxBlurs); radioButton1->setChecked(true); // Default blur
    QRadioButton* radioButton2 = new QRadioButton("Gaussian blur",   groupBoxBlurs);
    QRadioButton* radioButton3 = new QRadioButton("Median blur",     groupBoxBlurs);
    QRadioButton* radioButton4 = new QRadioButton("Bilateral filter",groupBoxBlurs);

    // Build the buttons for the morphological transformation buttons
    QRadioButton* radioButton5 = new QRadioButton("Erode filter",                groupBoxMorpho);
    QRadioButton* radioButton6 = new QRadioButton("Dilate filter",               groupBoxMorpho);
    QRadioButton* radioButton7 = new QRadioButton("Opening",                     groupBoxMorpho);
    QRadioButton* radioButton8 = new QRadioButton("Closing",                     groupBoxMorpho);
    QRadioButton* radioButton9 = new QRadioButton("Morphological gradient",      groupBoxMorpho);
    QRadioButton* radioButton10 = new QRadioButton("Top Hat",                    groupBoxMorpho);
    QRadioButton* radioButton11 = new QRadioButton("Black Hat",                  groupBoxMorpho);
    QRadioButton* radioButton12 = new QRadioButton("Hit/Miss (kernel to change)",groupBoxMorpho);

    // Create connections between button click and function
    connect(radioButton1, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton2, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton3, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton4, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton5, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton6, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton7, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton8, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton9, SIGNAL(clicked()),  this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton10, SIGNAL(clicked()), this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton11, SIGNAL(clicked()), this, SLOT(onClick_Radio_Blur_Method()) ) ;
    connect(radioButton12, SIGNAL(clicked()), this, SLOT(onClick_Radio_Blur_Method()) ) ;

    // Tool tips when hovering the buttons
//    QFont serifFont("Times", 14, QFont::Normal);
//    QToolTip::setFont(serifFont);
    radioButton1->setToolTip("Simple blur effect using a normalized box filter");
    radioButton2->setToolTip("Blur effect using a 2D Gaussian kernel");
    radioButton3->setToolTip("Each pixel is replaced with the median of its neighboring pixels");
    radioButton4->setToolTip("Smooth the image but preserve the edges");
    radioButton5->setToolTip("Erosion: compute a local minimum over the area of the kernel");
    radioButton6->setToolTip("Dilation: compute a local maximum over the area of the kernel");
    radioButton7->setToolTip("Opening: an erosion followed by a dilation. Useful to remove noise.");
    radioButton8->setToolTip("Closing: a dilation followed by an erosion. Useful in closing small holes inside the foreground objects, or small black points on the object.");
    radioButton9->setToolTip("Morphological gradient: difference between a dilation and an erosion");
    radioButton10->setToolTip("Top hat: difference between the input image and the opening of the image");
    radioButton11->setToolTip("Black hat: difference between the closing of the input image and the input image.");
    radioButton12->setToolTip("Hit-or-miss: useful to find patterns in binary images.");

    // Radio buttons
    this->RadioButtons = new QButtonGroup(); // contains all the buttons
    this->RadioButtons->addButton(radioButton1,1);
    this->RadioButtons->addButton(radioButton2,2);
    this->RadioButtons->addButton(radioButton3,3);
    this->RadioButtons->addButton(radioButton4,4);
    this->RadioButtons->addButton(radioButton5 ,5 );
    this->RadioButtons->addButton(radioButton6 ,6 );
    this->RadioButtons->addButton(radioButton7 ,7 );
    this->RadioButtons->addButton(radioButton8 ,8 );
    this->RadioButtons->addButton(radioButton9 ,9 );
    this->RadioButtons->addButton(radioButton10,10);
    this->RadioButtons->addButton(radioButton11,11);
    this->RadioButtons->addButton(radioButton12,12);

    // Vertical layout for the buttons, associated to a group
    QVBoxLayout *verticalLayoutBlurs  = new QVBoxLayout(groupBoxBlurs);
    QVBoxLayout *verticalLayoutMorpho = new QVBoxLayout(groupBoxMorpho);

    verticalLayoutBlurs->addWidget(radioButton1);
    verticalLayoutBlurs->addWidget(radioButton2);
    verticalLayoutBlurs->addWidget(radioButton3);
    verticalLayoutBlurs->addWidget(radioButton4);
    verticalLayoutMorpho->addWidget(radioButton5);
    verticalLayoutMorpho->addWidget(radioButton6);
    verticalLayoutMorpho->addWidget(radioButton7);
    verticalLayoutMorpho->addWidget(radioButton8);
    verticalLayoutMorpho->addWidget(radioButton9);
    verticalLayoutMorpho->addWidget(radioButton10);
    verticalLayoutMorpho->addWidget(radioButton11);
    verticalLayoutMorpho->addWidget(radioButton12);

    // Grid layout
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(Slider_range_filter, 0, 0);
    grid->addWidget(this->Slider_value,  0, 1);
    grid->addWidget(groupBoxBlurs,       1, 0);
    grid->addWidget(Slider_type_element, 2, 0);
    grid->addWidget(this->Slider_element,2, 1);
    grid->addWidget(groupBoxMorpho,      3, 0);

    setLayout(grid);
    setWindowTitle(tr("Blur control window"));
    resize(400, 400);
}

Dialog_Blur::~Dialog_Blur() {}

void Dialog_Blur::onClick_Slider_Blur_Range(int value) {
    // Function called when the slider for filter range is modified.
    // Emits a signal to the external world with the size of the filter
    this->value_Blur_Range = 2*value + 1;
    emit this->Signal_blur_range_changed(this->value_Blur_Range);
}

void Dialog_Blur::onClick_Slider_Element(int value) {
    // Function called when the slider for type of element is modified.
    // Emits a signal to the external world with the type of element (rect., cross, ellipse)
    this->value_Element = value;
    emit this->Signal_blur_element_changed(this->value_Element);
}

void Dialog_Blur::onClick_Radio_Blur_Method() {
    // Function called when the buttons are clicked
    // Send the signal to the external world
    emit this->Signal_blur_method_changed( this->RadioButtons->checkedId() ) ;
}

void Dialog_Blur::show_Slider_element() {
    QString txt = "Element: ";
    switch (this->value_Element) {
        case 1:
            txt+="rectangle";
            break;
        case 2:
            txt+="cross";
            break;
        case 3:
            txt+="ellipse";
            break;
        default:
            cout << "Dialog_Blur::show_Slider_value(): unknown type of element"<<endl;
            break;
    }
    this->Slider_element->setText(txt);
}

void Dialog_Blur::show_Slider_value() {
    this->Slider_value->setText("Range of filter: "+QString::number(this->value_Blur_Range));
}
