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
 * Constructor of the class captureVideo. Initialization of intern variables.
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
    this->main_directory = QCoreApplication::applicationDirPath().toStdString();
    this->file_name_save = this->main_directory + "webcam.jpg";
#ifdef withobjdetect
    this->file_cascade = OPENCV_HAARCASCADES_DIR"haarcascade_frontalface_default.xml" ;
#ifdef withface
    this->file_facemark = OPENCV_FACEMARK_DIR"lbfmodel.yaml";
#endif // endif withface
#endif // endif withobjdetect
    
    // Concerns the recording of the video
    this->record_time_blink = 0;
    this->recording = false;
    this->video_out_name = this->main_directory + "my_video.avi";
    this->cameraFPS = 20.;
    this->cameraFPScalculated = false;
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
 * @param camera: integer the ID number of the camera to open
 *
 * Set the ID number of the camera to open
 */
void captureVideo::setCamera(int camera){
    this->camID = camera;
}

/**
 * @brief captureVideo::openCamera
 * @return true if the camera is open. False if the camera ID is not set yet
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
    
#ifdef withobjdetect
    // Set the cascade file for face detection
    if (!this->setCascadeFile()){
        this->running = false;
        return false;
    }

#ifdef withface
    // Set the facemark file for face detection
    if (!this->setFacemarkFile()){
        this->running = false;
        return false;
    }
#endif // endif withface
#endif // endif withobjdetect

    // Load the ornaments for face and send them to myFrame
    if (!this->loadOrnaments()){
        this->running = false;
        return false;
    }
    
    return this->capture.open(this->camID);
}

/**
 * @brief captureVideo::closeCamera
 * @return true if everything went well. False otherwise.
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

#ifdef withzbar
/**
 * @brief captureVideo::getQRcodedata
 * @param qrdata: string that describes the type of object decoded (QR code, codebar, ISBN, etc.)
 * @param qrtype: string that contains the decoded content of the object
 * @return true if a QR code/codebar has been decoded by ZBar/OpenCV. False otherwise
 */
bool captureVideo::getQRcodedata(std::string &qrdata, std::string &qrtype){
    return this->myFrame->getQRcodedata(qrdata,qrtype) ;
}
#endif

/**
 * @brief captureVideo::run
 *
 * The core of the class captureVideo. It loops indefenitely to capture frames from the camera and
 * to apply image processings on them. The treated images are sent to the GUI through signals.
 */
void captureVideo::run() {
    this->running = true;
    cv::Mat imageMat;
    QTime timer;
    const int count_2_read = 100; // 100 frames to compute the FPS
    int current_count = 0;
    bool first_time = true;

    while (this->running) {
        if(cameraIsOpen()) {
            if (first_time){
                timer.start();
                first_time = false;
            }
            this->capture >> imageMat;
            while (imageMat.empty()) {
                this->capture >> imageMat;
                qDebug() << "captureVideo::run() : Empty image";
            }

            // Send the frame to the class MyImage for post-processing
            this->myFrame->set_image_content(imageMat);

            // Get the image after post-processing
            imageMat = myFrame->get_image_content();
            imageMat.copyTo(this->currentMat);
            
            // Record the video
            if (this->recording) {
                this->video_out << imageMat; // save the image before the colour conversion
                if (this->record_time_blink <= 20) // Display a blinking red circle
                    cv::circle(imageMat,cv::Point(20,20),15, cv::Scalar(0,0,255), -1, 8);
                this->record_time_blink ++;
                if (this->record_time_blink >= 40)
                    this->record_time_blink = 0;
            }
            current_count ++;
            if (current_count == count_2_read) {
                // Compute the FPS
                int elapsed_ms = timer.elapsed();
                this->cameraFPS = count_2_read / (elapsed_ms / 1000.0) ;
                this->cameraFPScalculated = true;
                emit changeInfo("The FPS of the camera has been computed: "+QString::number(this->cameraFPS));
//                emit this->setFPSrate(this->cameraFPS);
            }
        }
        else
        {
            qDebug() << "captureVideo::run() : Camera is not open";
            imageMat = cv::Mat::zeros(480, 640, CV_8UC1);
            myFrame->set_image_content(imageMat);
            imageMat = myFrame->get_image_content();
        }

        // Convert the opencv image to a QImage that will be displayed on the main window
        cv::cvtColor(imageMat, imageMat, cv::COLOR_BGR2RGB);

        this->data_lock->lock();
        this->myQimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
        this->data_lock->unlock();
        
        // Launch the motion detection algorithm and send the result to the Qt manager
        if (this->motion_active) {
            cv::Mat motionMat = this->myFrame->get_motion_detected();
            this->data_lock->lock();
            this->motionQimage = QImage(motionMat.data, motionMat.cols, motionMat.rows, motionMat.cols*3, QImage::Format_RGB888);
            this->data_lock->unlock();
            emit motionCaptured(&this->motionQimage);
        }

        // Launch the object detection algorithm and send the result to the Qt manager
        if (this->objects_active) {
            cv::Mat objectsMat = this->myFrame->get_object_detected();
            this->data_lock->lock();
            this->objectsQimage = QImage(objectsMat.data, objectsMat.cols, objectsMat.rows, objectsMat.cols*3, QImage::Format_RGB888);
            this->data_lock->unlock();
            emit objectsCaptured(&this->objectsQimage);
        }
        
        // Launch the histogram equalization algorithm and send the result to the Qt manager
        if (this->histo_active) {
            cv::Mat histoMat = this->myFrame->get_image_histogram();
            this->data_lock->lock();
            this->histoQimage = QImage(histoMat.data, histoMat.cols, histoMat.rows, histoMat.cols*3, QImage::Format_RGB888);
            this->data_lock->unlock();
            emit histogramCaptured(&this->histoQimage);
        }
        
        // Send the main image to the Qt manager
        emit frameCaptured(&this->myQimage);
        
    } // end of big while on running
    closeCamera();
    this->running = false;
}

