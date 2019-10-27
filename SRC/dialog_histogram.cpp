/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with histogram equalization
 *   Implemented methods are:
 *      1) Global histogram equalization
 *      2) Contrast Limited Adaptive Histogram Equalization (CLAHE)
 *  Two sliders allow the adjustement of the clip limit and number of tiles for the CLAHE
 *  One check box displays an additional window with the histogram
*/

#include "dialog_histogram.h"

Dialog_Histogram::Dialog_Histogram(QWidget *parent):    QDialog(parent)
{

    // Group box for the blur buttons
    QGroupBox *groupBoxCLAHE  = new QGroupBox("Contrast Limited Adaptive Histogram Equalization",this);

    // Build the buttons
    QRadioButton* radioButton1 = new QRadioButton("Global histogram equalization",this); radioButton1->setChecked(true);
    QRadioButton* radioButton2 = new QRadioButton("Contrast Limited Adaptive Histogram Equalization",groupBoxCLAHE);

    // Radio buttons (only one button is active at each moment)
    RadioButtons = new QButtonGroup();
    RadioButtons->addButton(radioButton1,1);
    RadioButtons->addButton(radioButton2,2);

    // Create connections between button click and function
    connect(radioButton1, SIGNAL(clicked()), this, SLOT(onClick_Radio_Histogram_Method()) ) ;
    connect(radioButton2, SIGNAL(clicked()), this, SLOT(onClick_Radio_Histogram_Method()) ) ;

    // Slider to adapt the clip limit (CLAHE)
    QSlider* Slider_clip_limit = new QSlider( Qt::Horizontal , groupBoxCLAHE);
    Slider_clip_limit->setTickPosition(QSlider::TicksBothSides);
    Slider_clip_limit->setTickInterval(5);
    Slider_clip_limit->setSingleStep(1);
    Slider_clip_limit->setRange(1,40);
    Slider_clip_limit->setSliderPosition(2);
    connect(Slider_clip_limit, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_Clip_limit(int)));
    connect(Slider_clip_limit, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_Clip_Limit_value()) );

    // QLabel to show the clip limit (CLAHE)
    this->Slider_clip_limit_value = new QLabel("Clip limit: 40", groupBoxCLAHE);
    this->Slider_clip_limit_value->setFixedWidth(this->Slider_clip_limit_value->sizeHint().width());
    this->Slider_clip_limit_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Slider_clip_limit_value->setText("Clip limit: 2");
    this->value_clip_limit = 2;

    // Slider to adapt the number of tiles (CLAHE)
    QSlider* Slider_tiles = new QSlider( Qt::Horizontal , groupBoxCLAHE);
    Slider_tiles->setTickPosition(QSlider::TicksBothSides);
    Slider_tiles->setTickInterval(1);
    Slider_tiles->setSingleStep(1);
    Slider_tiles->setRange(1,10);
    Slider_tiles->setSliderPosition(8);
    connect(Slider_tiles, SIGNAL(valueChanged(int)), this, SLOT(onClick_Slider_Tiles(int)));
    connect(Slider_tiles, SIGNAL(valueChanged(int)), this, SLOT(show_Slider_Tiles_value()) );

    // QLabel to show the number of tiles (CLAHE)
    this->Slider_tiles_value = new QLabel("Number of tiles: 10" , groupBoxCLAHE);
    this->Slider_tiles_value->setFixedWidth(this->Slider_tiles_value->sizeHint().width());
    this->Slider_tiles_value->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->Slider_tiles_value->setText("Number of tiles: 8");
    this->value_tiles = 8;

    // QCheckBox to show the histogram
    QCheckBox *showHistogram = new QCheckBox(tr("Show the histogram"));
    connect(showHistogram, SIGNAL(stateChanged(int)), this, SLOT(onClick_Check_Show_Histogram(int)) );
    showHistogram->setChecked(false);

    // Tool tips when hovering the buttons and sliders
    radioButton1->setToolTip("Global histogram equalization is good when histogram of the image is confined to a particular region. It will not work good in places where there is large intensity variations where histogram covers a large region");
    radioButton2->setToolTip("CLAHE: the image is divided into small blocks where Global histogram equalization is applied.");
    Slider_clip_limit->setToolTip("CLAHE: set the size of the small blocks (tiles) in pixels");
    Slider_tiles->setToolTip("CLAHE: set the clip limit to avoid noise amplification inside each tile");

    QVBoxLayout *verticalLayoutCLAHE  = new QVBoxLayout(groupBoxCLAHE);
    verticalLayoutCLAHE->addWidget(radioButton2);
    verticalLayoutCLAHE->addWidget(this->Slider_clip_limit_value);
    verticalLayoutCLAHE->addWidget(Slider_clip_limit);
    verticalLayoutCLAHE->addWidget(this->Slider_tiles_value);
    verticalLayoutCLAHE->addWidget(Slider_tiles);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(radioButton1,                 0, 0);
    grid->addWidget(groupBoxCLAHE,                1, 0);
    grid->addWidget(showHistogram,                2, 0);

    setLayout(grid);
    setWindowTitle(tr("Histogram control window"));
    resize(400, 400);
}

void Dialog_Histogram::onClick_Radio_Histogram_Method() {
    // Function called when the buttons are clicked
    // Emits a signal to the external world with the method selected for histogram equalization
    emit this->Signal_histogram_method_changed( this->RadioButtons->checkedId() ) ;
}

void Dialog_Histogram::onClick_Slider_Clip_limit(int value) {
    // Function called when the slider for clip limit (CLAHE) is modified
    // Emits a signal to the external world with the clip limit
    this->value_clip_limit = value;
    emit this->Signal_histogram_clip_limit_changed(this->value_clip_limit);
}

void Dialog_Histogram::onClick_Slider_Tiles(int value) {
    // Function called when the slider for number of tiles (CLAHE) is modified
    // Emits a signal to the external world with the number of tiles
    this->value_tiles = value;
    emit this->Signal_histogram_tiles_changed(this->value_tiles);
}

void Dialog_Histogram::onClick_Check_Show_Histogram(int state) {
    // Function called when the check box for histogram is modified
    // Emits a signal to the external world with the new state of the check box
    emit this->Signal_histogram_show_histogram( state == Qt::Checked );
}

void Dialog_Histogram::show_Slider_Clip_Limit_value() {
    this->Slider_clip_limit_value->setText("Clip limit: "+QString::number(this->value_clip_limit));
}

void Dialog_Histogram::show_Slider_Tiles_value() {
    this->Slider_tiles_value->setText("Number of tiles: "+QString::number(this->value_tiles));
}
