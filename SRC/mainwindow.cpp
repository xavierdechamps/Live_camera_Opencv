/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *  The Qt Window that contains the opencv images and all the secondary windows for image treatment.
 *  Receives signals from secondary windows and sends signals to opencv for corresponding treatment.
*/

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), 
                                          my_Timer(this),
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
    data_lock = new QMutex();
    capturer = new CaptureThread(camID, data_lock);
//    capturer->start();
    
    this->resize(800, 600); 
    
    // Setup menubar
    this->menu_File      = menuBar()->addMenu(tr("&File"));
    this->menu_Filters   = menuBar()->addMenu(tr("Filters"));
    this->menu_Detection = menuBar()->addMenu(tr("&Detection"));
    this->menu_Transformations = menuBar()->addMenu(tr("&Transformations"));
    this->menu_Operations = menuBar()->addMenu(tr("&Operations"));
    
    // QLabel that will actually show the video, conversion from QImage with QPixmap::fromImage
    this->Window_image = new QLabel(this);
    this->setCentralWidget(this->Window_image);
    
    // Setup status bar
    this->mainStatusBar = statusBar();
    this->mainStatusLabel = new QLabel(this->mainStatusBar);
    this->mainStatusBar->addPermanentWidget(this->mainStatusLabel);
    this->mainStatusLabel->setText("Image Information will be here!");
    
    // Initializations
    this->myFrame = new MyImage();
    this->main_directory = "/Users/dechamps/Documents/Codes/Cpp/Images/";

#ifdef withobjdetect
    this->file_cascade = this->main_directory + "Libraries/opencv-4.3.0/install/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
    bool testCascade = this->myFrame->set_Face_Cascade_Name(this->file_cascade);
    while (!testCascade){
        QString QfileNameLocal = QFileDialog::getOpenFileName(this,
                                                         tr("Select the face cascade file haarcascade_frontalface_default.xml"),
                                                         QString::fromStdString(this->main_directory),
                                                         tr("Images (*.xml)") );
        if ( ! QfileNameLocal.isEmpty() ) {
            this->file_cascade = QfileNameLocal.toStdString() ;
            testCascade = this->myFrame->set_Face_Cascade_Name(this->file_cascade);
        }
        else {
            this->mainStatusLabel->setText("Could not load the face cascade file");
        }
    }
#endif

    this->file_name_save = this->main_directory + "webcam.jpg";
    this->capture.open(camID);

    // Main area for image display // pretty slow... -> much faster with the QLabel
/*    this->imageScene = new QGraphicsScene(this);
    this->imageView = new QGraphicsView(this->imageScene);
    this->setCentralWidget(this->imageView);
*/
    
    createActions();
    createToolBars();
    createWindows();

    // Concerns the recording of the video
    this->record_time_blink = 0;
    this->recording = false;
    this->video_out_name = this->main_directory + "Video-OpenCV-QMake/my_video.avi";

//    this->setFixedSize(740, 480);   

    // Update the image every 10ms
    this->my_Timer.start(10);
    connect(&this->my_Timer, SIGNAL(timeout()), this, SLOT(repaint()));
}

void MainWindow::update_frame() {
    // Called by paint event every 10ms
    // Gets the content of the camera and push the image to the opencv container
    // Apply the transformations that are activated
    // Record the video if the option is activated

    Mat imageMat;

    if(capture.isOpened()) {

        capture >> imageMat;
        while (imageMat.empty()) {
            capture >> imageMat;
            cout << "empty image\n";
        }

        // Send the frame to the class MyImage for post-processing
        myFrame->set_image_content(imageMat);

        // Get the image after post-processing
        imageMat = myFrame->get_image_content();

        // Record the video
        if (this->recording) {
            this->video_out << imageMat; // save the image before the colour conversion
            if (this->record_time_blink <= 20) // Display a blinking red circle
                cv::circle(imageMat,cv::Point(20,20),15, Scalar(0,0,255), -1, 8);
            this->record_time_blink ++;
            if (this->record_time_blink >= 40)
                this->record_time_blink = 0;
        }

        // Convert the opencv image to a QImage that will be displayed on the main window
        cvtColor(imageMat, imageMat, COLOR_BGR2RGB);
        this->myQimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    }
    else
    {
        cout << "No capture" << endl;
        imageMat = Mat::zeros(480, 640, CV_8UC1);
        myFrame->set_image_content(imageMat);
        imageMat = myFrame->get_image_content();

        cvtColor(imageMat, imageMat, COLOR_BGR2RGB);
        this->myQimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    }
}

