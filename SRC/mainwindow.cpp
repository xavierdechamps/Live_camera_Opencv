/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *  The Qt Window that contains the opencv images and all the secondary windows for image treatment.
 *  Receives signals from secondary windows and sends signals to opencv for corresponding treatment.
*/

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), my_Timer(this)
{
    // Initializations
    this->myFrame = new MyImage();
    this->main_directory = "/Users/dechamps/Documents/Codes/Cpp/Images/";
    String tmp = "/Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.1.2/data/haarcascades/haarcascade_frontalface_default.xml";
    bool testCascade = this->myFrame->set_Face_Cascade_Name(tmp);
    while (!testCascade){
        QString QfileNameLocal = QFileDialog::getOpenFileName(this,
                                                         tr("Select the face cascade file haarcascade_frontalface_default.xml"),
                                                         QString::fromStdString(this->main_directory),
                                                         tr("Images (*.xml)") );
        if ( ! QfileNameLocal.isEmpty() ) {
            tmp = QfileNameLocal.toStdString() ;
            testCascade = this->myFrame->set_Face_Cascade_Name(tmp);
        }
    }
    this->file_name_save = this->main_directory + "Video-OpenCV-QMake/webcam.jpg";
    this->capture.open(0);

    // QLabel that will actually show the video, conversion from QImage with QPixmap::fromImage
    this->Window_image = new QLabel(this);
    this->setCentralWidget(this->Window_image);

    createActions();
    createToolBars();

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

    // Create a new object for stitching operations and create adequate connections to functions, depending on the received signals
    this->dialog_panorama = new Dialog_Panorama(this);
    connect(this->dialog_panorama,SIGNAL(Signal_pick_up_image_panorama()), this, SLOT(treat_Panorama_Pick_Up_Image()));
    connect(this->dialog_panorama,SIGNAL(Signal_pop_up_image_panorama()), this, SLOT(treat_Panorama_Pop_Up_Image()));
    connect(this->dialog_panorama,SIGNAL(Signal_update_panorama()), this, SLOT(treat_Panorama_Update()));
    connect(this->dialog_panorama,SIGNAL(Signal_reset_panorama()), this, SLOT(treat_Panorama_Reset()));
    connect(this->dialog_panorama,SIGNAL(Signal_save_panorama()), this, SLOT(treat_Panorama_Save()));
    this->dialog_panorama->hide();

    // Create a new object for motion detection and create adequate connections to functions, depending on the received signals
    this->dialog_motion_detection = new Dialog_Motion_Detection(this);
    connect(this->dialog_motion_detection, SIGNAL(Signal_motion_detection_method_changed(int)), this, SLOT(treat_Motion_Detection_Method(int)) );
    this->dialog_motion_detection->hide();

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

    // Create a new object for a secondary window that will show the panorama resulting from the stitching operation
    this->fourthWindow = new SecondaryWindow(this);
    this->fourthWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->fourthWindow->hide();
    this->panorama_window_opened = false;

    // Create a new object for a secondary window that will show the motion detection
    this->fifthWindow = new SecondaryWindow(this);
    this->fifthWindow->setWindowFlags(Qt::Window); // to show the close/minimize/maximize buttons
    this->fifthWindow->hide();
    this->motion_detection_window_opened = false;

    // Concerns the recording of the video
    this->record_time_blink = 0;
    this->recording = false;
    this->video_out_name = this->main_directory + "Video-OpenCV-QMake/my_video.avi";

    this->setFixedSize(740, 480);

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
    connect(this->actionColour_BW, SIGNAL(triggered()), this, SLOT(treat_Button_BW()));

    this->actionInverse = new QAction(tr("&Inverse"), this);
    this->actionInverse->setToolTip(tr("Inverse the image"));
    this->actionInverse->setCheckable(true);
    connect(this->actionInverse, SIGNAL(triggered()), this, SLOT(treat_Button_Inverse()));

    this->actionBlur = new QAction(tr("&Blur"), this);
    this->actionBlur->setToolTip(tr("Blur the image"));
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

    this->actionFace = new QAction(tr("&Face recognition"), this);
    this->actionFace->setToolTip(tr("Find human faces in the image"));
    this->actionFace->setCheckable(true);
    connect(this->actionFace, SIGNAL(triggered()), this, SLOT(treat_Button_Face_Recon()));

    this->actionHistoEq = new QAction(tr("&Histogram equalization"), this);
    this->actionHistoEq->setToolTip(tr("Equalize the histogram of the image"));
    this->actionHistoEq->setCheckable(true);
    connect(this->actionHistoEq, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Histogram(bool)));

    this->actionObjectDetection = new QAction(tr("&Object detection"), this);
    this->actionObjectDetection->setToolTip(tr("Detect patterns in the image"));
    this->actionObjectDetection->setCheckable(true);
    connect(this->actionObjectDetection, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Object_Detection(bool)));

    this->actionPanorama = new QAction(tr("&Panorama"), this);
    this->actionPanorama->setToolTip(tr("Create a panorama from chosen images"));
    this->actionPanorama->setCheckable(true);
    connect(this->actionPanorama, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Panorama(bool)));

    this->actionMotionDetection = new QAction(tr("&Motion Detection"), this);
    this->actionMotionDetection->setToolTip(tr("Detect the motion in the webcam"));
    this->actionMotionDetection->setCheckable(true);
    connect(this->actionMotionDetection, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Motion_Detection(bool)));

    this->actionRecord = new QAction(tr("&Record"), this);
    this->actionRecord->setToolTip(tr("Record the video"));
    this->actionRecord->setCheckable(true);
    connect(this->actionRecord, SIGNAL(triggered(bool)), this, SLOT(treat_Button_Record(bool)));
}

