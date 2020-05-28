/*
 * Copyright: Xavier Dechamps
*/

#ifndef DIALOG_PHOTO_H
#define DIALOG_PHOTO_H

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

class Dialog_Photo: public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Photo(QWidget *parent = nullptr);

private:
    QButtonGroup* RadioButtons;
    QLabel* Slider_sigmas_qlabel;
    QLabel* Slider_sigmar_qlabel;
    int value_sigmas;
    double value_sigmar,sigmar_min,sigmar_step;
    
private slots:
    void onClick_photo_Method();
    void onClick_Slider_sigmaS_value(int value);
    void onClick_Slider_sigmaR_value(int value);
    void show_Slider_sigmaS_value();
    void show_Slider_sigmaR_value();
    
signals:
    void Signal_photo_method_changed(int value);
    void Signal_photo_sigmaS_changed(int value);
    void Signal_photo_sigmaR_changed(double value);
    
};

#endif // DIALOG_PHOTO_H
