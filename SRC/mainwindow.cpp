/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *  The Qt Window that contains the opencv images and all the secondary windows for image treatment.
 *  Receives signals from secondary windows and sends signals to opencv for corresponding treatment.
*/

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), 
                                          worker(nullptr),
                                          menu_File(nullptr),
                                          menu_Filters(nullptr),
                                          currentImage(nullptr)
{
    // Get the number and name of the available cameras
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    
    int camID = cameras.size()-1;
    
    if (cameras.size() > 1) {
        QString info = QString("Available Cameras: \n");
        foreach (const QCameraInfo &cameraInfo, cameras) {
            info += " - " + cameraInfo.deviceName() + " : ";
            info += cameraInfo.description() + " : " ;
        }
        QMessageBox::information(this, "Cameras", info); 
    }
    
    // For capture thread
    this->data_lock = new QMutex();
    
    worker = new captureVideo(this,this->data_lock);
    
    // Links signals from the video capturer to functions that update the display
    connect(worker, &captureVideo::frameCaptured,     this, &MainWindow::updateFrame );
    connect(worker, &captureVideo::changeInfo   ,     this, &MainWindow::updateMainStatusLabel );
    connect(worker, &captureVideo::motionCaptured,    this, &MainWindow::update_motion_window );
    connect(worker, &captureVideo::objectsCaptured,   this, &MainWindow::update_objects_window );
    connect(worker, &captureVideo::histogramCaptured, this, &MainWindow::update_histogram_window );
#ifdef withstitching
    connect(worker, &captureVideo::panoramaCaptured,  this, &MainWindow::update_panorama_window );
#endif
    
    worker->setCamera(camID);
    worker->openCamera() ;
//    worker->setParent(this);
//    worker->setCascadeFile(); // call this one AFTER setparent() because worker needs a pointer to mainwindow
    
    this->worker->start(); // Launch the new thread (call to run() in capturevideo)
    
    this->resize(800, 600); 
    
    // Setup menubar
    this->menu_File            = menuBar()->addMenu(tr("&File"));
    this->menu_Filters         = menuBar()->addMenu(tr("Filters"));
    this->menu_Detection       = menuBar()->addMenu(tr("&Detection"));
    this->menu_Transformations = menuBar()->addMenu(tr("&Transformations"));
    this->menu_Operations      = menuBar()->addMenu(tr("&Operations"));
    
    this->imageScene = new QGraphicsScene(this);
    this->imageView = new QGraphicsView(imageScene);
    this->setCentralWidget(this->imageView);
    
    // Setup status bar
    this->mainStatusBar = statusBar();
    this->mainStatusLabel = new QLabel(this->mainStatusBar);
    this->mainStatusBar->addPermanentWidget(this->mainStatusLabel);
    this->mainStatusLabel->setText("Image Information will be here!");
    
    createActions();
    createToolBars();
    createWindows();

//    this->setFixedSize(740, 480);   
    
}

MainWindow::~MainWindow() {
    this->thread.quit(); // Nice way to tell the other thread to stop
    if(!this->thread.wait(3000)) //Wait until it actually has terminated (max. 3 sec)
    {
        this->thread.terminate(); //Thread didn't exit in time, probably deadlocked, terminate it!
        this->thread.wait(); //We have to wait again here!
    }
}

