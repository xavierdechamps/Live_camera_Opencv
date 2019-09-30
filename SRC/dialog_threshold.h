/*
 * Copyright: Xavier Dechamps
*/

#ifndef DIALOG_THRESHOLD_H
#define DIALOG_THRESHOLD_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QSizePolicy>
#include <QSlider>
#include <QRadioButton>
#include <QGroupBox>
#include <QCheckBox>

#include <iostream>
using namespace std;

class Dialog_Threshold : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Threshold(QWidget *parent = 0);

private:
    QLabel* Slider_value;
    QLabel* BlockSize_value;
    int value_threshold,value_blocksize;
    QButtonGroup* RadioButtons;

private slots:
    void onClick_Slider_Threshold_value(int value);
    void onClick_Slider_Adapt_Blocksize(int value);
    void onClick_Threshold_Method();
    void onClick_Botton_Threshold_Type(int value);

    void show_Slider_value();
    void show_Slider_Adapt_BlockSize();

signals:
    void Signal_threshold_value_changed(int value);
    void Signal_threshold_method_changed(int value);
    void Signal_blocksize_changed(int value);
    void Signal_adaptive_threshold_type(int value);
};

#endif // DIALOG_THRESHOLD_H