/**
 * @brief captureVideo::setThreadStatus
 * @param state: boolean the new status 
 * 
 * Change the status of the thread. If state=true, the thread is activated.
 * If false, the while loop in run() is escaped and  
 */
void captureVideo::setThreadStatus(bool state){
    this->running = state;
}

//+++++++++++++++++++++++++++++++++++++++++++++ FUNCTIONS THAT TOGGLE FEATURES

/**
 * @brief captureVideo::toggleBW
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the conversion to Black & White
 */
void captureVideo::toggleBW(bool state){
    this->myFrame->toggleBW(state) ;
    if (state)
        emit changeInfo("Black and White activated");
    else
        emit changeInfo("Black and White desactivated");
}

/**
 * @brief captureVideo::toggleInverse
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the inversion of the colours
 */
void captureVideo::toggleInverse(bool state){
    this->myFrame->toggleInverse(state) ;
    if (state)
        emit changeInfo("Inversed colours activated");
    else
        emit changeInfo("Inversed colours desactivated");
}

/**
 * @brief captureVideo::toggleBlur
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the blurring of the image
 */
void captureVideo::toggleBlur(bool state) {
    this->myFrame->toggleBlur(state);
    if (state)
        emit changeInfo("Blur filter activated");
    else
        emit changeInfo("Blur filter desactivated");
}

/**
 * @brief captureVideo::toggleThreshold
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the thresholding
 */
void captureVideo::toggleThreshold(bool state) {
    this->myFrame->toggleThreshold(state);
    if (state)
        emit changeInfo("Threshold activated");
    else
        emit changeInfo("Threshold desactivated");
}

/**
 * @brief captureVideo::toggleEdge
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the edge detection algorithm
 */
void captureVideo::toggleEdge(bool state) {
    this->myFrame->toggleEdge(state);
    if (state)
        emit changeInfo("Edge detection activated");
    else
        emit changeInfo("Edge detection desactivated");
}

/**
 * @brief captureVideo::toggleMotionDetection
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the motion detection algorithm
 */
void captureVideo::toggleMotionDetection(bool state) {
    this->myFrame->toggleMotionDetection(state);
    this->motion_active = state;
    if (state)
        emit changeInfo("Motion detection activated");
    else
        emit changeInfo("Motion detection desactivated");
}

#ifdef withobjdetect
/**
 * @brief captureVideo::toggleFaceDetection
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the face detection algorithm. A background image is fetch from a local directory
 */
void captureVideo::toggleFaceDetection(bool state) {
/*
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
*/
    this->myFrame->toggleFace_Recon(state);
    if (state)
        emit changeInfo("Face detection activated");
    else
        emit changeInfo("Face detection desactivated");
}
#endif

/**
 * @brief captureVideo::toggleObjectDetection
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the object detection algorithm for lines, circles and points
 */
void captureVideo::toggleObjectDetection(bool state) {
    this->myFrame->toggleObjectDetection(state);
    this->objects_active = state;
    if (state)
        emit changeInfo("Point/line/circle detection activated");
    else
        emit changeInfo("Point/line/circle detection desactivated");
}