void MainWindow::createActions() {
    // Create actions for the buttons in toolbar
    // Show a tooltip when mouse hovers the buttons

    this->actionColour_BW = new QAction(tr("&Colour/BW"), this);
    this->actionColour_BW->setToolTip(tr("Switch between a coloured image and a black and white image"));
    this->actionColour_BW->setCheckable(true);
    connect(this->actionColour_BW, SIGNAL(triggered(bool)), this->worker, SLOT(toggleBW(bool) ) ) ;

    this->actionInverse = new QAction(tr("&Inverse"), this);
    this->actionInverse->setToolTip(tr("Inverse the image"));
    this->actionInverse->setCheckable(true);
    connect(this->actionInverse, SIGNAL(triggered(bool)), this->worker, SLOT(toggleInverse(bool) ) ) ;
    
    this->actionBlur = new QAction(tr("&Blur"), this);
    this->actionBlur->setToolTip(tr("Blur"));
    this->actionBlur->setCheckable(true);
    connect(this->actionBlur, SIGNAL(triggered(bool)), this->worker, SLOT(toggleBlur(bool)));
    connect(this->actionBlur, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Blur(bool)));    
            
    this->actionThreshold = new QAction(tr("&Threshold"), this);
    this->actionThreshold->setToolTip(tr("Threshold the image"));
    this->actionThreshold->setCheckable(true);
    connect(this->actionThreshold, SIGNAL(triggered(bool)), this->worker, SLOT(toggleThreshold(bool)));
    connect(this->actionThreshold, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Threshold(bool)));
    
    this->actionEdge = new QAction(tr("&Edge recognition"), this);
    this->actionEdge->setToolTip(tr("Find the edges in the image"));
    this->actionEdge->setCheckable(true);
    connect(this->actionEdge, SIGNAL(triggered(bool)), this->worker, SLOT(toggleEdge(bool)));
    connect(this->actionEdge, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Edge(bool)));
    
    this->actionMotionDetection = new QAction(tr("&Motion Detection"), this);
    this->actionMotionDetection->setToolTip(tr("Detect the motion in the webcam"));
    this->actionMotionDetection->setCheckable(true);
    connect(this->actionMotionDetection, SIGNAL(triggered(bool)), this->worker, SLOT(toggleMotionDetection(bool)));
    connect(this->actionMotionDetection, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Motion_Detection(bool)));    
    
    this->actionTransformation = new QAction(tr("&Transformation"), this);
    this->actionTransformation->setToolTip(tr("Apply geometric transformations to the image"));
    this->actionTransformation->setCheckable(true);
    connect(this->actionTransformation, SIGNAL(triggered(bool)), this->worker, SLOT(toggleTransformation(bool)));
    connect(this->actionTransformation, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Transformation(bool)));

#ifdef withobjdetect
    this->actionFace = new QAction(tr("&Face recognition"), this);
    this->actionFace->setToolTip(tr("Find human faces in the image"));
    this->actionFace->setCheckable(true);
    connect(this->actionFace, SIGNAL(triggered(bool)), this->worker, SLOT(toggleFaceDetection(bool)));
#endif

    this->actionObjectDetection = new QAction(tr("&Object detection"), this);
    this->actionObjectDetection->setToolTip(tr("Detect patterns in the image"));
    this->actionObjectDetection->setCheckable(true);
    connect(this->actionObjectDetection, SIGNAL(triggered(bool)), this->worker, SLOT(toggleObjectDetection(bool)));
    connect(this->actionObjectDetection, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Object_Detection(bool)));

    this->actionHistoEq = new QAction(tr("&Histogram equalization"), this);
    this->actionHistoEq->setToolTip(tr("Equalize the histogram of the image"));
    this->actionHistoEq->setCheckable(true);
    connect(this->actionHistoEq, SIGNAL(triggered(bool)), this->worker, SLOT(toggleHistogramEqualization(bool)));
    connect(this->actionHistoEq, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Histogram(bool)));
    
#ifdef withstitching
    this->actionPanorama = new QAction(tr("&Panorama"), this);
    this->actionPanorama->setToolTip(tr("Create a panorama from chosen images"));
    this->actionPanorama->setCheckable(true);
    connect(this->actionPanorama, SIGNAL(triggered(bool)), this->worker, SLOT(togglePanorama(bool)));
    connect(this->actionPanorama, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Panorama(bool)));
