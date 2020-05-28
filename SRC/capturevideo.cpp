#include "capturevideo.h"

captureVideo::captureVideo(QMainWindow *parent,QMutex *data_lock) : mainWindowParent(parent),
                                                                    data_lock(data_lock)
{
    //-------------------------------------------------------------
    // DO NOT allocate heap objects (using new) in this constructor
    //-------------------------------------------------------------
    
    this->running = false;
    this->motion_active = false;
    this->objects_active = false;
    this->qrdecoder_active = false;
    
    this->camID = -1;
    
    // Initializations
    this->main_directory = "/Users/dechamps/Documents/Codes/Cpp/Images/";
    this->file_name_save = this->main_directory + "webcam.jpg";
    
    // Concerns the recording of the video
    this->record_time_blink = 0;
    this->recording = false;
    this->video_out_name = this->main_directory + "Video-OpenCV-QMake/my_video.avi";
}

void captureVideo::setParent(QMainWindow *parent) {
    this->mainWindowParent=parent;
}

void captureVideo::setCamera(int camera){
    this->camID = camera;
}

bool captureVideo::openCamera(){
    if (this->camID<0) {
        qDebug() << "captureVideo::openCamera - Cannot open camera";
        return false;
    }
    closeCamera();
    
    // Create a new OpenCV MyImage, don't initialize it in the constructor otherwise 
    // myFrame is owned by mainWindow
    this->myFrame = new MyImage();
    setCascadeFile();
    
    return this->capture.open(this->camID);
}

bool captureVideo::closeCamera() {
    if (cameraIsOpen()){
        this->capture.release();
        if (this->myFrame != nullptr )
            delete this->myFrame ;
    }
    return true;
}

bool captureVideo::cameraIsOpen(){
    return this->capture.isOpened();
}

void captureVideo::setCascadeFile(){
#ifdef withobjdetect
    if (this->mainWindowParent != nullptr) {
        this->file_cascade = this->main_directory + "Libraries/opencv-4.3.0/install/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
        bool testCascade = this->myFrame->set_Face_Cascade_Name(this->file_cascade);
        while (!testCascade){
            QString QfileNameLocal = QFileDialog::getOpenFileName(this->mainWindowParent,
                                                             tr("Select the face cascade file haarcascade_frontalface_default.xml"),
                                                             QString::fromStdString(this->main_directory),
                                                             tr("Images (*.xml)") );
            if ( ! QfileNameLocal.isEmpty() ) {
                this->file_cascade = QfileNameLocal.toStdString() ;
                testCascade = this->myFrame->set_Face_Cascade_Name(this->file_cascade);
            }
    
        }
    }
    else {
        qDebug() << "captureVideo::setCascadeFile() : mainWindowParent is not set";
        qDebug() << "                                 call the function captureVideo::setParent(QMainWindow *parent)";
    }
#endif
}

bool captureVideo::getQRcodedata(string &qrdata, string &qrtype){
    return this->myFrame->getQRcodedata(qrdata,qrtype) ;
}

void captureVideo::run() {
    running = true;
    Mat imageMat;
    
    while (running) {
        if(cameraIsOpen()) {
            this->capture >> imageMat;
            while (imageMat.empty()) {
                this->capture >> imageMat;
                qDebug() << "captureVideo::run() : Empty image";
            }
    
            // Send the frame to the class MyImage for post-processing
            this->myFrame->set_image_content(imageMat);
    
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
        }
        else
        {
            qDebug() << "captureVideo::run() : Camera is not open";
            imageMat = Mat::zeros(480, 640, CV_8UC1);
            myFrame->set_image_content(imageMat);
            imageMat = myFrame->get_image_content();
        }
        
        // Convert the opencv image to a QImage that will be displayed on the main window
        cvtColor(imageMat, imageMat, COLOR_BGR2RGB);
        
        data_lock->lock();
        this->myQimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
        data_lock->unlock();
        
        if (this->motion_active) {
            Mat motionMat = this->myFrame->get_motion_detected();
            data_lock->lock();
            this->motionQimage = QImage(motionMat.data, motionMat.cols, motionMat.rows, motionMat.cols*3, QImage::Format_RGB888);            
            data_lock->unlock();
            emit motionCaptured(&this->motionQimage);
        }
        
        if (this->objects_active) {
            Mat objectsMat = this->myFrame->get_object_detected();
            data_lock->lock();
            this->objectsQimage = QImage(objectsMat.data, objectsMat.cols, objectsMat.rows, objectsMat.cols*3, QImage::Format_RGB888);            
            data_lock->unlock();
            emit objectsCaptured(&this->objectsQimage);
        }
        
        emit frameCaptured(&this->myQimage);
    }
    closeCamera();
    running = false;
}