void MainWindow::createActions() {
    // Create actions for the buttons in toolbar
    // Show a tooltip when mouse hovers the buttons

    this->actionColour_BW = new QAction(tr("&Colour/BW"), this);
    this->actionColour_BW->setToolTip(tr("Switch between a coloured image and a black and white image"));
    this->actionColour_BW->setCheckable(true);
    connect(this->actionColour_BW, SIGNAL(triggered(bool)), this, SLOT(treat_Button_BW(bool)));

    this->actionInverse = new QAction(tr("&Inverse"), this);
    this->actionInverse->setToolTip(tr("Inverse the image"));
    this->actionInverse->setCheckable(true);
    connect(this->actionInverse, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Inverse(bool)));

    this->actionBlur = new QAction(tr("&Blur"), this);
    this->actionBlur->setToolTip(tr("Blur"));
    this->actionBlur->setCheckable(true);
    connect(this->actionBlur, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Blur(bool)));

    this->actionThreshold = new QAction(tr("&Threshold"), this);
    this->actionThreshold->setToolTip(tr("Threshold the image"));
    this->actionThreshold->setCheckable(true);
    connect(this->actionThreshold, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Threshold(bool)));

    this->actionTransformation = new QAction(tr("&Transformation"), this);
    this->actionTransformation->setToolTip(tr("Apply geometric transformations to the image"));
    this->actionTransformation->setCheckable(true);
    connect(this->actionTransformation, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Transformation(bool)));

    this->actionEdge = new QAction(tr("&Edge recognition"), this);
    this->actionEdge->setToolTip(tr("Find the edges in the image"));
    this->actionEdge->setCheckable(true);
    connect(this->actionEdge, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Edge(bool)));

#ifdef withobjdetect
    this->actionFace = new QAction(tr("&Face recognition"), this);
    this->actionFace->setToolTip(tr("Find human faces in the image"));
    this->actionFace->setCheckable(true);
    connect(this->actionFace, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Face_Recon(bool)));
#endif

    this->actionHistoEq = new QAction(tr("&Histogram equalization"), this);
    this->actionHistoEq->setToolTip(tr("Equalize the histogram of the image"));
    this->actionHistoEq->setCheckable(true);
    connect(this->actionHistoEq, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Histogram(bool)));

    this->actionObjectDetection = new QAction(tr("&Object detection"), this);
    this->actionObjectDetection->setToolTip(tr("Detect patterns in the image"));
    this->actionObjectDetection->setCheckable(true);
    connect(this->actionObjectDetection, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Object_Detection(bool)));

#ifdef withstitching
    this->actionPanorama = new QAction(tr("&Panorama"), this);
    this->actionPanorama->setToolTip(tr("Create a panorama from chosen images"));
    this->actionPanorama->setCheckable(true);
    connect(this->actionPanorama, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Panorama(bool)));
#endif

    this->actionMotionDetection = new QAction(tr("&Motion Detection"), this);
    this->actionMotionDetection->setToolTip(tr("Detect the motion in the webcam"));
    this->actionMotionDetection->setCheckable(true);
    connect(this->actionMotionDetection, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Motion_Detection(bool)));
    
    this->actionPhoto = new QAction(tr("Module Photo"), this);
    this->actionPhoto->setToolTip(tr("OpenCV module Photo"));
    this->actionPhoto->setCheckable(true);
    connect(this->actionPhoto, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Photo(bool)));

    this->actionRecord = new QAction(tr("&Record"), this);
    this->actionRecord->setToolTip(tr("Record the video"));
    this->actionRecord->setCheckable(true);
    connect(this->actionRecord, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Record(bool)));
    
    this->actionSaveImage = new QAction(tr("&Save"), this );
    this->actionSaveImage->setToolTip(tr("Save the current image"));
//    this->actionSaveImage->setCheckable(true);
    connect(this->actionSaveImage, SIGNAL(triggered()), this, SLOT(treat_Button_Save()) );
    
