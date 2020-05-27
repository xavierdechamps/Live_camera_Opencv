#include "capturevideo.h"

captureVideo::captureVideo(QMutex *data_lock) : data_lock(data_lock)
{
    //-------------------------------------------------------------
    // DO NOT allocate heap objects (using new) in this constructor
    //-------------------------------------------------------------
    
    running = false;
    
    this->camID = -1;
    this->mainWindowParent = nullptr;
    
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
    }
#endif
}

void captureVideo::run() {
    running = true;
    Mat imageMat;
    
    while (running) {
        if(cameraIsOpen()) {
            this->capture >> imageMat;
            while (imageMat.empty()) {
                this->capture >> imageMat;
                qDebug() << "captureVideo::onTimeOut() : Empty image";
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
            qDebug() << "captureVideo::onTimeOut() : Camera is not open";
            imageMat = Mat::zeros(480, 640, CV_8UC1);
            myFrame->set_image_content(imageMat);
            imageMat = myFrame->get_image_content();
        }
        
        // Convert the opencv image to a QImage that will be displayed on the main window
        cvtColor(imageMat, imageMat, COLOR_BGR2RGB);
        
        data_lock->lock();
        this->myQimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
        data_lock->unlock();
        
        emit frameCaptured(&this->myQimage);
    }
    closeCamera();
    running = false;
}

void captureVideo::onTimeOut(){
    // Capture the frame and apply OpenCV on it + send signal with QImage
    // Signal received by mainwindow to display in Qt window
    
    Mat imageMat;

    if(cameraIsOpen()) {

        this->capture >> imageMat;
        while (imageMat.empty()) {
            this->capture >> imageMat;
            qDebug() << "captureVideo::onTimeOut() : Empty image";
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
        qDebug() << "captureVideo::onTimeOut() : Camera is not open";
        imageMat = Mat::zeros(480, 640, CV_8UC1);
        myFrame->set_image_content(imageMat);
        imageMat = myFrame->get_image_content();
    }
    
    // Convert the opencv image to a QImage that will be displayed on the main window
    cvtColor(imageMat, imageMat, COLOR_BGR2RGB);
    this->myQimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    emit frameCaptured(&this->myQimage);
    
//    qDebug()<<"Worker::onTimeout get called from?: "<<QThread::currentThreadId();
}

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

void captureVideo::change_blur_range(int value) {
    this->myFrame->set_size_blur(value);
}

void captureVideo::change_blur_method(int method) {
    this->myFrame->set_blur_method(method);
}

void captureVideo::change_blur_element(int element) {
    this->myFrame->set_morpho_element(element);
}