void MainWindow::createToolBars() {
    // Build the toolbar with all the buttons to open the secondary windows for image treatment

    this->editToolBar = new QToolBar(tr("&Edit"), this);
    this->editToolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
    this->editToolBar->setFixedWidth(100);

    this->editToolBar->addAction(this->actionColour_BW);
    this->editToolBar->addAction(this->actionInverse);
    this->editToolBar->addSeparator();
    this->editToolBar->addAction(this->actionTransformation);
    this->editToolBar->addAction(this->actionBlur);
    this->editToolBar->addAction(this->actionThreshold);
    this->editToolBar->addAction(this->actionHistoEq);
    this->editToolBar->addSeparator();
    this->editToolBar->addAction(this->actionEdge);
    this->editToolBar->addAction(this->actionFace);
    this->editToolBar->addAction(this->actionObjectDetection);
    this->editToolBar->addSeparator();
    this->editToolBar->addAction(this->actionPanorama);
    this->editToolBar->addAction(this->actionMotionDetection);
    this->editToolBar->addSeparator();
    this->editToolBar->addAction(this->actionRecord);
    addToolBar(Qt::RightToolBarArea, this->editToolBar);
}

void MainWindow::treat_Button_BW() {
    this->myFrame->toggleBW() ;
}

void MainWindow::treat_Button_Inverse() {
    this->myFrame->toggleInverse() ;
}

void MainWindow::treat_Button_Blur(bool state) {
    this->myFrame->toggleBlur();
    if (state)
        this->dialog_blur->show();
    else
        this->dialog_blur->hide();
}

void MainWindow::treat_Button_Threshold(bool state) {
    this->myFrame->toggleThreshold();
    if (state)
        this->dialog_threshold->show();
    else
        this->dialog_threshold->hide();
}

void MainWindow::treat_Button_Transformation(bool state) {
    this->myFrame->toggleTransformation();
    if (state)
        this->dialog_transformation->show();
    else
        this->dialog_transformation->hide();
}

void MainWindow::treat_Button_Edge(bool state) {
    this->myFrame->toggleEdge();
    if (state)
        this->dialog_edge->show();
    else
        this->dialog_edge->hide();
}

void MainWindow::treat_Button_Face_Recon() {
    this->myFrame->toggleFace_Recon();
}

void MainWindow::treat_Button_Histogram(bool state) {
    this->myFrame->toggleHistoEq();
    if (state)
        this->dialog_histogram->show();
    else
        this->dialog_histogram->hide();
}

void MainWindow::treat_Button_Object_Detection(bool state) {
    this->myFrame->toggleObjectDetection();
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

void MainWindow::treat_Button_Panorama(bool state) {
    this->myFrame->togglePanorama();
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

void MainWindow::treat_Button_Motion_Detection(bool state) {
    this->myFrame->toggleMotionDetection();
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

void MainWindow::treat_Button_Record(bool state) {
    this->recording = state;
    if (state) {
        if (! this->video_out.isOpened() ) {// FPS : this->capture.get(cv::CAP_PROP_FPS)

            QString QfileNameLocal = QFileDialog::getSaveFileName(this,
                                                                 tr("File name to save the video"),
                                                                 QString::fromStdString(this->main_directory),
                                                                 tr("Images (*.avi)") );
            this->video_out_name = QfileNameLocal.toStdString();

            this->video_out.open(this->video_out_name,VideoWriter::fourcc('X','V','I','D'),
                                 10.,
                                 cv::Size(this->capture.get(cv::CAP_PROP_FRAME_WIDTH),
                                          this->capture.get(cv::CAP_PROP_FRAME_HEIGHT)),
                                 true);
        }
    }
    else {
        if (this->video_out.isOpened() )
            this->video_out.release();

        this->record_time_blink = 0;
    }
}

void MainWindow::treat_Slider_Blur_Range(int value) {
    this->myFrame->set_size_blur(value);
}

void MainWindow::treat_Blur_Method(int method) {
    this->myFrame->set_blur_method(method);
}

void MainWindow::treat_Slider_Blur_Element(int element) {
    this->myFrame->set_morpho_element(element);
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

void MainWindow::treat_Panorama_Pick_Up_Image() {
    this->myFrame->panorama_insert_image();
    this->treat_Panorama_Update();
}

void MainWindow::treat_Panorama_Pop_Up_Image() {
    this->myFrame->panorama_pop_up_image();
    int num_imgs = this->myFrame->panorama_get_size();
    this->dialog_panorama->set_QLabel_number_images(num_imgs);
    std::string return_status = "Removed the last image from the stack";
    this->dialog_panorama->set_QLabel_string(return_status);
}

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

void MainWindow::treat_Panorama_Save() {
    Mat imageMat = this->myFrame->get_image_panorama();

    QString QfileNameLocal = QFileDialog::getSaveFileName(this,
                                                         tr("File name to save the panorama"),
                                                         QString::fromStdString(this->main_directory),
                                                         tr("Images (*.png *.jpg)") );
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

void MainWindow::treat_Motion_Detection_Method(int method) {
    this->myFrame->set_motion_detection_method(method);
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

void MainWindow::paintEvent(QPaintEvent* ) {
    update_frame();

    if (this->histogram_window_opened)
        update_histogram_window();

    if (this->object_detection_window_opened)
        update_objects_window();

    if (this->motion_detection_window_opened)
        update_motion_window();

    // Convert the QImage to a QLabel and set the content of the main window
    this->Window_image->setPixmap(QPixmap::fromImage(this->myQimage));
}