#ifdef withzbar
    this->actionQRcode = new QAction(tr("&QR code"), this);
    this->actionQRcode->setToolTip(tr("Detect QR codes in the video"));
    this->actionQRcode->setCheckable(true);
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
    connect(this->dialog_blur, SIGNAL(Signal_blur_range_changed(int)), this, SLOT(treat_Slider_Blur_Range(int)) );
    connect(this->dialog_blur, SIGNAL(Signal_blur_method_changed(int)),this, SLOT(treat_Blur_Method(int)) );
    connect(this->dialog_blur, SIGNAL(Signal_blur_element_changed(int)), this, SLOT(treat_Slider_Blur_Element(int)) );
    this->dialog_blur->hide();

    // Create a new object for edge detection and create adequate connections to functions, depending on the received signals
    this->dialog_edge = new Dialog_Edge(this);
    connect(this->dialog_edge, SIGNAL(Signal_edge_method_changed(int)),this, SLOT(treat_Edge_Method(int)) );
    connect(this->dialog_edge, SIGNAL(Signal_canny_lowthreshold_changed(int)), this, SLOT(treat_Slider_canny_lowthreshold(int)) );
    connect(this->dialog_edge, SIGNAL(Signal_canny_ratio_changed(double)), this, SLOT(treat_Slider_canny_ratio(double)) );
    this->dialog_edge->hide();

    // Create a new object for threshold operations and create adequate connections to functions, depending on the received signals
    this->dialog_threshold = new Dialog_Threshold(this);
    connect(this->dialog_threshold, SIGNAL(Signal_threshold_method_changed(int)), this, SLOT(treat_Threshold_Method(int)) );
    connect(this->dialog_threshold, SIGNAL(Signal_threshold_value_changed(int)), this, SLOT(treat_Slider_Threshold_Value(int)) );
    connect(this->dialog_threshold, SIGNAL(Signal_blocksize_changed(int)), this, SLOT(treat_Slider_Threshold_Blocksize_Value(int)) );
    connect(this->dialog_threshold, SIGNAL(Signal_adaptive_threshold_type(int)), this, SLOT(treat_Threshold_Type(int)) );
    this->dialog_threshold->hide();

    // Create a new object for geometric transformations and create adequate connections to functions, depending on the received signals
    this->dialog_transformation = new Dialog_Transformation(this);
    connect(this->dialog_transformation, SIGNAL(Signal_transformation_method_changed(int)), this, SLOT(treat_Transformation_Method(int)) );
    connect(this->dialog_transformation, SIGNAL(Signal_transformation_rotation_changed(int)), this, SLOT(treat_Slider_Transformation_Rotation_Value(int)) );
    this->dialog_transformation->hide();

    // Create a new object for histogram operations and create adequate connections to functions, depending on the received signals
    this->dialog_histogram = new Dialog_Histogram(this);
    connect(dialog_histogram, SIGNAL(Signal_histogram_method_changed(int)), this, SLOT(treat_Histogram_method(int)) );
    connect(dialog_histogram, SIGNAL(Signal_histogram_clip_limit_changed(int)), this, SLOT(treat_Histogram_clip_limit(int)) );
    connect(dialog_histogram, SIGNAL(Signal_histogram_tiles_changed(int)), this, SLOT(treat_Histogram_tiles(int)) );
    connect(dialog_histogram, SIGNAL(Signal_histogram_show_histogram(bool)), this, SLOT(treat_Histogram_show_histogram(bool)) );
    this->dialog_histogram->hide();

    // Create a new object for object detections and create adequate connections to functions, depending on the received signals
    this->dialog_object_detection = new Dialog_Object_Detection(this);
    connect(this->dialog_object_detection, SIGNAL(Signal_object_detection_method_changed(int)), this, SLOT(treat_Object_Detection_Method(int)) );
    connect(this->dialog_object_detection, SIGNAL(Signal_hough_line_threshold_changed(int)), this, SLOT(treat_Slider_hough_line_threshold(int)) );
    this->dialog_object_detection->hide();

#ifdef withstitching
    // Create a new object for stitching operations and create adequate connections to functions, depending on the received signals
    this->dialog_panorama = new Dialog_Panorama(this);
    connect(this->dialog_panorama,SIGNAL(Signal_pick_up_image_panorama()), this, SLOT(treat_Panorama_Pick_Up_Image()));
    connect(this->dialog_panorama,SIGNAL(Signal_pop_up_image_panorama()), this, SLOT(treat_Panorama_Pop_Up_Image()));
    connect(this->dialog_panorama,SIGNAL(Signal_update_panorama()), this, SLOT(treat_Panorama_Update()));
    connect(this->dialog_panorama,SIGNAL(Signal_reset_panorama()), this, SLOT(treat_Panorama_Reset()));
    connect(this->dialog_panorama,SIGNAL(Signal_save_panorama()), this, SLOT(treat_Panorama_Save()));
    this->dialog_panorama->hide();
