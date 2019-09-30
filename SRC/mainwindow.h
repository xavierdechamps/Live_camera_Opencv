/*
 * Copyright: Xavier Dechamps
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QAction>
#include <QToolBar>
#include <QFileDialog>

#include "dialog_blur.h"
#include "dialog_edge.h"
#include "dialog_threshold.h"
#include "dialog_transformation.h"
#include "dialog_histogram.h"
#include "dialog_object_detection.h"
#include "dialog_panorama.h"
#include "dialog_motion_detection.h"
#include "secondarywindow.h"
#include "myimage.h"

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

class MainWindow: public QMainWindow
{
public:
    explicit MainWindow(QWidget * parent = 0);

private:
    Q_OBJECT
    MyImage* myFrame;
    QImage myQimage,mySecondQimage,myThirdImage,myFourthImage,myFifthImage;
    VideoCapture capture;
    QLabel *Window_image;
    QTimer my_Timer;
    Dialog_Blur* dialog_blur;
    Dialog_Edge* dialog_edge;
    Dialog_Threshold* dialog_threshold;
    Dialog_Transformation* dialog_transformation;
    Dialog_Histogram* dialog_histogram;
    Dialog_Object_Detection* dialog_object_detection;
    Dialog_Panorama* dialog_panorama;
    Dialog_Motion_Detection* dialog_motion_detection;
    SecondaryWindow* secondWindow;
    SecondaryWindow* thirdWindow;
    SecondaryWindow* fourthWindow;
    SecondaryWindow* fifthWindow;

    String file_name_save, main_directory;

    QAction *actionColour_BW;
    QAction *actionInverse;
    QAction *actionBlur;
    QAction *actionThreshold;
    QAction *actionTransformation;
    QAction *actionEdge;
    QAction *actionFace;
    QAction *actionHistoEq;
    QAction *actionObjectDetection;
    QAction *actionPanorama;
    QAction *actionMotionDetection;
    QAction *actionRecord;
    QToolBar *editToolBar;

    bool histogram_window_opened;
    bool object_detection_window_opened;
    bool panorama_window_opened;
    bool motion_detection_window_opened;

    VideoWriter video_out;
    String video_out_name;
    bool recording;
    int record_time_blink;

    void update_frame();
    void update_histogram_window();
    void update_objects_window();
    void update_motion_window();
    void createActions();
    void createToolBars();

protected:
    void paintEvent(QPaintEvent* );

private slots:
    void treat_Button_BW() ;
    void treat_Button_Inverse() ;
    void treat_Button_Blur(bool) ;
    void treat_Button_Edge(bool) ;
    void treat_Button_Threshold(bool);
    void treat_Button_Transformation(bool);
    void treat_Button_Face_Recon() ;
    void treat_Button_Histogram(bool);
    void treat_Button_Object_Detection(bool);
    void treat_Button_Panorama(bool);
    void treat_Button_Motion_Detection(bool);
    void treat_Button_Record(bool);

    void treat_Slider_Blur_Range(int);
    void treat_Blur_Method(int);
    void treat_Slider_Blur_Element(int);

    void treat_Slider_Threshold_Value(int);
    void treat_Slider_Threshold_Blocksize_Value(int);
    void treat_Threshold_Method(int);
    void treat_Threshold_Type(int);

    void treat_Transformation_Method(int);
    void treat_Slider_Transformation_Rotation_Value(int);

    void treat_Edge_Method(int);
    void treat_Slider_canny_lowthreshold(int);
    void treat_Slider_canny_ratio(double);

    void treat_Histogram_method(int);
    void treat_Histogram_tiles(int);
    void treat_Histogram_clip_limit(int);
    void treat_Histogram_show_histogram(bool);

    void treat_Object_Detection_Method(int);
    void treat_Slider_hough_line_threshold(int);

    void treat_Panorama_Pick_Up_Image();
    void treat_Panorama_Pop_Up_Image();
    void treat_Panorama_Update();
    void treat_Panorama_Reset();
    void treat_Panorama_Save();

    void treat_Motion_Detection_Method(int);
};

#endif // MAINWINDOW_H
