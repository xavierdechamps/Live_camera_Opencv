/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *   Management of thread that takes care of capturing the camera, applying image
 *   processes on it and sending the treated image to the GUI MainWindow through signals
 * 
 *   Multithreaded implementation. The class captureVideo is solely dedicated to the treatment
 *   of the image and the saving of movies/images to local files.
*/

#include "capturevideo.h"

/**
 * @brief captureVideo::captureVideo
 * @param parent: object of class QMainWindow to open QFileDialog windows
 * @param data_lock: object of class QMutex to lock-unlock access to common variables between threads
 * 
 * Constructor of the class captureVideo. 
 */
captureVideo::captureVideo(QMainWindow *parent,QMutex *data_lock) : mainWindowParent(parent),
                                                                    data_lock(data_lock)
{
    this->running          = false; // Is this thread running?
    this->motion_active    = false; // Is motion detection activated?
    this->objects_active   = false; // Is object detection activated?
    this->qrdecoder_active = false; // Is QR decoder activated?
    this->histo_active     = false; // Is histogram activated?
    this->panorama_active  = false; // Is panorama stitching activated?
    
    this->camID = -1;
    
    // Initializations
    this->main_directory = "/Users/dechamps/Documents/Codes/Cpp/Images/";
    this->file_name_save = this->main_directory + "webcam.jpg";
    
    // Concerns the recording of the video
    this->record_time_blink = 0;
    this->recording = false;
    this->video_out_name = this->main_directory + "Video-OpenCV-QMake/my_video.avi";
}

/**
 * @brief captureVideo::setParent
 * @param parent
 * 
 * Set a link to an object of type QMainWindow in order to be able to open QFileDialog windows
 */
void captureVideo::setParent(QMainWindow *parent) {
    this->mainWindowParent=parent;
}

/**
 * @brief captureVideo::setCamera
 * @param camera
 * 
 * Set the ID number of the camera to open
 */
void captureVideo::setCamera(int camera){
    this->camID = camera;
}

/**
 * @brief captureVideo::openCamera
 * @return true if the camera is open
 *         false if the camera ID is not set yet
 * 
 * Open the camera anad initialize a new object of class MyImage
 */
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

/**
 * @brief captureVideo::closeCamera
 * @return true if everything went well
 * 
 * Close the camera and delete the object of class MyImage. Close also the current movie if active.
 */
bool captureVideo::closeCamera() {
    if (cameraIsOpen()){
        this->capture.release();
        
        // Delete object of class MyImage
        if (this->myFrame != nullptr ) {
            delete this->myFrame ;
            this->myFrame = nullptr ;
        }
        
        // Stop saving the movie
        file_save_movie(false);
    }
    return true;
}

/**
 * @brief captureVideo::cameraIsOpen
 * @return the status of the camera
 */
bool captureVideo::cameraIsOpen(){
    return this->capture.isOpened();
}

/**
 * @brief captureVideo::setCascadeFile
 * 
 * Set the cascade file required by OpenCV. Active only if face detection is required in the .pro file.
 */
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

/**
 * @brief captureVideo::getQRcodedata
 * @param qrdata: string that describes the type of object decoded (QR code, codebar, ISBN, etc.)
 * @param qrtype: string that contains the decoded content of the object
 * @return true if a QR code/codebar has been decoded by ZBar/OpenCV.
 *         false otherwise
 */
bool captureVideo::getQRcodedata(string &qrdata, string &qrtype){
    return this->myFrame->getQRcodedata(qrdata,qrtype) ;
}

/**
 * @brief captureVideo::run
 * 
 * The core of the class captureVideo. It loops indefenitely to capture frames from the camera and 
 * to apply image processings on them. The treated images are sent to the GUI through signals.
 */
void captureVideo::run() {
    this->running = true;
    Mat imageMat;
    
    while (this->running) {
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
        
        if (this->histo_active) {
            Mat histoMat = this->myFrame->get_image_histogram();
            data_lock->lock();
            this->histoQimage = QImage(histoMat.data, histoMat.cols, histoMat.rows, histoMat.cols*3, QImage::Format_RGB888);
            data_lock->unlock();
            emit histogramCaptured(&this->histoQimage);
        }
        
        emit frameCaptured(&this->myQimage);
    }
    closeCamera();
    running = false;
}

void captureVideo::setThreadStatus(bool state){
    this->running = state;
}

//+++++++++++++++++++++++++++++++++++++++++++++ FUNCTIONS THAT TOGGLE FEATURES 

/**
 * @brief captureVideo::toggleBW
 * @param state
 * 
 * Activate/desactivate the conversion to Black & White
 */