#endif

    // Create a new object for motion detection and create adequate connections to functions, depending on the received signals
    this->dialog_motion_detection = new Dialog_Motion_Detection(this);
    connect(this->dialog_motion_detection, SIGNAL(Signal_motion_detection_method_changed(int)), this, SLOT(treat_Motion_Detection_Method(int)) );
    this->dialog_motion_detection->hide();
    
    // Create a new object for module Photo and create adequate connections to functions, depending on the received signals
    this->dialog_photo = new Dialog_Photo(this);
    connect(this->dialog_photo, SIGNAL(Signal_photo_method_changed(int)), this, SLOT(treat_Photo_Method(int)) );
    connect(this->dialog_photo, SIGNAL(Signal_photo_sigmaS_changed(int)),this, SLOT(treat_Photo_SigmaS(int)) );
    connect(this->dialog_photo, SIGNAL(Signal_photo_sigmaR_changed(double)),this, SLOT(treat_Photo_SigmaR(double)) );
    this->dialog_motion_detection->hide();
    
#ifdef withzbar
    this->qrdecoder_activated = false;
#endif
    
    // Create a new object for a secondary window that will show the histogram
    this->secondWindow = new SecondaryWindow(this);
    this->secondWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->secondWindow->hide();
    this->histogram_window_opened = false;

    // Create a new object for a secondary window that will show the the detected objects
    this->thirdWindow = new SecondaryWindow(this);
    this->thirdWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->thirdWindow->hide();
    this->object_detection_window_opened = false;

#ifdef withstitching
    // Create a new object for a secondary window that will show the panorama resulting from the stitching operation
    this->fourthWindow = new SecondaryWindow(this);
    this->fourthWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->fourthWindow->hide();
    this->panorama_window_opened = false;
#endif

    // Create a new object for a secondary window that will show the motion detection
    this->fifthWindow = new SecondaryWindow(this);
    this->fifthWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->fifthWindow->hide();
    this->motion_detection_window_opened = false;
}

void MainWindow::treat_Button_BW(bool state) {
    this->myFrame->toggleBW() ;
    if (state)
        this->mainStatusLabel->setText("Black and White activated");
    else
        this->mainStatusLabel->setText("Black and White desactivated");
}

void MainWindow::treat_Button_Inverse(bool state) {
    this->myFrame->toggleInverse() ;
    if (state)
        this->mainStatusLabel->setText("Inversed colours activated");
    else
        this->mainStatusLabel->setText("Inversed colours desactivated");
}

void MainWindow::treat_Button_Blur(bool state) {
    this->myFrame->toggleBlur();
    if (state){
        this->dialog_blur->show();
        this->mainStatusLabel->setText("Blur filter activated");
    }
    else {
        this->dialog_blur->hide();
        this->mainStatusLabel->setText("Blur filter desactivated");
    }
}

void MainWindow::treat_Button_Threshold(bool state) {
    this->myFrame->toggleThreshold();
    if (state){
        this->dialog_threshold->show();
        this->mainStatusLabel->setText("Threshold activated");
    }
    else {
        this->dialog_threshold->hide();
        this->mainStatusLabel->setText("Threshold desactivated");
    }
}

void MainWindow::treat_Button_Transformation(bool state) {
    this->myFrame->toggleTransformation();
    if (state) {
        this->dialog_transformation->show();
        this->mainStatusLabel->setText("Transformations activated");
    }
    else {
        this->dialog_transformation->hide();
        this->mainStatusLabel->setText("Transformations desactivated");
    }
}

void MainWindow::treat_Button_Edge(bool state) {
    this->myFrame->toggleEdge();
    if (state) {
        this->dialog_edge->show();
        this->mainStatusLabel->setText("Edge detection activated");
    }
    else {
        this->dialog_edge->hide();
        this->mainStatusLabel->setText("Edge detection desactivated");
    }
}