//+++++++++++++++++++++++++++++++++++++++++++++ FUNCTIONS THAT TOGGLE FEATURES 

void captureVideo::toggleBW(bool state){
    this->myFrame->toggleBW() ;
    if (state)
        emit changeInfo("Black and White activated");
    else
        emit changeInfo("Black and White desactivated");
}

void captureVideo::toggleInverse(bool state){
    this->myFrame->toggleInverse() ;
    if (state)
        emit changeInfo("Inversed colours activated");
    else
        emit changeInfo("Inversed colours desactivated");
}

void captureVideo::toggleBlur(bool state) {
    this->myFrame->toggleBlur();
    if (state)
        emit changeInfo("Blur filter activated");
    else
        emit changeInfo("Blur filter desactivated");
}

void captureVideo::toggleThreshold(bool state) {
    this->myFrame->toggleThreshold();
    if (state)
        emit changeInfo("Threshold activated");
    else
        emit changeInfo("Threshold desactivated");
}

void captureVideo::toggleEdge(bool state) {
    this->myFrame->toggleEdge();
    if (state)
        emit changeInfo("Edge detection activated");
    else 
        emit changeInfo("Edge detection desactivated");
}

void captureVideo::toggleMotionDetection(bool state) {
    this->myFrame->toggleMotionDetection();
    this->motion_active = state;
    if (state) 
        emit changeInfo("Motion detection activated");
    else 
        emit changeInfo("Motion detection desactivated");
}

#ifdef withobjdetect
void captureVideo::toggleFaceDetection(bool state) {
    
    if (!this->myFrame->getFace_Status()) {
        this->file_background = this->main_directory + "cartoon_background.jpg";
        bool test = this->myFrame->set_background_image(this->file_background);
        while (!test){
            QString QfileNameLocal = QFileDialog::getOpenFileName(this->mainWindowParent,
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
        emit changeInfo("Face detection activated");
    else
        emit changeInfo("Face detection desactivated");
}
#endif

void captureVideo::toggleObjectDetection(bool state) {
    this->myFrame->toggleObjectDetection();
    this->objects_active = state;
    if (state) 
        emit changeInfo("Point/line/circle detection activated");
    else 
        emit changeInfo("Point/line/circle detection desactivated");
}

#ifdef withzbar
void captureVideo::toggleQRcode(bool state){
    this->myFrame->toggleQRcode();
    this->qrdecoder_active = state;
    if (state)
        emit changeInfo("QR code detection activated");
    else
        emit changeInfo("QR code detection desactivated");
}
#endif

//+++++++++++++++++++++++++++++++++++++++++++++ FUNCTIONS THAT CHANGE VALUES 
void captureVideo::change_blur_range(int value) {
    this->myFrame->set_size_blur(value);
}

void captureVideo::change_blur_method(int method) {
    this->myFrame->set_blur_method(method);
}

void captureVideo::change_blur_element(int element) {
    this->myFrame->set_morpho_element(element);
}

void captureVideo::change_threshold_value(int value) {
    this->myFrame->set_threshold_value(value);
}

void captureVideo::change_threshold_method(int method){
    this->myFrame->set_threshold_method(method);
}

void captureVideo::change_threshold_blocksize(int value){
    this->myFrame->set_threshold_blocksize(value);
}

void captureVideo::change_threshold_type(int type){
    if (type == Qt::Checked)
        this->myFrame->set_threshold_type(1);
    else
        this->myFrame->set_threshold_type(0);
}

void captureVideo::change_edge_method(int method) {
    this->myFrame->set_edge_method(method);
}

void captureVideo::change_edge_canny_lowthreshold(int value) {
    this->myFrame->set_canny_threshold(value);
}

void captureVideo::change_edge_canny_ratio(double value) {
    this->myFrame->set_canny_ratio(value);
}

void captureVideo::change_motion_detection_method(int method){
    this->myFrame->set_motion_detection_method(method);
}

void captureVideo::change_object_detection_method(int method){
    this->myFrame->set_object_detection_method(method);
}

void captureVideo::change_object_hough_line_threshold(int value){
    this->myFrame->set_hough_line_threshold(value);
}
