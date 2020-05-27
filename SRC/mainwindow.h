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
#include <QFileDialog>
#ifdef withzbar
#include <QDesktopServices> // to open URL links from QR codes
#include <QUrl>             // to open URL links from QR codes
#include <QMessageBox>
#endif
#include <QCameraInfo> // From Qt5
#include <QMenuBar>
#include <QGraphicsScene> // To show the content of the camera in the Qt window
#include <QGraphicsView>  // To show the content of the camera in the Qt window
#include <QGraphicsPixmapItem>
#include <QStatusBar>

#include <QMutex>
#include <QThread>



#include "dialog_blur.h"
#include "dialog_edge.h"
#include "dialog_threshold.h"
#include "dialog_transformation.h"
#include "dialog_histogram.h"
#include "dialog_object_detection.h"
#ifdef withstitching
#include "dialog_panorama.h"
#endif
#include "dialog_motion_detection.h"
#include "dialog_photo.h"
#include "secondarywindow.h"

#include "myimage.h"

#include "capturevideo.h"
//#include "capturethread.h"

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

class MainWindow: public QMainWindow
{
public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow() ;
    
private:
    Q_OBJECT
    
    // For capture thread
    captureVideo *worker;
    QThread thread;
    
//    CaptureThread *capturer;
    QMutex *data_lock;
    
    MyImage* myFrame;
    QImage myQimage,mySecondQimage,myThirdImage,myFourthImage,myFifthImage;
    VideoCapture capture;

    QLabel *Window_image;
    
    QTimer my_Timer , thread_timer;
    QMenu *menu_File , *menu_Filters, *menu_Detection, *menu_Transformations, *menu_Operations;
    
    QGraphicsScene *imageScene;
    QGraphicsView *imageView;
    QGraphicsPixmapItem *currentImage;
    
    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;
    
    Dialog_Blur* dialog_blur;
    Dialog_Edge* dialog_edge;
    Dialog_Threshold* dialog_threshold;
    Dialog_Transformation* dialog_transformation;
    Dialog_Histogram* dialog_histogram;
    Dialog_Object_Detection* dialog_object_detection;
#ifdef withstitching
    Dialog_Panorama* dialog_panorama;
#endif
    Dialog_Motion_Detection* dialog_motion_detection;
    Dialog_Photo* dialog_photo;
    SecondaryWindow* secondWindow;
    SecondaryWindow* thirdWindow;
    SecondaryWindow* fourthWindow;
    SecondaryWindow* fifthWindow;

    String file_name_save, main_directory, file_background, file_cascade;
#ifdef withzbar
    bool qrdecoder_activated;
#endif
    
    bool histogram_window_opened;
    bool object_detection_window_opened;
#ifdef withstitching
    bool panorama_window_opened;
#endif
    bool motion_detection_window_opened;
    
    VideoWriter video_out;
    String video_out_name;
    bool recording;
    int record_time_blink;

    // List of QActions
    QAction *actionColour_BW;
    QAction *actionInverse;
    QAction *actionBlur;
    QAction *actionThreshold;
    QAction *actionTransformation;
    QAction *actionEdge;
#ifdef withobjdetect
    QAction *actionFace;
#endif
    QAction *actionHistoEq;
    QAction *actionObjectDetection;
#ifdef withstitching
    QAction *actionPanorama;
#endif
    QAction *actionMotionDetection;
    QAction *actionPhoto;
    QAction *actionRecord;
    QAction *actionSaveImage;
#ifdef withzbar
    QAction *actionQRcode;
#endif

    void update_histogram_window();
    void update_objects_window();
    void update_motion_window();
    void createActions();
    void createToolBars();
    void createWindows();
#ifdef withzbar
    void look_for_qrURL();
#endif

protected:
    void paintEvent(QPaintEvent* );

private slots:
    void updateFrame(QImage *image);
    void updateMainStatusLabel(QString);

    void treat_Button_Blur(bool) ;
    void treat_Button_Edge(bool);
    void treat_Button_Threshold(bool);
    void treat_Button_Transformation(bool);
#ifdef withobjdetect
    void treat_Button_Face_Recon(bool) ;
#endif
    void treat_Button_Histogram(bool);
    void treat_Button_Object_Detection(bool);
#ifdef withstitching
    void treat_Button_Panorama(bool);
#endif
    void treat_Button_Motion_Detection(bool);
    void treat_Button_Photo(bool);
    void treat_Button_Record(bool);
    void treat_Button_Save();
#ifdef withzbar
    void treat_Button_QRcode(bool);
#endif

//    void treat_Slider_Blur_Range(int);
//    void treat_Blur_Method(int);
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

#ifdef withstitching
    void treat_Panorama_Pick_Up_Image();
    void treat_Panorama_Pop_Up_Image();
    void treat_Panorama_Update();
    void treat_Panorama_Reset();
    void treat_Panorama_Save();
#endif

    void treat_Motion_Detection_Method(int);
    
    void treat_Photo_Method(int);
    void treat_Photo_SigmaS(int value);
    void treat_Photo_SigmaR(double value);
};

#endif // MAINWINDOW_H