#ifdef withobjdetect
void MainWindow::treat_Button_Face_Recon(bool state) {
    
    if (!this->myFrame->getFace_Status()) {
        this->file_background = this->main_directory + "cartoon_background.jpg";
        bool test = this->myFrame->set_background_image(this->file_background);
        while (!test){
            QString QfileNameLocal = QFileDialog::getOpenFileName(this,
                                                             tr("Select a background image"),
                                                             QString::fromStdString(this->main_directory),
                                                             tr("Images (*.bmp *.png *.jpg *.jpeg *.jpe *.jp2 *.webp *.pbm *.pgm *.ppm *.pnm *.pfm *.src *.tiff *.tif *.exr *.hdr *.pic)") );
            if ( ! QfileNameLocal.isEmpty() ) {
                this->file_background = QfileNameLocal.toStdString() ;
                test = this->myFrame->set_background_image(this->file_background);
            }
        }
    }
    
    this->myFrame->toggleFace_Recon();
    if (state)
        this->mainStatusLabel->setText("Face detection activated");
    else
        this->mainStatusLabel->setText("Face detection desactivated");
}
#endif

void MainWindow::treat_Button_Histogram(bool state) {
    this->myFrame->toggleHistoEq();
    if (state) {
        this->dialog_histogram->show();
        this->mainStatusLabel->setText("Histogram equalization activated");
    }
    else {
        this->dialog_histogram->hide();
        this->mainStatusLabel->setText("Histogram equalization desactivated");
    }
}

void MainWindow::treat_Button_Object_Detection(bool state) {
    this->myFrame->toggleObjectDetection();
    this->object_detection_window_opened = state;
    if (state) {
        this->dialog_object_detection->show();
        this->thirdWindow->show();
        this->mainStatusLabel->setText("Point/line/circle detection activated");
    }
    else {
        this->dialog_object_detection->hide();
        this->thirdWindow->hide();
        this->mainStatusLabel->setText("Point/line/circle detection desactivated");
    }
}

#ifdef withstitching
void MainWindow::treat_Button_Panorama(bool state) {
    this->myFrame->togglePanorama();
    this->panorama_window_opened = state;
    if (state) {
        this->dialog_panorama->show();
        this->fourthWindow->show();
        this->mainStatusLabel->setText("Panorama creation activated");
    }
    else {
        this->dialog_panorama->hide();
        this->fourthWindow->hide();
        this->mainStatusLabel->setText("Panorama creation desactivated");
    }
}
#endif

void MainWindow::treat_Button_Motion_Detection(bool state) {
    this->myFrame->toggleMotionDetection();
    this->motion_detection_window_opened = state;
    if (state) {
        this->dialog_motion_detection->show();
        this->fifthWindow->show();
        this->mainStatusLabel->setText("Motion detection activated");
    }
    else {
        this->dialog_motion_detection->hide();
        this->fifthWindow->hide();
        this->mainStatusLabel->setText("Motion detection desactivated");
    }
}

void MainWindow::treat_Button_Photo(bool state) {
    this->myFrame->togglePhoto();
    if (state) {
        this->dialog_photo->show();
        this->mainStatusLabel->setText("Module Photo activated");
    }
    else {
        this->dialog_photo->hide();
        this->mainStatusLabel->setText("Module Photo desactivated");
    }
}

void MainWindow::treat_Button_Record(bool state) {
    this->recording = state;
    if (state) {
        if (! this->video_out.isOpened() ) {// FPS : this->capture.get(cv::CAP_PROP_FPS)

            QString QfileNameLocal = QFileDialog::getSaveFileName(this,
                                                                 tr("Filename for the video"),
                                                                 QString::fromStdString(this->main_directory),
                                                                 tr("Images (*.avi)") );
            if (QfileNameLocal.isEmpty()) {// If one clicked the cancel button, the string is empty
                this->recording = false;
                this->actionRecord->setChecked(false) ;
                return;
            }
            
            this->video_out_name = QfileNameLocal.toStdString();

            this->video_out.open(this->video_out_name,VideoWriter::fourcc('X','V','I','D'),
                                 10.,
                                 cv::Size(this->capture.get(cv::CAP_PROP_FRAME_WIDTH),
                                          this->capture.get(cv::CAP_PROP_FRAME_HEIGHT)),
                                 true);
        }
        this->mainStatusLabel->setText("Saving the video under "+QString::fromStdString(this->video_out_name));
    }
    else {
        if (this->video_out.isOpened() )
            this->video_out.release();

        this->record_time_blink = 0;
        this->mainStatusLabel->setText("Saved the video under "+QString::fromStdString(this->video_out_name));
    }
}