#endif

    this->actionPhoto = new QAction(tr("Module Photo"), this);
    this->actionPhoto->setToolTip(tr("OpenCV module Photo"));
    this->actionPhoto->setCheckable(true);
    connect(this->actionPhoto, SIGNAL(triggered(bool)), this->worker, SLOT(togglePhoto(bool)));
    connect(this->actionPhoto, SIGNAL(triggered(bool)), this,         SLOT(treat_Button_Photo(bool)));

    this->actionRecord = new QAction(tr("&Record"), this);
    this->actionRecord->setToolTip(tr("Record the video"));
    this->actionRecord->setCheckable(true);
    connect(this->actionRecord, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Record(bool)));
    
    this->actionSaveImage = new QAction(tr("&Save"), this );
    this->actionSaveImage->setToolTip(tr("Save the current image"));
//    this->actionSaveImage->setCheckable(true);
    connect(this->actionSaveImage, SIGNAL(triggered()), this->worker, SLOT(file_save_image()) );
    
#ifdef withzbar
    this->actionQRcode = new QAction(tr("&QR code"), this);
    this->actionQRcode->setToolTip(tr("Detect QR codes in the video"));
    this->actionQRcode->setCheckable(true);
    connect(this->actionQRcode, SIGNAL(triggered(bool)) , this->worker, SLOT(toggleQRcode(bool)) );
    connect(this->actionQRcode, SIGNAL(triggered(bool)) , this, SLOT(treat_Button_QRcode(bool)) );
#endif
}

void MainWindow::createToolBars() {
    // Build the toolbar with all the buttons to open the secondary windows for image treatment
//    this->editToolBar = new QToolBar(tr("&Edit"), this);
//    this->editToolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
//    this->editToolBar->setFixedWidth(100);
    
    this->menu_File->addAction(this->actionSaveImage);
    this->menu_File->addAction(this->actionRecord);
    
    this->menu_Filters->addAction(this->actionColour_BW);
    this->menu_Filters->addAction(this->actionInverse);
    this->menu_Filters->addAction(this->actionBlur);
    this->menu_Filters->addAction(this->actionThreshold);
    
    this->menu_Transformations->addAction(this->actionTransformation);
    this->menu_Transformations->addAction(this->actionHistoEq);
    
    this->menu_Detection->addAction(this->actionEdge);
    this->menu_Detection->addAction(this->actionMotionDetection);
#ifdef withobjdetect
    this->menu_Detection->addAction(this->actionFace);
#endif
    this->menu_Detection->addAction(this->actionObjectDetection);
#ifdef withzbar
    this->menu_Detection->addAction(this->actionQRcode);
#endif
    
#ifdef withstitching
    this->menu_Operations->addAction(this->actionPanorama);
#endif
    
    this->menu_Operations->addAction(this->actionPhoto);

//    this->editToolBar->addSeparator();
//    addToolBar(Qt::RightToolBarArea, this->editToolBar);
}