#ifdef withzbar
/**
 * @brief captureVideo::toggleQRcode
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the ZBar decoding of QR codes and barcodes
 */
void captureVideo::toggleQRcode(bool state){
    this->myFrame->toggleQRcode(state);
    this->qrdecoder_active = state;
    if (state)
        emit changeInfo("QR code detection activated");
    else
        emit changeInfo("QR code detection desactivated");
}
#endif

/**
 * @brief captureVideo::toggleTransformation
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the geometrical transformations (rotations, etc.)
 */
void captureVideo::toggleTransformation(bool state){
    this->myFrame->toggleTransformation(state);
    if (state)
        emit changeInfo("Transformations activated");
    else
        emit changeInfo("Transformations desactivated");
}

/**
 * @brief captureVideo::toggleHistogramEqualization
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the histogram equalization + show histogram
 */
void captureVideo::toggleHistogramEqualization(bool state){
    this->myFrame->toggleHistoEq(state);
    if (state)
        emit changeInfo("Histogram equalization activated");
    else
        emit changeInfo("Histogram equalization desactivated");
}

#ifdef withstitching
/**
 * @brief captureVideo::togglePanorama
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the image stitching operation
 */
void captureVideo::togglePanorama(bool state){
    this->myFrame->togglePanorama(state);
    this->panorama_active = state;
    if (state)
        emit changeInfo("Panorama creation activated");
    else
        emit changeInfo("Panorama creation desactivated");
}
#endif

/**
 * @brief captureVideo::togglePhoto
 * @param state: boolean ON/OFF
 *
 * Activate/desactivate the photo module
 */
void captureVideo::togglePhoto(bool state) {
    this->myFrame->togglePhoto(state);
    if (state)
        emit changeInfo("Module Photo activated");
    else
        emit changeInfo("Module Photo desactivated");
}

//+++++++++++++++++++++++++++++++++++++++++++++ FUNCTIONS THAT CHANGE VALUES
// Their purposes are pretty explicit
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
    cv::Mat imageMat = this->myFrame->get_image_panorama();
    int num_imgs = this->myFrame->panorama_get_size();
    emit panoramaNumberImages(num_imgs);
    emit panoramaInfo(QString::fromStdString(return_status));

    this->data_lock->lock();
    this->panorama_Qimage = QImage(imageMat.data, imageMat.cols, imageMat.rows, imageMat.cols*3, QImage::Format_RGB888);
    this->data_lock->unlock();

    emit panoramaCaptured(&this->panorama_Qimage);
}

/**
 * @brief captureVideo::panorama_save
 *
 * Save the resulting panorama to a local file
 */
