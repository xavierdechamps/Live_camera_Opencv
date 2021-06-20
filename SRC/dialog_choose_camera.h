#ifndef DIALOG_CHOOSE_CAMERA_H
#define DIALOG_CHOOSE_CAMERA_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QCameraInfo>
#include <QDebug>

class Dialog_choose_camera : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog_choose_camera(QList<QCameraInfo> cameras,QWidget *parent = nullptr);
    QString get_info_camera() ;
    
private:
    QString info_camera ;
    QList<QCameraInfo> list_cameras ;

private slots:
    void onClick_Choose_Camera(int);
    
signals:
    void Signal_camera_chosen(int value);
};

#endif // DIALOG_CHOOSE_CAMERA_H