void MainWindow::createWindows(){
    // Create a new object for blur operations and create adequate connections to functions, depending on the received signals
    this->dialog_blur = new Dialog_Blur(this);
    connect(this->dialog_blur, SIGNAL(Signal_blur_range_changed(int)),   this->worker, SLOT(change_blur_range(int)) );
    connect(this->dialog_blur, SIGNAL(Signal_blur_method_changed(int)),  this->worker, SLOT(change_blur_method(int)) );
    connect(this->dialog_blur, SIGNAL(Signal_blur_element_changed(int)), this->worker, SLOT(change_blur_element(int)) );
    this->dialog_blur->hide();
    
    // Create a new object for threshold operations and create adequate connections to functions, depending on the received signals
    this->dialog_threshold = new Dialog_Threshold(this);
    connect(this->dialog_threshold, SIGNAL(Signal_threshold_method_changed(int)), this->worker, SLOT(change_threshold_method(int)) );
    connect(this->dialog_threshold, SIGNAL(Signal_threshold_value_changed(int)),  this->worker, SLOT(change_threshold_value(int)) );
    connect(this->dialog_threshold, SIGNAL(Signal_blocksize_changed(int)),        this->worker, SLOT(change_threshold_blocksize(int)) );
    connect(this->dialog_threshold, SIGNAL(Signal_adaptive_threshold_type(int)),  this->worker, SLOT(change_threshold_type(int)) );
    this->dialog_threshold->hide();

    // Create a new object for edge detection and create adequate connections to functions, depending on the received signals
    this->dialog_edge = new Dialog_Edge(this);
    connect(this->dialog_edge, SIGNAL(Signal_edge_method_changed(int)),        this->worker, SLOT(change_edge_method(int)) );
    connect(this->dialog_edge, SIGNAL(Signal_canny_lowthreshold_changed(int)), this->worker, SLOT(change_edge_canny_lowthreshold(int)) );
    connect(this->dialog_edge, SIGNAL(Signal_canny_ratio_changed(double)),     this->worker, SLOT(change_edge_canny_ratio(double)) );
    this->dialog_edge->hide();
    
    // Create a new object for motion detection and create adequate connections to functions, depending on the received signals
    this->dialog_motion_detection = new Dialog_Motion_Detection(this);
    connect(this->dialog_motion_detection, SIGNAL(Signal_motion_detection_method_changed(int)), this->worker, SLOT(change_motion_detection_method(int)) );
    this->dialog_motion_detection->hide();
    
    // Create a new object for object detections and create adequate connections to functions, depending on the received signals
    this->dialog_object_detection = new Dialog_Object_Detection(this);
    connect(this->dialog_object_detection, SIGNAL(Signal_object_detection_method_changed(int)), this->worker, SLOT(change_object_detection_method(int)) );
    connect(this->dialog_object_detection, SIGNAL(Signal_hough_line_threshold_changed(int)),    this->worker, SLOT(change_object_hough_line_threshold(int)) );
    this->dialog_object_detection->hide();
    
    // Create a new object for geometric transformations and create adequate connections to functions, depending on the received signals
    this->dialog_transformation = new Dialog_Transformation(this);
    connect(this->dialog_transformation, SIGNAL(Signal_transformation_method_changed(int)),   this->worker, SLOT(change_transformation_method(int)) );
    connect(this->dialog_transformation, SIGNAL(Signal_transformation_rotation_changed(int)), this->worker, SLOT(change_transformation_value_rotation(int)) );
    this->dialog_transformation->hide();

    // Create a new object for histogram operations and create adequate connections to functions, depending on the received signals
    this->dialog_histogram = new Dialog_Histogram(this);
    connect(dialog_histogram, SIGNAL(Signal_histogram_method_changed(int)),     this->worker, SLOT(change_histogram_method(int)) );
    connect(dialog_histogram, SIGNAL(Signal_histogram_clip_limit_changed(int)), this->worker, SLOT(change_histogram_clip_limit(int)) );
    connect(dialog_histogram, SIGNAL(Signal_histogram_tiles_changed(int)),      this->worker, SLOT(change_histogram_tiles(int)) );
    connect(dialog_histogram, SIGNAL(Signal_histogram_show_histogram(bool)),     this->worker, SLOT(change_histogram_show(bool)) );
    connect(dialog_histogram, SIGNAL(Signal_histogram_show_histogram(bool)),    this, SLOT(treat_Histogram_show_histogram(bool)) );
    this->dialog_histogram->hide();

#ifdef withstitching
    // Create a new object for stitching operations and create adequate connections to functions, depending on the received signals
    this->dialog_panorama = new Dialog_Panorama(this);
    connect(this->dialog_panorama,SIGNAL(Signal_pick_up_image_panorama()), this->worker, SLOT(panorama_pick_up_image()));
    connect(this->dialog_panorama,SIGNAL(Signal_pop_up_image_panorama()),  this->worker, SLOT(panorama_pop_out_image()));
    connect(this->dialog_panorama,SIGNAL(Signal_reset_panorama()),         this->worker, SLOT(panorama_reset()));
    connect(this->dialog_panorama,SIGNAL(Signal_update_panorama()),        this->worker, SLOT(panorama_update()));
    connect(this->dialog_panorama,SIGNAL(Signal_save_panorama()),          this->worker, SLOT(panorama_save()));
    
    connect(this->worker,SIGNAL(panoramaInfo(QString)),     this->dialog_panorama, SLOT(set_QLabel_string(QString)));
    connect(this->worker,SIGNAL(panoramaNumberImages(int)), this->dialog_panorama, SLOT(set_QLabel_number_images(int)));
    
    this->dialog_panorama->hide();
#endif

    // Create a new object for module Photo and create adequate connections to functions, depending on the received signals
    this->dialog_photo = new Dialog_Photo(this);
    connect(this->dialog_photo, SIGNAL(Signal_photo_method_changed(int)),   this->worker, SLOT(change_photo_method(int)) );
    connect(this->dialog_photo, SIGNAL(Signal_photo_sigmaS_changed(int)),   this->worker, SLOT(change_photo_sigmas(int)) );
    connect(this->dialog_photo, SIGNAL(Signal_photo_sigmaR_changed(double)),this->worker, SLOT(change_photo_sigmar(double)) );
    this->dialog_motion_detection->hide();
    
#ifdef withzbar
    this->qrdecoder_activated = false;
#endif
    
    // Create a new object for a secondary window that will show the histogram
    this->secondWindow = new SecondaryWindow(this);
    this->secondWindow->set_window_title("Histogram");
    this->secondWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->secondWindow->hide();
    this->histogram_window_opened = false;

    // Create a new object for a secondary window that will show the the detected objects
    this->thirdWindow = new SecondaryWindow(this);
    this->thirdWindow->set_window_title("Objects detection");
    this->thirdWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->thirdWindow->hide();
    this->object_detection_window_opened = false;

#ifdef withstitching
    // Create a new object for a secondary window that will show the panorama resulting from the stitching operation
    this->fourthWindow = new SecondaryWindow(this);
    this->fourthWindow->set_window_title("Panorama stitching");
    this->fourthWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->fourthWindow->hide();
    this->panorama_window_opened = false;
#endif

    // Create a new object for a secondary window that will show the motion detection
    this->fifthWindow = new SecondaryWindow(this);
    this->fifthWindow->set_window_title("Motion detection");
    this->fifthWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->fifthWindow->hide();
    this->motion_detection_window_opened = false;
}

