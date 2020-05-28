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
    explicit captureVideo(QMainWindow *parent=nullptr,QMutex* data_lock=nullptr);
    void setCamera(int camID);
    bool openCamera();
    bool closeCamera();
    bool cameraIsOpen();
    void setParent(QMainWindow *parent);
    void setCascadeFile();
#ifdef withzbar
    bool getQRcodedata(string &, string &);
#endif
    
signals:
    void frameCaptured(QImage *image);
    void motionCaptured(QImage *image);
    void objectsCaptured(QImage *image);
    void changeInfo(QString label);
    
public slots:
    void toggleBW(bool);
    void toggleInverse(bool);
    void toggleBlur(bool);
    void toggleThreshold(bool);
    void toggleEdge(bool);
    void toggleMotionDetection(bool);
#ifdef withobjdetect
    void toggleFaceDetection(bool);
#endif
    void toggleObjectDetection(bool);
#ifdef withzbar
    void toggleQRcode(bool);
#endif
    
    void change_blur_range(int);
    void change_blur_method(int);
    void change_blur_element(int);
    
    void change_threshold_value(int);
    void change_threshold_method(int);
    void change_threshold_blocksize(int);
    void change_threshold_type(int);
    
    void change_edge_method(int);
    void change_edge_canny_lowthreshold(int);
    void change_edge_canny_ratio(double);
    
    void change_motion_detection_method(int);
    
    void change_object_detection_method(int);
    void change_object_hough_line_threshold(int);
    
protected:
    void run() override; 
    
private:
    // Private variables
    MyImage* myFrame;
    int camID,record_time_blink;
    VideoCapture capture;
    VideoWriter video_out;
    String file_name_save, main_directory, file_background, file_cascade,video_out_name;
    bool recording,running,motion_active,objects_active,qrdecoder_active;
    QImage myQimage,motionQimage,objectsQimage;
    QMainWindow *mainWindowParent;
    QMutex *data_lock;
    // Private functions
};

#endif // CAPTUREVIDEO_H
