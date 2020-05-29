/*
 * Copyright (C) 2019-2020 Xavier Dechamps
*/

#ifndef DIALOG_PANORAMA_H
#define DIALOG_PANORAMA_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

class Dialog_Panorama: public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Panorama(QWidget *parent = nullptr);
    
public slots:
    void set_QLabel_number_images(int);
    void set_QLabel_string(QString);

private:
    QLabel* Panorama_value; // shows the number of pictures in the current panorama
    QLabel* Panorama_status; // return status from the stitching operation

private slots:
    void onClick_Pick_Up_Image();
    void onClick_Pop_Up_Image();
    void onClick_Update_Panorama();
    void onClick_Reset_Panorama();
    void onClick_Save_Panorama();

signals:
    void Signal_pick_up_image_panorama();
    void Signal_pop_up_image_panorama();
    void Signal_update_panorama();
    void Signal_reset_panorama();
    void Signal_save_panorama();
};

#endif // DIALOG_PANORAMA_H
