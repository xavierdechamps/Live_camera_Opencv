/*
 * Copyright: Xavier Dechamps
*/

#ifndef DIALOG_OBJECT_DETECTION_H
#define DIALOG_OBJECT_DETECTION_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QSizePolicy>
#include <QSlider>
#include <QRadioButton>
#include <QCheckBox>

#include <iostream>

class Dialog_Object_Detection: public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Object_Detection(QWidget *parent = 0);

private:
    int value_hough_line_threshold;
    QButtonGroup* RadioButtons;
    QLabel* Slider_hough_line_threshold_value;

private slots:
    void onClick_Object_Detection_Method();
    void onClick_Slider_hough_line_treshold_value(int value);

    void show_Slider_hough_line_treshold_value();

signals:
    void Signal_object_detection_method_changed(int);
    void Signal_hough_line_threshold_changed(int);
};

#endif // DIALOG_OBJECT_DETECTION_H