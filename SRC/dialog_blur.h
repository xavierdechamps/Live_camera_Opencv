/*
 * Copyright: Xavier Dechamps
*/

#ifndef DIALOG_BLUR_H
#define DIALOG_BLUR_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QSizePolicy>
#include <QSlider>
#include <QRadioButton>
#include <QGroupBox>
//#include <QToolTip> // show tool tips when hovering over buttons and sliders
//#include <QFont>

#include <iostream>
using namespace std;

class Dialog_Blur : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Blur(QWidget *parent = nullptr);
    ~Dialog_Blur();

private:
    QLabel* Slider_value, *Slider_element;
    int value_Blur_Range, value_Element;
    QButtonGroup* RadioButtons;
    
private slots:
    void onClick_Slider_Blur_Range(int value);
    void onClick_Slider_Element(int value);
    void onClick_Radio_Blur_Method();

    void show_Slider_value();
    void show_Slider_element();

signals:
    void Signal_blur_range_changed(int value);
    void Signal_blur_element_changed(int value); // element = [1] rectangle, [2] cross or [3] ellipse
    void Signal_blur_method_changed(int value);
};

#endif // DIALOG_BLUR_H
