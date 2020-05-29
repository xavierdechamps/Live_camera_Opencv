/*
 * Copyright (C) 2019-2020 Xavier Dechamps
*/

#ifndef DIALOG_TRANSFORMATION_H
#define DIALOG_TRANSFORMATION_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QSizePolicy>
#include <QSlider>
#include <QRadioButton>

class Dialog_Transformation : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Transformation(QWidget *parent = nullptr);

private:
    QLabel* Slider_rotation_qlabel;
    int value_rotation;
    QButtonGroup* RadioButtons;

private slots:
    void onClick_Slider_rotation_value(int value);
    void onClick_Transformation_Method();
    void show_Slider_rotation_value();

signals:
    void Signal_transformation_method_changed(int value);
    void Signal_transformation_rotation_changed(int value);
};

#endif // DIALOG_TRANSFORMATION_H