void captureVideo::panorama_save(){
    cv::Mat imageMat = this->myFrame->get_image_panorama();

    QString QfileNameLocal = QFileDialog::getSaveFileName(this->mainWindowParent,
                                                         tr("File name to save the panorama"),
                                                         QString::fromStdString(this->main_directory),
                                                         tr("Images (*.png *.jpg)") );
    if (QfileNameLocal.isEmpty()) // If one clicked the cancel button, the string is empty
        return;

    this->file_name_save = QfileNameLocal.toStdString();

    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(4);

    cv::Mat imageOutput;
    cv::cvtColor(imageMat, imageOutput, cv::COLOR_BGR2RGB);
    cv::imwrite(this->file_name_save , imageOutput , compression_params );
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

    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(4);

//    Mat imageMat = myFrame->get_image_content();
//    Mat imageOutput;
//    cvtColor(imageMat, imageOutput, COLOR_BGR2RGB);
    cv::imwrite(this->file_name_save , this->currentMat , compression_params );

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
                return false; // return false so that the button "Record" is desactivated in the mainwindow
            }

            this->video_out_name = QfileNameLocal.toStdString();

            double local_fps;
            if (this->cameraFPScalculated)
                local_fps = this->cameraFPS ;
            else
                local_fps = 20;
            this->video_out.open(this->video_out_name,cv::VideoWriter::fourcc('X','V','I','D'),
                                 local_fps,
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

#ifdef withtesseract
/**
     * @brief detectTextAreas
     * @param areas
     * @return 
     */
QImage captureVideo::detectTextAreas(std::vector<QRect> &areas, bool detectArea){
    std::vector<cv::Rect> areas_cv ;
    cv::Mat frame_cv = this->myFrame->textAreasDetect(areas_cv, detectArea);
    
    // Convert cv::Rect to QRect
    for (int i=0; i<areas_cv.size(); i++) {
        cv::Rect cur = areas_cv.at(i);
        areas.push_back( QRect(cur.y , cur.x , cur.height , cur.width) ) ;// int left, int top, int width, int height
    }
    
    // I don't know why I don't have to convert the colours this time
//    if (!frame_cv.empty())
//        cv::cvtColor(frame_cv, frame_cv, cv::COLOR_BGR2RGB);
    QImage frame = QImage(frame_cv.data, frame_cv.cols, frame_cv.rows, frame_cv.cols*3, QImage::Format_RGB888);
    
    return frame;
}
#endif // endif withtesseract

#ifdef withobjdetect
/**
 * @brief captureVideo::setCascadeFile
 *
 * Set the cascade file required by OpenCV. Active only if face detection is required in the .pro file.
 */
bool captureVideo::setCascadeFile(){
    if (this->mainWindowParent != nullptr) {
        bool testCascade = this->myFrame->set_Face_Cascade_Name(this->file_cascade);
        if (!testCascade){
            QString QfileNameLocal = QFileDialog::getOpenFileName(this->mainWindowParent,
                                                             tr("Select the face cascade file haarcascade_frontalface_default.xml"),
                                                             QString::fromStdString(this->main_directory),
                                                             tr("Images (*.xml)") );
            if ( ! QfileNameLocal.isEmpty() ) {
                this->file_cascade = QfileNameLocal.toStdString() ;
                testCascade = this->myFrame->set_Face_Cascade_Name(this->file_cascade);
            }
            else
                return false;

        }
    }
    else {
        qDebug() << "captureVideo::setCascadeFile() : mainWindowParent is not set";
        qDebug() << "                                 call the function captureVideo::setParent(QMainWindow *parent)";
        return false;
    }
    return true;
}
#endif // endif withobjdetect

/**
 * @brief captureVideo::loadOrnaments
 * 
 * Load the ornaments from the Resources / images.qrc and send them to myFrame
 */
bool captureVideo::loadOrnaments(){
    QImage image;
    image.load(":/images/glasses.jpg");
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat glasses = cv::Mat(image.height(), image.width(), CV_8UC3,
                              image.bits(), image.bytesPerLine()).clone();

    image.load(":/images/mustache.jpg");
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mustache = cv::Mat(image.height(), image.width(), CV_8UC3,
                               image.bits(), image.bytesPerLine()).clone();
    
    image.load(":/images/mouse-nose.jpg");
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mouse_nose = cv::Mat(image.height(), image.width(), CV_8UC3,
                                 image.bits(), image.bytesPerLine()).clone();

    image.load(":/images/cartoon_background.jpg");
    image = image.convertToFormat(QImage::Format_RGB888);
    QImage swapped = image.rgbSwapped() ;
    cv::Mat background = cv::Mat(swapped.height(),swapped.width(),CV_8UC3, const_cast<uchar*>(swapped.bits()), static_cast<size_t>(swapped.bytesPerLine()) ).clone() ;

    std::vector<cv::Mat> Mat2send;
    if (!glasses.empty() && !mustache.empty() && !mouse_nose.empty() && !background.empty()){
        Mat2send.push_back(glasses);
        Mat2send.push_back(mustache);
        Mat2send.push_back(mouse_nose);
        Mat2send.push_back(background);
        this->myFrame->loadOrnaments(Mat2send);
    }
    else {
        std::cerr << " captureVideo::loadOrnaments(): couln't load all the images from the resource"<< std::endl;
        return false;
    }
    return true;
}

#ifdef withface
/**
 * @brief captureVideo::setFacemarkFile
 *
 * Set the Facemark file lbfmodel.yaml required by OpenCV. Active only if face detection is required in the .pro file.
 */
bool captureVideo::setFacemarkFile(){
    if (this->mainWindowParent != nullptr) {
        bool testCascade = this->myFrame->set_Face_Facemark_Name(this->file_facemark);
        while (!testCascade){
            QString QfileNameLocal = QFileDialog::getOpenFileName(this->mainWindowParent,
                                                             tr("Select the facemark file lbfmodel.yaml"),
                                                             QString::fromStdString(this->main_directory),
                                                             tr("Images (*.yaml)") );
            if ( ! QfileNameLocal.isEmpty() ) {
                this->file_facemark = QfileNameLocal.toStdString() ;
                testCascade = this->myFrame->set_Face_Cascade_Name(this->file_facemark);
            }
            else
                return false;

        }
    }
    else {
        qDebug() << "captureVideo::setCascadeFile() : mainWindowParent is not set";
        qDebug() << "                                 call the function captureVideo::setParent(QMainWindow *parent)";
        return false;
    }
    return true;
}
#endif // endif withface
