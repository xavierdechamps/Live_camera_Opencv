#ifndef CAPTUREVIDEO_H
#define CAPTUREVIDEO_H

// Qt things here
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutex>

#include <QImage>
#include <QFileDialog>

#include <QMainWindow>

// MyImage
#include "myimage.h"

// OpenCV
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

class captureVideo : public QThread
{
    Q_OBJECT
public:
    explicit captureVideo(QMutex* data_lock=nullptr);
    void setCamera(int camID);
    bool openCamera();
    bool closeCamera();
    bool cameraIsOpen();
    void setParent(QMainWindow *parent);
    void setCascadeFile();
    
signals:
    void frameCaptured(QImage *image);
    void changeInfo(QString label);
    
public slots:
    void onTimeOut();
    void toggleBW(bool);
    void toggleInverse(bool);
    void toggleBlur(bool);
    
    void change_blur_range(int);
    void change_blur_method(int);
    void change_blur_element(int);
    
protected:
    void run() override; 
    
private:
    // Private variables
    MyImage* myFrame;
    int camID,record_time_blink;
    VideoCapture capture;
    VideoWriter video_out;
    String file_name_save, main_directory, file_background, file_cascade,video_out_name;
    bool recording,running;
    QImage myQimage;
    QMainWindow *mainWindowParent;
    QMutex *data_lock;
    // Private functions
};

#endif // CAPTUREVIDEO_H