void MainWindow::treat_Button_Save() {
    QString QfileNameLocal = QFileDialog::getSaveFileName(this,
                                                         tr("File name to save the image"),
                                                         QString::fromStdString(this->main_directory),
                                                         tr("Images (*.png *.jpg)") );
    if (QfileNameLocal.isEmpty()) // If one clicked the cancel button, the string is empty
        return;

    this->file_name_save = QfileNameLocal.toStdString();

    vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(4);

    Mat imageMat = myFrame->get_image_content();
    Mat imageOutput;
    cvtColor(imageMat, imageOutput, COLOR_BGR2RGB);
    imwrite(this->file_name_save , imageOutput , compression_params );
    
    this->mainStatusLabel->setText("Saved the image under "+QfileNameLocal);
}

#ifdef withzbar
void MainWindow::treat_Button_QRcode(bool state) {
    this->myFrame->toggleQRcode();
    this->qrdecoder_activated = state;
    if (state)
        this->mainStatusLabel->setText("QR code detection activated");
    else
        this->mainStatusLabel->setText("QR code detection desactivated");
}
#endif

void MainWindow::treat_Slider_Blur_Range(int value) {
    this->myFrame->set_size_blur(value);
}

void MainWindow::treat_Blur_Method(int method) {
    this->myFrame->set_blur_method(method);
}

void MainWindow::treat_Slider_Blur_Element(int element) {
    this->myFrame->set_morpho_element(element);
}

void MainWindow::treat_Photo_SigmaS(int value){
    this->myFrame->set_photo_sigmas(value);
}

void MainWindow::treat_Photo_SigmaR(double value){
    this->myFrame->set_photo_sigmar(value);
}

void MainWindow::treat_Slider_Threshold_Value(int value) {
    this->myFrame->set_threshold_value(value);
}

void MainWindow::treat_Slider_Threshold_Blocksize_Value(int value) {
    this->myFrame->set_threshold_blocksize(value);
}

void MainWindow::treat_Threshold_Method(int method) {
    this->myFrame->set_threshold_method(method);
}

void MainWindow::treat_Threshold_Type(int type) {
    if (type == Qt::Checked)
        this->myFrame->set_threshold_type(1);
    else
        this->myFrame->set_threshold_type(0);
}

void MainWindow::treat_Transformation_Method(int method) {
    this->myFrame->set_transf_method(method);
}

void MainWindow::treat_Slider_Transformation_Rotation_Value(int value) {
    this->myFrame->set_transf_rotation_value(value);
}

void MainWindow::treat_Edge_Method(int method) {
    this->myFrame->set_edge_method(method);
}

void MainWindow::treat_Slider_canny_lowthreshold(int value) {
    this->myFrame->set_canny_threshold(value);
}

void MainWindow::treat_Slider_canny_ratio(double value) {
    this->myFrame->set_canny_ratio(value);
}

void MainWindow::treat_Histogram_method(int method) {
    this->myFrame->set_histo_eq_method(method);
}

void MainWindow::treat_Histogram_tiles(int value) {
    this->myFrame->set_histo_eq_tiles(value);
}

void MainWindow::treat_Histogram_clip_limit(int value) {
    this->myFrame->set_histo_eq_clip_limit(value);
}

void MainWindow::treat_Histogram_show_histogram(bool checked) {
    this->histogram_window_opened = checked;
    if (checked)
        this->secondWindow->show();
    else
        this->secondWindow->hide();
}

void MainWindow::treat_Object_Detection_Method(int method) {
    this->myFrame->set_object_detection_method(method);
}

void MainWindow::treat_Slider_hough_line_threshold(int value) {
    this->myFrame->set_hough_line_threshold(value);
}

#ifdef withstitching
void MainWindow::treat_Panorama_Pick_Up_Image() {
    this->myFrame->panorama_insert_image();
    this->treat_Panorama_Update();
}
#endif

#ifdef withstitching
void MainWindow::treat_Panorama_Pop_Up_Image() {
    this->myFrame->panorama_pop_up_image();
    int num_imgs = this->myFrame->panorama_get_size();
    this->dialog_panorama->set_QLabel_number_images(num_imgs);
    std::string return_status = "Removed the last image from the stack";
    this->dialog_panorama->set_QLabel_string(return_status);
}
#endif

