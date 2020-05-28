/*
 * Copyright: Xavier Dechamps
*/

#ifndef DIALOG_HISTOGRAM_H
#define DIALOG_HISTOGRAM_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QSizePolicy>
#include <QSlider>
#include <QRadioButton>
#include <QCheckBox>
#include <QGroupBox>

#include <iostream>

class Dialog_Histogram : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Histogram(QWidget *parent = nullptr);

private:
    int value_clip_limit;
    int value_tiles;
    QLabel* Slider_clip_limit_value;
    QLabel* Slider_tiles_value;
    QButtonGroup* RadioButtons;

private slots:
    void onClick_Radio_Histogram_Method();
    void onClick_Slider_Clip_limit(int);
    void onClick_Slider_Tiles(int);
    void onClick_Check_Show_Histogram(int);

    void show_Slider_Clip_Limit_value();
    void show_Slider_Tiles_value();

signals:
    void Signal_histogram_method_changed(int);
    void Signal_histogram_clip_limit_changed(int);
    void Signal_histogram_tiles_changed(int);
    void Signal_histogram_show_histogram(bool);
};

#endif // DIALOG_HISTOGRAM_H