void MainWindow::treat_Button_Blur(bool state) {
    if (state)
        this->dialog_blur->show();
    else 
        this->dialog_blur->hide();
}

void MainWindow::treat_Button_Threshold(bool state) {
    if (state)
        this->dialog_threshold->show();
    else 
        this->dialog_threshold->hide();
}

void MainWindow::treat_Button_Transformation(bool state) {
    if (state) 
        this->dialog_transformation->show();
    else 
        this->dialog_transformation->hide();
}

void MainWindow::treat_Button_Edge(bool state) {
    if (state) 
        this->dialog_edge->show();
    else 
        this->dialog_edge->hide();
}

void MainWindow::treat_Button_Histogram(bool state) {
    if (state) 
        this->dialog_histogram->show();
    else 
        this->dialog_histogram->hide();
}

void MainWindow::treat_Button_Object_Detection(bool state) {
    this->object_detection_window_opened = state;
    if (state) {
        this->dialog_object_detection->show();
        this->thirdWindow->show();
    }
    else {
        this->dialog_object_detection->hide();
        this->thirdWindow->hide();
    }
}

#ifdef withstitching
void MainWindow::treat_Button_Panorama(bool state) {
    this->panorama_window_opened = state;
    if (state) {
        this->dialog_panorama->show();
        this->fourthWindow->show();
    }
    else {
        this->dialog_panorama->hide();
        this->fourthWindow->hide();
    }
}
#endif

void MainWindow::treat_Button_Motion_Detection(bool state) {
    this->motion_detection_window_opened = state;
    if (state) {
        this->dialog_motion_detection->show();
        this->fifthWindow->show();
    }
    else {
        this->dialog_motion_detection->hide();
        this->fifthWindow->hide();
    }
}