#ifdef withstitching
void MainWindow::treat_Panorama_Update() {
    std::string return_status = "Computing the new panorama...";
    this->dialog_panorama->set_QLabel_string(return_status);

    return_status = this->myFrame->panorama_compute_result();
    Mat imageMat = this->myFrame->get_image_panorama();
    int num_imgs = this->myFrame->panorama_get_size();
    this->dialog_panorama->set_QLabel_number_images(num_imgs);
    this->dialog_panorama->set_QLabel_string(return_status);

    this->myFourthImage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    this->fourthWindow->set_image_content(this->myFourthImage, imageMat.cols, imageMat.rows );
}
#endif

#ifdef withstitching
void MainWindow::treat_Panorama_Reset() {
    this->myFrame->panorama_reset();
    int num_imgs = this->myFrame->panorama_get_size();
    this->dialog_panorama->set_QLabel_number_images(num_imgs);
    std::string return_status = "Reset the stack";
    this->dialog_panorama->set_QLabel_string(return_status);

    Mat imageMat = this->myFrame->get_image_panorama();
    this->myFourthImage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    this->fourthWindow->set_image_content(this->myFourthImage, imageMat.cols, imageMat.rows );
}
#endif

#ifdef withstitching
void MainWindow::treat_Panorama_Save() {
    Mat imageMat = this->myFrame->get_image_panorama();

    QString QfileNameLocal = QFileDialog::getSaveFileName(this,
                                                         tr("File name to save the panorama"),
                                                         QString::fromStdString(this->main_directory),
                                                         tr("Images (*.png *.jpg)") );
    if (QfileNameLocal.isEmpty()) // If one clicked the cancel button, the string is empty
        return;

    this->file_name_save = QfileNameLocal.toStdString();

    vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(4);

    Mat imageOutput;
    cvtColor(imageMat, imageOutput, COLOR_BGR2RGB);
    imwrite(this->file_name_save , imageOutput , compression_params );
}
#endif

void MainWindow::treat_Motion_Detection_Method(int method) {
    this->myFrame->set_motion_detection_method(method);
}

void MainWindow::treat_Photo_Method(int method) {
    this->myFrame->set_photo_method(method);
}

void MainWindow::update_histogram_window() {
    // Called by repaint event every 10ms
    // Update the display of the histogram in the secondary window
    Mat imageMat = this->myFrame->get_image_histogram();
    this->mySecondQimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    this->secondWindow->set_image_content(this->mySecondQimage, imageMat.cols, imageMat.rows );
}

void MainWindow::update_objects_window() {
    // Called by repaint event every 10ms
    // Update the display of the objects detection in the secondary window
    Mat imageMat = this->myFrame->get_object_detected();
    this->myThirdImage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    this->thirdWindow->set_image_content(this->myThirdImage, imageMat.cols, imageMat.rows );
}

void MainWindow::update_motion_window() {
    // Called by repaint event every 10ms
    // Update the display of the motion detection in the secondary window
    Mat imageMat = this->myFrame->get_motion_detected();
    this->myFifthImage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    this->fifthWindow->set_image_content(this->myFifthImage, imageMat.cols, imageMat.rows );
}

#ifdef withzbar
// Look for data from QRcode decoded by ZBar. URLs and ISBN codes can be opened in an external web browser
void MainWindow::look_for_qrURL(){
    if (this->qrdecoder_activated){
        string qrdata,qrtype;
        if ( this->myFrame->getQRcodedata(qrdata,qrtype) ) {
            
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

void MainWindow::paintEvent(QPaintEvent* ) {
    update_frame();

    if (this->histogram_window_opened)
        update_histogram_window();

    if (this->object_detection_window_opened)
        update_objects_window();

    if (this->motion_detection_window_opened)
        update_motion_window();

    // Convert the QImage to a QLabel and set the content of the main window
/*    this->imageScene->clear();
    this->imageView->resetMatrix();
    QPixmap image = QPixmap::fromImage(this->myQimage) ;
    this->currentImage = imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
*/    
    //QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
    //        .arg(image.height()).arg(QFile(path).size());
    //    mainStatusLabel->setText(status);
    //    currentImagePath = path;
    
    this->Window_image->setPixmap(QPixmap::fromImage(this->myQimage));
    
#ifdef withzbar
    look_for_qrURL();
#endif
}