void captureVideo::toggleBW(bool state){
    this->myFrame->toggleBW() ;
    if (state)
        emit changeInfo("Black and White activated");
    else
        emit changeInfo("Black and White desactivated");
}

/**
 * @brief captureVideo::toggleInverse
 * @param state
 * 
 * Activate/desactivate the inversion of the colours
 */
void captureVideo::toggleInverse(bool state){
    this->myFrame->toggleInverse() ;
    if (state)
        emit changeInfo("Inversed colours activated");
    else
        emit changeInfo("Inversed colours desactivated");
}

/**
 * @brief captureVideo::toggleBlur
 * @param state
 * 
 * Activate/desactivate the blurring of the image
 */
void captureVideo::toggleBlur(bool state) {
    this->myFrame->toggleBlur();
    if (state)
        emit changeInfo("Blur filter activated");
    else
        emit changeInfo("Blur filter desactivated");
}

/**
 * @brief captureVideo::toggleThreshold
 * @param state
 * 
 * Activate/desactivate the thresholding
 */
void captureVideo::toggleThreshold(bool state) {
    this->myFrame->toggleThreshold();
    if (state)
        emit changeInfo("Threshold activated");
    else
        emit changeInfo("Threshold desactivated");
}

/**
 * @brief captureVideo::toggleEdge
 * @param state
 * 
 * Activate/desactivate the edge detection algorithm
 */
void captureVideo::toggleEdge(bool state) {
    this->myFrame->toggleEdge();
    if (state)
        emit changeInfo("Edge detection activated");
    else 
        emit changeInfo("Edge detection desactivated");
}

/**
 * @brief captureVideo::toggleMotionDetection
 * @param state
 * 
 * Activate/desactivate the motion detection algorithm
 */
void captureVideo::toggleMotionDetection(bool state) {
    this->myFrame->toggleMotionDetection();
    this->motion_active = state;
    if (state) 
        emit changeInfo("Motion detection activated");
    else 
        emit changeInfo("Motion detection desactivated");
}


#ifdef withobjdetect
/**
 * @brief captureVideo::toggleFaceDetection
 * @param state
 * 
 * Activate/desactivate the face detection algorithm. A background image is fetch from a local directory
 */
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

/**
 * @brief captureVideo::toggleObjectDetection
 * @param state
 * 
 * Activate/desactivate the object detection algorithm for lines, circles and points
 */
void captureVideo::toggleObjectDetection(bool state) {
    this->myFrame->toggleObjectDetection();
    this->objects_active = state;
    if (state) 
        emit changeInfo("Point/line/circle detection activated");
    else 
        emit changeInfo("Point/line/circle detection desactivated");
}

#ifdef withzbar
/**
 * @brief captureVideo::toggleQRcode
 * @param state
 * 
 * Activate/desactivate the ZBar decoding of QR codes and barcodes
 */
void captureVideo::toggleQRcode(bool state){
    this->myFrame->toggleQRcode();
    this->qrdecoder_active = state;
    if (state)
        emit changeInfo("QR code detection activated");
    else
        emit changeInfo("QR code detection desactivated");
}
#endif

/**
 * @brief captureVideo::toggleTransformation
 * @param state
 * 
 * Activate/desactivate the geometrical transformations (rotations, etc.)
 */
void captureVideo::toggleTransformation(bool state){
    this->myFrame->toggleTransformation();
    if (state) 
        emit changeInfo("Transformations activated");
    else 
        emit changeInfo("Transformations desactivated");
}

/**
 * @brief captureVideo::toggleHistogramEqualization
 * @param state
 * 
 * Activate/desactivate the histogram equalization + show histogram
 */
void captureVideo::toggleHistogramEqualization(bool state){
    this->myFrame->toggleHistoEq();
    if (state) 
        emit changeInfo("Histogram equalization activated");
    else 
        emit changeInfo("Histogram equalization desactivated");
}

#ifdef withstitching
/**
 * @brief captureVideo::togglePanorama
 * @param state
 * 
 * Activate/desactivate the image stitching operation
 */
void captureVideo::togglePanorama(bool state){
    this->myFrame->togglePanorama();
    this->panorama_active = state;
    if (state) 
        emit changeInfo("Panorama creation activated");
    else 
        emit changeInfo("Panorama creation desactivated");
}
#endif

/**
 * @brief captureVideo::togglePhoto
 * @param state
 * 
 * Activate/desactivate the photo module
 */
void captureVideo::togglePhoto(bool state) {
    this->myFrame->togglePhoto();
    if (state) 
        emit changeInfo("Module Photo activated");
    else 
        emit changeInfo("Module Photo desactivated");
}

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

void captureVideo::change_transformation_method(int method){
    this->myFrame->set_transf_method(method);
}