void MainWindow::treat_Button_Photo(bool state) {
    if (state) 
        this->dialog_photo->show();
    else 
        this->dialog_photo->hide();
}

void MainWindow::treat_Button_Record(bool state) {
    if ( ! this->worker->file_save_movie(state) )
        // in the case there is a problem with saving the movie
        // (clicked on cancel in filename window)
        this->actionRecord->setChecked(false) ; 
}

#ifdef withzbar
void MainWindow::treat_Button_QRcode(bool state) {
    this->qrdecoder_activated = state;
}
#endif

void MainWindow::treat_Histogram_show_histogram(bool checked) {
    this->histogram_window_opened = checked;
    if (checked)
        this->secondWindow->show();
    else
        this->secondWindow->hide();
}

#ifdef withstitching
void MainWindow::update_panorama_window(QImage *image) {
    this->fourthWindow->set_image_content(*image);
}
#endif

void MainWindow::update_histogram_window(QImage *image) {
    if (this->histogram_window_opened)
        this->secondWindow->set_image_content(*image);
}

void MainWindow::update_objects_window(QImage *image) {
    if (this->object_detection_window_opened)
        this->thirdWindow->set_image_content(*image);
}

void MainWindow::update_motion_window(QImage *image) {
    if (this->motion_detection_window_opened)
        this->fifthWindow->set_image_content(*image);
}

#ifdef withzbar
// Look for data from QRcode decoded by ZBar. URLs and ISBN codes can be opened in an external web browser
void MainWindow::look_for_qrURL(){
    if (this->qrdecoder_activated){
        string qrdata,qrtype;
        if ( this->worker->getQRcodedata(qrdata,qrtype) ) {
            
            if( (qrdata.find("http://" ) == 0) 
                    || (qrdata.find("https://") == 0) 
                    || (qrdata.find("www.") == 0)   ) {
                // Standard URLs
                this->qrdecoder_activated = false;
                
                QMessageBox msgBox;
                msgBox.setText("Do you want to open the URL in an external window?");
                msgBox.setInformativeText( QString::fromStdString ( qrdata ) );
                msgBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
                msgBox.setDefaultButton( QMessageBox::Ok );
                int ret = msgBox.exec() ;
                
                if (ret == QMessageBox::Ok)
                    QDesktopServices::openUrl( QUrl( QString::fromStdString ( qrdata ) ) ) ;
            }
            
            else if ( qrtype.find("ISBN" ) == 0 ) {
                this->qrdecoder_activated = false;
                
                QMessageBox msgBox;
                msgBox.setText("Do you want to search for the ISBN in an external window?");
                msgBox.setInformativeText( QString::fromStdString ( qrtype ) + 
                                            " = " +
                                           QString::fromStdString ( qrdata ) );
                msgBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
                msgBox.setDefaultButton( QMessageBox::Ok );
                int ret = msgBox.exec() ;
                
                if (ret == QMessageBox::Ok) {
                    // Search the web for the given ISBN
                    QUrl myURL = "https://www.ecosia.org/search?q="+QString::fromStdString ( qrtype )+
                            "+"+QString::fromStdString ( qrdata );
                    QDesktopServices::openUrl( myURL ) ;
                }
            }
        }
    }
}
#endif

void MainWindow::updateFrame(QImage *image){
    data_lock->lock();
    this->myQimage = *image;
    data_lock->unlock();
    repaint();
}

void MainWindow::updateMainStatusLabel(QString newstring){
    this->mainStatusLabel->setText(newstring);
}

void MainWindow::paintEvent(QPaintEvent* ) {
    QPixmap piximage = QPixmap::fromImage(this->myQimage);

    imageScene->clear();
    imageView->resetMatrix();
    imageScene->addPixmap(piximage);
    imageScene->update();
    imageView->setSceneRect(piximage.rect());
    
#ifdef withzbar
    look_for_qrURL();
#endif
}
