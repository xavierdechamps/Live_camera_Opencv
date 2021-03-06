/*
 * Copyright (C) 2019-2020 Xavier Dechamps
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QImage>
#include <QPainter>
#include <QAction>
#ifdef withzbar
#include <QDesktopServices> // to open URL links from QR codes
#include <QUrl>             // to open URL links from QR codes
#endif
#include <QMessageBox>
#include <QCameraInfo> // From Qt5
#include <QMenuBar>
#include <QGraphicsScene> // To show the content of the camera in the Qt window
#include <QGraphicsView>  // To show the content of the camera in the Qt window
#include <QStatusBar>

#include <QMutex>
#include <QThread>

#include "dialog_choose_camera.h"
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
#ifdef withtesseract
#include "window_tesseract.h"
#endif

#include "capturevideo.h"

/**
 * @brief The MainWindow class
 */
class MainWindow: public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow() ;
    
private:
    // For capture thread
    captureVideo *worker;
//    QThread thread;
    QMutex *data_lock;
    
    QImage myQimage;
    
    QMenu *menu_File , *menu_Filters, *menu_Detection, *menu_Transformations, *menu_Operations;
    
    QGraphicsScene *imageScene;
    QGraphicsView *imageView;
    QGraphicsPixmapItem *currentImage;
    
    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;
    
    Dialog_choose_camera    *dialog_choose_camera;
    Dialog_Blur             *dialog_blur;
    Dialog_Edge             *dialog_edge;
    Dialog_Threshold        *dialog_threshold;
    Dialog_Transformation   *dialog_transformation;
    Dialog_Histogram        *dialog_histogram;
    Dialog_Object_Detection *dialog_object_detection;
#ifdef withstitching
    Dialog_Panorama         *dialog_panorama;
#endif
    Dialog_Motion_Detection *dialog_motion_detection;
    Dialog_Photo            *dialog_photo;
    
    SecondaryWindow *secondWindow;
    SecondaryWindow *thirdWindow;
    SecondaryWindow *fourthWindow;
    SecondaryWindow *fifthWindow;
#ifdef withtesseract
    Window_Tesseract *window_tesseract;
#endif
    
#ifdef withzbar
    bool qrdecoder_activated;
#endif
    
    bool histogram_window_opened;
    bool object_detection_window_opened;
#ifdef withstitching
    bool panorama_window_opened;
#endif
#ifdef withtesseract
    bool tesseract_window_opened;
#endif
    bool motion_detection_window_opened;

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
    QAction *actionChangeCamera;
#ifdef withzbar
    QAction *actionQRcode;
#endif
#ifdef withtesseract
    QAction *action_Tesseract;
#endif

    void update_histogram_window(QImage *image);
    void update_objects_window(QImage *image);
    void update_motion_window(QImage *image);
    void createActions();
    void createToolBars();
    void createWindows();
    void stopThread();
#ifdef withzbar
    void look_for_qrURL();
#endif

protected:
    void paintEvent(QPaintEvent* );
    void closeEvent(QCloseEvent * event);

private slots:
    void launchCamera(int);
    void showCameraSettings();
    void updateFrame(QImage *image);
    void updateMainStatusLabel(QString);

    void treat_Button_Blur(bool) ;
    void treat_Button_Edge(bool);
    void treat_Button_Threshold(bool);
    void treat_Button_Transformation(bool);
    void treat_Button_Histogram(bool);
    void treat_Button_Object_Detection(bool);
#ifdef withstitching
    void treat_Button_Panorama(bool);
    void update_panorama_window(QImage *image);
#endif
    void treat_Button_Motion_Detection(bool);
    void treat_Button_Photo(bool);
    void treat_Button_Record(bool);
#ifdef withzbar
    void treat_Button_QRcode(bool);
#endif
    void treat_Histogram_show_histogram(bool);
#ifdef withtesseract
    void treat_Button_Tesseract(bool);
    void treat_Tesseract_get_new_image(bool);
#endif // endif withtesseract
};

#endif // MAINWINDOW_H
