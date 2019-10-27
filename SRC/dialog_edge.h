/*
 * Copyright: Xavier Dechamps
*/

#ifndef DIALOG_EDGE_H
#define DIALOG_EDGE_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QSizePolicy>
#include <QSlider>
#include <QRadioButton>
#include <QGroupBox>

#include <iostream>
using namespace std;

class Dialog_Edge : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Edge(QWidget *parent = 0);

private:
    QLabel* Slider_canny_lowthreshold_value;
    QLabel* Slider_canny_ratio_value;
    int value_canny_lowthreshold;
    double value_canny_ratio,canny_ratio_max,canny_ratio_min,canny_single_step;
    QButtonGroup* RadioButtons;

private slots:
    void onClick_Edge_Method();
    void onClick_Slider_canny_lowthreshold_value(int value);
    void onClick_Slider_canny_ratio_value(int value);

    void show_Slider_canny_threshold_value();
    void show_Slider_canny_ratio_value();

signals:
    void Signal_edge_method_changed(int value);
    void Signal_canny_lowthreshold_changed(int value);
    void Signal_canny_ratio_changed(double value);
    void Signal_hough_line_threshold_changed(int value);
};

#endif // DIALOG_EDGE_H