void captureVideo::change_transformation_value_rotation(int value) {
    this->myFrame->set_transf_rotation_value(value);
}

void captureVideo::change_histogram_method(int method){
    this->myFrame->set_histo_eq_method(method);
}

void captureVideo::change_histogram_tiles(int value){
    this->myFrame->set_histo_eq_tiles(value);
}

void captureVideo::change_histogram_clip_limit(int value){
    this->myFrame->set_histo_eq_clip_limit(value);
}

void captureVideo::change_histogram_show(bool state){
    this->histo_active = state;
}

#ifdef withstitching
/**
 * @brief captureVideo::panorama_pick_up_image
 * 
 * Insert a new image in the panorama stitching and update the result
 */
void captureVideo::panorama_pick_up_image(){
    this->myFrame->panorama_insert_image();
    emit panoramaInfo("Computing the new panorama...");
    panorama_update();
}

/**
 * @brief captureVideo::panorama_pop_out_image
 * 
 * Delete the last inserted image from the panorama and update the result
 */
void captureVideo::panorama_pop_out_image(){
    this->myFrame->panorama_pop_up_image();
    emit panoramaInfo("Removed the last image from the stack");
    panorama_update();
}

/**
 * @brief captureVideo::panorama_reset
 * 
 * Empty the panorama from all images
 */
void captureVideo::panorama_reset(){
    this->myFrame->panorama_reset();
    emit panoramaInfo("Reset the stack");
    panorama_update();
}

/**
 * @brief captureVideo::panorama_update
 * 
 * Compute the result of the image stitching and send the result to the GUI
 */
void captureVideo::panorama_update(){
    std::string return_status = this->myFrame->panorama_compute_result();
    Mat imageMat = this->myFrame->get_image_panorama();
    int num_imgs = this->myFrame->panorama_get_size();
    emit panoramaNumberImages(num_imgs);
    emit panoramaInfo(QString::fromStdString(return_status));
    
    data_lock->lock();
    this->panorama_Qimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    data_lock->unlock();
    
    emit panoramaCaptured(&this->panorama_Qimage);
}

/**
 * @brief captureVideo::panorama_save
 * 
 * Save the resulting panorama to a local file
 */
void captureVideo::panorama_save(){
    Mat imageMat = this->myFrame->get_image_panorama();

    QString QfileNameLocal = QFileDialog::getSaveFileName(this->mainWindowParent,
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

void captureVideo::change_photo_method(int method){
    this->myFrame->set_photo_method(method);
}

void captureVideo::change_photo_sigmas(int value){
    this->myFrame->set_photo_sigmas(value);
}

void captureVideo::change_photo_sigmar(double value){
    this->myFrame->set_photo_sigmar(value);
}

/**
 * @brief captureVideo::file_save_image
 * 
 * Save the current image on the camera to a local file
 */
void captureVideo::file_save_image() {
    QString QfileNameLocal = QFileDialog::getSaveFileName(this->mainWindowParent,
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
    
    emit changeInfo("Saved the image under "+QfileNameLocal);
}

/**
 * @brief captureVideo::file_save_movie
 * @param state: if true, start recording a movie from the camera
 *                 false, stop the recording of the movie
 * @return true if everything went well
 *        false if the user clicked on 'Cancel' when setting the filename for the movie.
 *              This allows the GUI to desactivate the button 'Record'
 * 
 * Managment of the recording to a movie.
 */
bool captureVideo::file_save_movie(bool state) {
    this->recording = state;
    if (state) {
        if (! this->video_out.isOpened() ) {// FPS : this->capture.get(cv::CAP_PROP_FPS)

            QString QfileNameLocal = QFileDialog::getSaveFileName(this->mainWindowParent,
                                                                 tr("Filename for the video"),
                                                                 QString::fromStdString(this->main_directory),
                                                                 tr("Images (*.avi)") );
            if (QfileNameLocal.isEmpty()) {// If one clicked the cancel button, the string is empty
                this->recording = false;
                return false;
            }
            
            this->video_out_name = QfileNameLocal.toStdString();

            this->video_out.open(this->video_out_name,VideoWriter::fourcc('X','V','I','D'),
                                 10.,
                                 cv::Size(this->capture.get(cv::CAP_PROP_FRAME_WIDTH),
                                          this->capture.get(cv::CAP_PROP_FRAME_HEIGHT)),
                                 true);
        }
        emit changeInfo("Saving the video under "+QString::fromStdString(this->video_out_name));
    }
    else {
        if (this->video_out.isOpened() ) 
            this->video_out.release();

        this->record_time_blink = 0;
        emit changeInfo("Saved the video under "+QString::fromStdString(this->video_out_name));
    }
    return true;
}
