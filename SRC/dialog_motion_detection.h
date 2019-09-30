/*
 * Copyright: Xavier Dechamps
*/

#ifndef DIALOG_MOTION_DETECTION_H
#define DIALOG_MOTION_DETECTION_H

#include <QDialog>
#include <QGridLayout>
#include <QButtonGroup>
#include <QRadioButton>

#include <iostream>

class Dialog_Motion_Detection : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Motion_Detection(QWidget *parent = 0);

private:
    QButtonGroup* RadioButtons;

private slots:
    void onClick_Radio_Motion_Method();

signals:
    void Signal_motion_detection_method_changed(int);

};

#endif // DIALOG_MOTION_DETECTION_H
