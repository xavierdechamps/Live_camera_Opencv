/*
 * Copyright (C) 2019-2020 Xavier Dechamps
*/

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
    void setThreadStatus(bool);
    void setParent(QMainWindow *parent);
    void setCascadeFile();
#ifdef withzbar
    bool getQRcodedata(std::string &, std::string &);
#endif
    bool file_save_movie(bool);
    
signals:
    void frameCaptured(QImage *image);
    void motionCaptured(QImage *image);
    void objectsCaptured(QImage *image);
    void histogramCaptured(QImage *image);
    void panoramaCaptured(QImage *image);
    void changeInfo(QString label);
    void panoramaInfo(QString label);
    void panoramaNumberImages(int);
    
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
    void toggleTransformation(bool);
    void toggleHistogramEqualization(bool);
#ifdef withstitching
    void togglePanorama(bool);
#endif
    void togglePhoto(bool);
    
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
    
    void change_transformation_method(int);
    void change_transformation_value_rotation(int);
    
    void change_histogram_method(int);
    void change_histogram_tiles(int);
    void change_histogram_clip_limit(int);
    void change_histogram_show(bool);
    
    void change_photo_method(int);
    void change_photo_sigmas(int);
    void change_photo_sigmar(double);
    
#ifdef withstitching
    void panorama_pick_up_image();
    void panorama_pop_out_image();
    void panorama_reset();
    void panorama_update();
    void panorama_save();
#endif
    
    void file_save_image();
    
    
protected:
    void run() override; 
    
private:
    // Private variables
    MyImage* myFrame;
    cv::Mat currentMat;
    int camID,record_time_blink;
    cv::VideoCapture capture;
    cv::VideoWriter video_out;
    cv::String file_name_save, main_directory, file_background, file_cascade,video_out_name;
    bool recording,running,motion_active,objects_active,qrdecoder_active;
    bool histo_active,panorama_active;
    QImage myQimage,motionQimage,objectsQimage,histoQimage,panorama_Qimage;
    QMainWindow *mainWindowParent;
    QMutex *data_lock;
    
    // Private functions
#ifdef withface
    void loadOrnaments();
#endif
};

#endif // CAPTUREVIDEO_H
