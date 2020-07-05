/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *  The OpenCV class that contains the opencv images and all the operations for image treatment.
 *  No Qt thing here -> it is completely independent from Qt and focusses on image treatment.
 *  Receives signals from the OpenCV thread and perform corresponding image treatment.
*/

#include "myimage.h"

/**
 * @brief MyImage::MyImage
 * 
 * Constructor of the class MyImage. Initialization of all the parameters to default values.
 */
MyImage::MyImage()
{
    // Initializations for the all image operations
#ifdef withobjdetect
    this->face_cascade_name = "";
#endif

    this->blurred        = false;
    this->blur_range     = 3;
    this->blur_method    = 1;
    this->morpho_element = 1; // element = [1] rectangle, [2] cross or [3] ellipse

    this->edge_detect     = false;
    this->edge_method     = 1;
    this->canny_threshold = 50;
    this->canny_ratio     = 2.5;

    this->thresholded         = false;
    this->threshold_method    = 1;
    this->threshold_value     = 127;
    this->threshold_blocksize = 3; // for adaptive methods
    this->threshold_type      = 1; // 1 Normal, 0 inverted

    this->transformed           = false;
    this->transformation_method = 1;
    this->transf_rotation_value = 0;

    this->coloured    = true;
    this->inversed    = false;
    this->face_recon  = false;

    this->histo_eq         = false;
    this->histo_eq_method  = 1;
    this->histo_tiles      = 8; // for CLAHE
    this->histo_clip_limit = 2; // for CLAHE

    this->object_detected         = false;
    this->object_detection_method = 1;
    this->hough_line_threshold    = 100;

#ifdef withstitching
    this->panorama_activated = false;
#endif
    
#ifdef withzbar
    this->qrcodeactivated = false;
#endif

    this->motion_detected              = false;
    this->motion_detection_method      = 1;
    this->motion_background_first_time = true;
    
    this->photoed      = false;
    this->photo_method = 1;
    this->photo_sigmar = 0.15;
    this->photo_sigmas = 50;
    
    
//    std::string model = TESSERACT_DNN;
    cv::String model = TESSERACT_DNN ;
//    cv::String model = "/Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.3.0/install/share/opencv4/dnn/frozen_east_text_detection.pb";
    // Load DNN network.
    if (this->net.empty()) {
        this->net = cv::dnn::readNet(model);
        
        std::cout << "MyImage::MyImage(): DNN model loaded" << std::endl;
        
//        this->net = cv::dnn::readNetFromTensorflow(model);
    }
    
}

/**
 * @brief MyImage::toggleBW
 * @param state boolean the new state of the filter
 * 
 * Toggles the black/white filter
 */
void MyImage::toggleBW(bool state) {
    this->coloured = !state;
}

/**
 * @brief MyImage::toggleInverse
 * @param state boolean the new state of the filter
 * 
 * Toggles the Inverse colours filter
 */
void MyImage::toggleInverse(bool state) {
    this->inversed = state;
}

/**
 * @brief MyImage::toggleBlur
 * @param state boolean the new state of the filter
 * 
 * Toggles the blur filter
 */
void MyImage::toggleBlur(bool state) {
    this->blurred = state;
}

/**
 * @brief MyImage::toggleEdge
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the edge detection algorithm
 */
void MyImage::toggleEdge(bool state) {
    this->edge_detect = state;
}

/**
 * @brief MyImage::toggleThreshold
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the thresholding
 */
void MyImage::toggleThreshold(bool state) {
    this->thresholded = state;
}

/**
 * @brief MyImage::toggleTransformation
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the geometrical transformations
 */
void MyImage::toggleTransformation(bool state) {
    this->transformed = state;
}

#ifdef withobjdetect
/**
 * @brief MyImage::toggleFace_Recon
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the face detection algorithm
 */
void MyImage::toggleFace_Recon(bool state) {
    this->face_recon = state;
}

/**
 * @brief MyImage::getFace_Status
 * @return true if face detection algorithm is activated. False otherwise
 */
bool MyImage::getFace_Status(){
    return this->face_recon;
}
#endif // endif withobjdetect

/**
 * @brief MyImage::loadOrnaments
 * @param Mat2receive vector<cv::Mat> vector of OpenCV matrices containing the ornamental pictures
 * 
 * Receives the ornamental pictures from the database images.qrc, they have been loaded by the OpenCV thread
 */
void MyImage::loadOrnaments(std::vector<cv::Mat> Mat2receive){
    assert ( Mat2receive.size() >2 );
    this->ornament_glasses    = Mat2receive.at(0) ;
    this->ornament_mustache   = Mat2receive.at(1);
    this->ornament_mouse_nose = Mat2receive.at(2) ;
    this->background          = Mat2receive.at(3) ; //cv::bitwise_not(this->background, this->background);
}

/**
 * @brief MyImage::toggleHistoEq
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the histogram equalization algorithm
 */
void MyImage::toggleHistoEq(bool state) {
    this->histo_eq = state;
}

/**
 * @brief MyImage::toggleObjectDetection
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the object detection algorithm
 */
void MyImage::toggleObjectDetection(bool state) {
    this->object_detected = state;
}

#ifdef withstitching
/**
 * @brief MyImage::togglePanorama
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the image stitching algorithm
 */
void MyImage::togglePanorama(bool state) {
    this->panorama_activated = state;
}
#endif

/**
 * @brief MyImage::toggleMotionDetection
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the motion detection algorithm
 */
void MyImage::toggleMotionDetection(bool state) {
    this->motion_detected = state;
    if (this->motion_detected)
        this->motion_background_first_time = true;
}

/**
 * @brief MyImage::togglePhoto
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the module photo
 */
void MyImage::togglePhoto(bool state) {
    this->photoed = state;
}

#ifdef withzbar
/**
 * @brief MyImage::toggleQRcode
 * @param state boolean the new state of the algorithm
 * 
 * Toggles the detection of QR codes / barcodes
 */
void MyImage::toggleQRcode(bool state){
    this->qrcodeactivated = state;
    if (!this->qrcodeactivated) {
        this->qrcodedata.clear();
        this->qrcodetype.clear();
    }
}
#endif

/**
 * @brief MyImage::set_image_content
 * @param content: Mat received from the camera
 * 
 * Receives a new image from the camera and applies algorithms on it, depending on the activated modules
 */
void MyImage::set_image_content(cv::Mat &content) {
    // Save the old image for motion detection
    if (this->motion_detected)
        this->previmage = this->image;

    this->image = content;

#ifdef withobjdetect
    if (this->face_recon) {
        // Look for faces before any operation
        detect_faces() ;

        // this->mask contains the region of interest
/*        this->image.copyTo( this->smoothed , this->mask ); // copy the region of interest to this->smoothed
        smoothImage(this->smoothed, 31, 1);                // smooth this region
        this->smoothed.copyTo(this->image , this->mask);   // replace the region in the original image by the smoothed one
*/      
        if (this->background.size() != this->image.size())
                    cv::resize(this->background, this->background, this->image.size(), 0, 0, cv::INTER_CUBIC);
        
        this->background.copyTo(this->image, this->mask);
    }
#endif

    if (!(this->coloured))
        toBlackandWhite();
    
    if (this->photoed)
        modulephoto();
    
    if (this->inversed)
        inverseImage();

    if (this->histo_eq)
        equalizeHistogram();

    if (this->edge_detect)
        detectEdges();

    if (this->blurred)
        smoothImage(this->image, this->blur_range, this->blur_method);

    if (this->thresholded)
        thresholdImage();

    if (this->transformed)
        transformImage();
    
#ifdef withzbar
    if (this->qrcodeactivated)
        getQRcode();
#endif
    
    this->image2export = this->image;
}

/**
 * @brief MyImage::set_size_blur
 * @param value: integer the new value of the blur range
 */
void MyImage::set_size_blur(int value) {
    assert(value>0);
    this->blur_range = value;
}

/**
 * @brief MyImage::set_blur_method
 * @param method: integer the new method selected for blurring
 */
void MyImage::set_blur_method(int method) {
    assert (method>0);
    this->blur_method = method;
}

/**
 * @brief MyImage::set_morpho_element
 * @param element: integer the new morphological element type for blurring
 */
void MyImage::set_morpho_element(int element) {
    assert(element>0);
    this->morpho_element = element;
}

/**
 * @brief MyImage::set_edge_method
 * @param method: integer the new method for edge detection
 */
void MyImage::set_edge_method(int method) {
    assert (method>0);
    this->edge_method = method;
}

/**
 * @brief MyImage::set_threshold_method
 * @param method: integer the new method for thresholding
 */
void MyImage::set_threshold_method(int method) {
    assert (method>0);
    this->threshold_method = method;
}

/**
 * @brief MyImage::set_threshold_type
 * @param type: integer the type (normal/inverted) for adaptive threshold methods 
 */
void MyImage::set_threshold_type(int type) {
    assert (type==0 || type ==1);
    this->threshold_type = type;
}

/**
 * @brief MyImage::set_threshold_value
 * @param value: integer the new threshold value
 */
void MyImage::set_threshold_value(int value) {
    assert (value>=0);
    assert (value<=255);
    this->threshold_value = value;
}

/**
 * @brief MyImage::set_threshold_blocksize
 * @param value: integer the new value for blocksize for adaptive threshold methods
 */
void MyImage::set_threshold_blocksize(int value) {
    assert (value>0);
    this->threshold_blocksize = value;
}

/**
 * @brief MyImage::set_transf_method
 * @param method: integer the new method for geometrical transformations
 */
void MyImage::set_transf_method(int method) {
    assert (method>0);
    this->transformation_method = method;
}

/**
 * @brief MyImage::set_transf_rotation_value
 * @param value: integer the new value for the rotation of the image
 */
void MyImage::set_transf_rotation_value(int value) {
    this->transf_rotation_value = value;
}

/**
 * @brief MyImage::set_canny_threshold
 * @param value: integer the new value for the Canny low threshold (edge detector)
 */
void MyImage::set_canny_threshold(int value) {
    assert (value>=0);
    assert (value<=255);
    this->canny_threshold = value;
}

/**
 * @brief MyImage::set_canny_ratio
 * @param value: double the new value for the Canny ratio value (edge detector)
 */
void MyImage::set_canny_ratio(double value) {
    assert (value>0.);
    this->canny_ratio = value;
}

/**
 * @brief MyImage::set_histo_eq_method
 * @param method: integer the new method for histogram equalization
 */
void MyImage::set_histo_eq_method(int method) {
    assert (method>0);
    this->histo_eq_method = method;
}

/**
 * @brief MyImage::set_histo_eq_tiles
 * @param value: integer the new value for the number of tiles (CLAHE) 
 */
void MyImage::set_histo_eq_tiles(int value) {
    assert (value>0);
    this->histo_tiles = value;
}

/**
 * @brief MyImage::set_histo_eq_clip_limit
 * @param value: integer the new value for the clip limit (CLAHE)
 */
void MyImage::set_histo_eq_clip_limit(int value) {
    assert (value>0);
    this->histo_clip_limit = value;
}

/**
 * @brief MyImage::set_object_detection_method
 * @param method: integer the new method for object detection
 */
void MyImage::set_object_detection_method(int method) {
    assert (method>0);
    this->object_detection_method = method;
}

/**
 * @brief MyImage::set_hough_line_threshold
 * @param value: integer the new value for Hough line transform threshold
 */
void MyImage::set_hough_line_threshold(int value) {
    assert (value>=0);
    this->hough_line_threshold = value;
}

/**
 * @brief MyImage::set_motion_detection_method
 * @param method: integer the new method for motion detection
 */
void MyImage::set_motion_detection_method(int method) {
    assert (method>0);
    this->motion_detection_method = method;
    this->motion_background_first_time = true;
}

/**
 * @brief MyImage::set_photo_method
 * @param method: integer the new method for module photo
 */
void MyImage::set_photo_method(int method){
    assert(method>0);
    this->photo_method = method;
}

/**
 * @brief MyImage::set_photo_sigmas
 * @param value: integer the new value for Sigma S (module photo, NPR)
 */
void MyImage::set_photo_sigmas(int value){
    assert(value >= 0) ;
    assert(value <= 200) ;
    this->photo_sigmas = value;
}

/**
 * @brief MyImage::set_photo_sigmar
 * @param value: double the new value for Sigma R (module photo, NPR)
 */
void MyImage::set_photo_sigmar(double value){
    assert(value >= 0.) ;
    assert(value <= 1.) ;
    this->photo_sigmar = value;
}

/**
 * @brief MyImage::get_image_content
 * @return The cv::Mat containing the processed image
 */
cv::Mat& MyImage::get_image_content() {
    return this->image2export ;
}

/**
 * @brief MyImage::get_image_histogram
 * @return The cv::Mat containing the histogram of the processed image
 */
cv::Mat& MyImage::get_image_histogram() {
    std::vector<cv::Mat> bgr_planes;
    cv::split( this->image, bgr_planes );

    int histSize = 256;
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;
    cv::calcHist( &bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    cv::calcHist( &bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    cv::calcHist( &bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histograms for B, G and R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );

    this->histogram = cv::Mat( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

    cv::normalize(b_hist, b_hist, 0, this->histogram.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    cv::normalize(g_hist, g_hist, 0, this->histogram.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    cv::normalize(r_hist, r_hist, 0, this->histogram.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    for( int i = 1; i < histSize; i++ )
    {
        cv::line( this->histogram, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                                   cv::Point( bin_w*(i),   hist_h - cvRound(b_hist.at<float>(i)) ),
                                   cv::Scalar( 255, 0, 0), 2, 8, 0  );
        cv::line( this->histogram, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                                   cv::Point( bin_w*(i),   hist_h - cvRound(g_hist.at<float>(i)) ),
                                   cv::Scalar( 0, 255, 0), 2, 8, 0  );
        cv::line( this->histogram, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                                   cv::Point( bin_w*(i),   hist_h - cvRound(r_hist.at<float>(i)) ),
                                   cv::Scalar( 0, 0, 255), 2, 8, 0  );
    }
    return this->histogram;
}

#ifdef withstitching
/**
 * @brief MyImage::get_image_panorama
 * @return The cv::Mat containing the result of image stitching
 */
cv::Mat& MyImage::get_image_panorama() {
    return this->panorama;
}
#endif

/**
 * @brief MyImage::get_object_detected
 * @return The cv::Mat containing the detected objects (lines, circles, points)
 */
cv::Mat& MyImage::get_object_detected() {
    this->objects = cv::Mat::zeros( this->image.size(), CV_8UC3 );
    cv::Mat objectsBW = cv::Mat::zeros( this->image.size(), CV_8UC1 );

    switch (this->object_detection_method) {
        case 1: { // Hough line transform
            cv::Canny(this->image, objectsBW, 50, 200, 3);
            cv::cvtColor(objectsBW, this->objects, cv::COLOR_GRAY2BGR);
            std::vector<cv::Vec2f> lines;
            cv::HoughLines(objectsBW, lines, 1, CV_PI/180, this->hough_line_threshold, 0, 0 );
            for( size_t i = 0; i < lines.size(); i++ )
            {
                float rho = lines[i][0], theta = lines[i][1];
                cv::Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                cv::line( this->objects, pt1, pt2, cv::Scalar(255,0,0), 3, cv::LINE_AA);
            }
            break;
        }
        case 2: { // Hough circle transform
            cv::cvtColor(this->image, objectsBW, cv::COLOR_RGB2GRAY);
            cv::cvtColor(objectsBW, this->objects, cv::COLOR_GRAY2BGR);
            cv::GaussianBlur(objectsBW, objectsBW, cv::Size(9, 9), 2, 2 );
            std::vector<cv::Vec3f> circles;
            cv::HoughCircles(objectsBW, circles, cv::HOUGH_GRADIENT, 1, 10, 100, this->hough_line_threshold, 5);
            for(size_t i = 0; i < circles.size(); i++) {
                cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                int radius = cvRound(circles[i][2]);
                // draw the circle center
                cv::circle(this->objects, center, 3, cv::Scalar(0, 255, 0), -1);
                // draw the circle outline
                cv::circle(this->objects, center, radius, cv::Scalar(255, 0, 0), 3, 8, 0);
            }
            break;
        }
        case 3: { // Harris corner detection
            int blockSize = 2;
            int apertureSize = 3;
            double k = 0.04;
            cv::Mat dst = cv::Mat::zeros( this->image.size(), CV_32FC1 );

            cv::cvtColor(this->image, objectsBW, cv::COLOR_RGB2GRAY);
            cv::cvtColor(objectsBW, this->objects, cv::COLOR_GRAY2BGR);
            cv::cornerHarris( objectsBW, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT );
            cv::normalize( dst, dst, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );

            for( int j = 0; j < dst.rows ; j++ )
                for( int i = 0; i < dst.cols; i++ )
                    if( (int) dst.at<float>(j,i) > this->hough_line_threshold )
                        cv::circle( this->objects, cv::Point( i, j ), 5,  cv::Scalar(255, 0, 0), 2, 8, 0 );
            break;
        }
        default :
            std::cerr << "MyImage::get_object_detected(): Unknown kind of object detection "<<this->object_detection_method<<std::endl ;
            break;
    }
    return this->objects;
}

/**
 * @brief MyImage::get_motion_detected
 * @return The cv::Mat containing the detected motion
 */
cv::Mat& MyImage::get_motion_detected() {
    this->motion = cv::Mat::zeros( this->image.size(), CV_8UC3 );

    switch (this->motion_detection_method) {
        case 1: { // Background extraction
            cv::Mat Mask;
            if (this->motion_background_first_time) {
                this->motion_background_first_time = false;
                int history = 50;
                double threshold = 16.;
                bool detectShadows = false;
                this->pMOG2 = cv::createBackgroundSubtractorMOG2(history,threshold,detectShadows);
           }
            pMOG2->apply(this->image, Mask); // this->motion contains the foreground mask
            if (Mask.empty()) {
                return this->motion;
            }
            
            // Remove noise and emphasize the detected motion
            cv::threshold(Mask, Mask, 25, 255, cv::THRESH_BINARY);
            int noise_size = 3;
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(noise_size, noise_size));
            cv::erode(Mask, Mask, kernel);
            kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(noise_size, noise_size));
            cv::dilate(Mask, Mask, kernel, cv::Point(-1,-1), 3);
            
            // Export the detected motion to the cv::Mat motion
            this->image.copyTo(this->motion, Mask);
            
            // Find contours 
            std::vector<std::vector<cv::Point> > contours;
            cv::findContours(Mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
            
            // Create bounding boxes for each contours and plot on main image 
            cv::Scalar color = cv::Scalar(0, 0, 255); // red
            for(size_t i = 0; i < contours.size(); i++) {
                cv::Rect rect = cv::boundingRect(contours[i]);
                cv::rectangle(this->image, rect, color, 1);
            } 
            
            break;
        }
        default: {
            std::cerr << "MyImage::get_motion_detected(): Unknown kind of motion detection "<<this->motion_detection_method<<std::endl ;
            break;
        }
    }

    return this->motion;
}

#ifdef withobjdetect
/**
 * @brief MyImage::set_Face_Cascade_Name
 * @param new_name: String containing the filename for the cascade file (face detection)
 * @return true is the file is correctly loaded
 *        false otherwise
 */
bool MyImage::set_Face_Cascade_Name(cv::String &new_name) {
    this->face_cascade_name = new_name;

    if ( ! this->face_cascade.load( this->face_cascade_name ) ){
        std::cerr << "MyImage::set_Face_Cascade_Name(): Error loading face cascade"<<std::endl;
        return false;
    }
    return true;
}

#ifdef withface
bool MyImage::set_Face_Facemark_Name(cv::String &new_name){
    this->mark_detector = cv::face::createFacemarkLBF();
    this->mark_detector->loadModel(new_name); // how to test if successfully set?
    return true;
}
#endif // endif withface

// /**
// * @brief MyImage::set_background_image
// * @param filename: String containing the name of a background to be applied during face detection
// * @return true is the file is correctly loaded
// *        false otherwise
// */
/*
bool MyImage::set_background_image(cv::String &filename){
    this->background = cv::imread(filename);
    if (!this->background.data) {
        return false;
    }
    cv::resize(this->background, this->background, this->image.size(), 0, 0, cv::INTER_CUBIC);
    return true;
}
*/

#endif // endif withobjdetect

/**
 * @brief MyImage::toBlackandWhite
 * 
 * Performs the Colour -> Black and White operation
 */
void MyImage::toBlackandWhite() {
    // Convert image to black and white with 3 channels
    cv::cvtColor(this->image,this->image,cv::COLOR_RGB2GRAY);
    cv::cvtColor(this->image,this->image,cv::COLOR_GRAY2BGR);
}

/**
 * @brief MyImage::inverseImage
 * 
 * Performs the colours inversion
 */
void MyImage::inverseImage() {
    // Inverse the content of the image
    cv::bitwise_not(this->image, this->image);
}

/**
 * @brief MyImage::smoothImage
 * @param imag: cv::Mat input image to blur
 * @param blur_range: integer the range of blurring
 * @param method: integer the method of blurring
 * 
 * Performs blurring on the input image
 */
void MyImage::smoothImage(cv::Mat &imag, int blur_range, int method) {
    switch (method) {
        case 1:
            cv::blur( imag, imag, cv::Size(blur_range,blur_range) );
            break;
        case 2:
            cv::GaussianBlur( imag, imag, cv::Size(blur_range,blur_range) , 0 , 0 );
            break;
        case 3:
            cv::medianBlur( imag, imag, blur_range );
            break;
        case 4:
            imag.copyTo( this->mask );
            cv::bilateralFilter( this->mask, imag, blur_range, blur_range*2.0 , blur_range*0.5 );
            break;
        case 5:  // cv::MorphTypes MORPH_ERODE    = 0
        case 6:  // cv::MorphTypes MORPH_DILATE   = 1
        case 7:  // cv::MorphTypes MORPH_OPEN     = 2
        case 8:  // cv::MorphTypes MORPH_CLOSE    = 3
        case 9:  // cv::MorphTypes MORPH_GRADIENT = 4
        case 10: // cv::MorphTypes MORPH_TOPHAT   = 5
        case 11: // cv::MorphTypes MORPH_BLACKHAT = 6
        {
            int op = method - 5;
            int shape = morpho_element - 1;
            cv::morphologyEx(imag, imag, op, getStructuringElement(shape, cv::Size(blur_range,blur_range)) );
            break;
        }
        case 12: // cv::MorphTypes MORPH_HITMISS  = 7
        {
            cv::cvtColor(imag,imag,cv::COLOR_RGB2GRAY); // Image must be in BW
            int op = method - 5;
            cv::Mat kernel = (cv::Mat_<int>(3, 3) <<
                0, 1, 0,
                1, -1, 1,
                0, 1, 0);
            cv::morphologyEx(imag, imag, op, kernel );
            break;
        }
        default :
            std::cerr << "MyImage::smoothImage(): Unknown kind of blur "<<method<<std::endl ;
            break;
    }
}

/**
 * @brief MyImage::detectEdges
 * 
 * Performs the edge detection
 */
void MyImage::detectEdges() {
    switch (this->edge_method) {
        case 1: { // Sobel
            cv::Mat gray,grad,grad_x,abs_grad_x,grad_y,abs_grad_y;
            smoothImage(this->image, 3, 2); // Gaussian filter
            cv::cvtColor( this->image, gray, cv::COLOR_BGR2GRAY );
            // Gradient X
            cv::Sobel( gray, grad_x, CV_16S, 1, 0, 3 );
            cv::convertScaleAbs( grad_x, abs_grad_x );
            // Gradient Y
            cv::Sobel( gray, grad_y, CV_16S, 0, 1, 3 );
            cv::convertScaleAbs( grad_y, abs_grad_y );
            // Total Gradient (approximate)
            cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
            cv::cvtColor(grad,this->image,cv::COLOR_GRAY2BGR);
            break;
            }
        case 2: { // Laplacian
            cv::Mat gray,abs_laplacian,laplacian;
            smoothImage(this->image, 3, 2); // Gaussian filter
            cv::cvtColor( this->image, gray, cv::COLOR_BGR2GRAY );
            cv::Laplacian( gray, laplacian, CV_16S, 3 );
            cv::convertScaleAbs( laplacian, abs_laplacian );
            cv::cvtColor(abs_laplacian,this->image,cv::COLOR_GRAY2BGR);
            break;
            }
        case 3: { // Canny edge detector
            cv::Mat dst,gray,detected_edges;
            smoothImage(this->image, 3, 2); // Gaussian filter
            cv::cvtColor( this->image, gray, cv::COLOR_BGR2GRAY );
            cv::Canny( gray, detected_edges, this->canny_threshold, this->canny_threshold*this->canny_ratio, 3 );
            dst = cv::Scalar::all(0);
            this->image.copyTo(dst,detected_edges);
            dst.copyTo(this->image);
            break;
            }
        default :
            std::cerr << "MyImage::detectEdges(): Unknown kind of edge detection "<<this->edge_method<<std::endl ;
            break;
        }
}

#ifdef withobjdetect
/**
 * @brief MyImage::detect_faces
 * 
 * Performs the face detection
 */
void MyImage::detect_faces() {
    this->image.copyTo(this->mask);
    this->mask.setTo(cv::Scalar(255,255,255));

    std::vector<cv::Rect> faces;
    std::vector<cv::Point> ROI_Poly;

    // Detect faces
    face_cascade.detectMultiScale( this->image, faces, 1.1, 2, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30) );

    // Draw ellipses around faces and get regions of interest
    for( size_t i = 0; i < faces.size(); i++ )
    {
        cv::Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        //ellipse( image, center, Size( faces[i].width*0.6, faces[i].height*0.8), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

        // Approximate ellipse by a polygon
        cv::ellipse2Poly(center, cv::Size( faces[i].width*0.6, faces[i].height*0.8), 0, 0, 360, 10, ROI_Poly) ;
        // fill the polygon by zeros in the mask
        cv::fillConvexPoly(this->mask, &ROI_Poly[0], ROI_Poly.size(), cv::Scalar( 0, 0, 0 ), 4, 0);

#ifdef withface
        // Detect the facial land marks
        std::vector< std::vector<cv::Point2f> > shapes;
        cv::Scalar color = cv::Scalar(0, 0, 255); // red
        
        if (mark_detector->fit(this->image, faces, shapes)) {
            // draw facial land marks
            for (unsigned long i=0; i<faces.size(); i++) {
                if ( shapes.at(i).size() == 68 ) {
                    
/*                for(unsigned long k=0; k<shapes[i].size(); k++) {
                        cv::circle(this->image, shapes[i][k], 2, color, cv::FILLED);
                        
    //                    String index = std::to_string(k);
    //                    cv::putText(this->image, index, shapes[i][k], cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 2); 
    
                    }
*/
                    this->draw_glasses (this->image, shapes[i]);
                    this->draw_mustache(this->image, shapes[i]);
                    this->draw_mouse_nose(this->image, shapes[i]);
                }
            }
        }
#endif
    
    }
}

#ifdef withface
/**
 * @brief MyImage::draw_glasses
 * @param frame the cv::Mat on which the glasses will be displayed
 * @param marks facial marks detected by OpenCV mark_detector->fit()
 * 
 * Draw glasses on the detected face. Activated only if the optional library libopencv_face is available
 * Glasses correspond to marks 36 (left most eye point) to 45 (right most eye point)
 */
void MyImage::draw_glasses(cv::Mat &frame, std::vector<cv::Point2f> &marks){
    // resize the glasses to match the face
    cv::Mat ornament;
    double distance = cv::norm(marks[45] - marks[36]) * 1.5;
    cv::resize(this->ornament_glasses, ornament, cv::Size(0, 0), distance / this->ornament_glasses.cols, 
               distance / this->ornament_glasses.cols, cv::INTER_NEAREST);
    
    // rotate the glasses to match the angle of the face
    double angle = -atan((marks[45].y - marks[36].y) / (marks[45].x - marks[36].x));
    cv::Point2f center = cv::Point(ornament.cols/2, ornament.rows/2); 
    cv::Mat rotateMatrix = cv::getRotationMatrix2D(center, angle * 180 / 3.14, 1.0);
    cv::Mat rotated;
    cv::warpAffine(ornament, rotated, rotateMatrix, ornament.size(),
                   cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
    
    // paint the glasses on the face
    center = cv::Point((marks[45].x + marks[36].x) / 2, (marks[45].y + marks[36].y) / 2);
    cv::Rect rec(center.x - rotated.cols / 2, center.y - rotated.rows / 2, rotated.cols, rotated.rows);
    
    // Test if the ROI can be placed on the frame
    bool test1 = 0 <= rec.x; 
    bool test2 = 0 <= rec.width;
    bool test3 = (rec.x + rec.width) <= frame.cols ;
    bool test4 = 0 <= rec.y ;
    bool test5 = 0 <= rec.height ;
    bool test6 = (rec.y + rec.height) <= frame.rows ;
    if (test1 && test2 && test3 && test4 && test4 && test5 && test6)
        frame(rec) &= rotated; 
}

/**
 * @brief MyImage::draw_mustache
 * @param frame the cv::Mat on which the mustache will be displayed
 * @param marks facial marks detected by OpenCV mark_detector->fit()
 * 
 * Draw a mustache on the detected face. Activated only if the optional library libopencv_face is available 
 * The mouth corresponds to marks 48 (left most mouth point) to 54 (right most mouth point)
 * The mustache must be positioned between the nose tip (mark 33) and the center of the mouth (mark 51)
 */
void MyImage::draw_mustache(cv::Mat &frame, std::vector<cv::Point2f> &marks){
    // resize the glasses to match the face
    cv::Mat ornament;
    double distance = cv::norm(marks[54] - marks[48]) * 1.5;
    cv::resize(this->ornament_mustache, ornament, cv::Size(0, 0), distance / this->ornament_mustache.cols, 
               distance / this->ornament_mustache.cols, cv::INTER_NEAREST);
    
    // rotate the mustache to match the angle of the face
    double angle = -atan((marks[54].y - marks[48].y) / (marks[54].x - marks[48].x));
    cv::Point2f center = cv::Point(ornament.cols/2, ornament.rows/2); 
    cv::Mat rotateMatrix = cv::getRotationMatrix2D(center, angle * 180 / 3.14, 1.0);
    cv::Mat rotated;
    cv::warpAffine(ornament, rotated, rotateMatrix, ornament.size(),
                   cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
    
    // paint the mustache on the face, to be placed between nose and mouth
    center = cv::Point((marks[33].x + marks[51].x) / 2, (marks[33].y + marks[51].y) / 2);
    cv::Rect rec(center.x - rotated.cols / 2, center.y - rotated.rows / 2, rotated.cols, rotated.rows);
    
    // Test if the ROI can be placed on the frame
    bool test1 = 0 <= rec.x; 
    bool test2 = 0 <= rec.width;
    bool test3 = (rec.x + rec.width) <= frame.cols ;
    bool test4 = 0 <= rec.y ;
    bool test5 = 0 <= rec.height ;
    bool test6 = (rec.y + rec.height) <= frame.rows ;
    if (test1 && test2 && test3 && test4 && test4 && test5 && test6)
        frame(rec) &= rotated; 
}

/**
 * @brief MyImage::draw_mouse_nose
 * @param frame the cv::Mat on which the mouse nose will be displayed
 * @param marks facial marks detected by OpenCV mark_detector->fit()
 * 
 * Draw a mouse nose on the detected face. Activated only if the optional library libopencv_face is available
 * The nose corresponds to mark 30 and marks 3-13 + 0-16 allows the computation of the size + rotation angle
 */
void MyImage::draw_mouse_nose(cv::Mat &frame, std::vector<cv::Point2f> &marks) {
    // resize the glasses to match the face
    cv::Mat ornament;
    double distance = cv::norm(marks[13] - marks[3]) * 1.5;
    cv::resize(this->ornament_mouse_nose, ornament, cv::Size(0, 0), distance / this->ornament_mouse_nose.cols, 
               distance / this->ornament_mouse_nose.cols, cv::INTER_NEAREST);
    
    // rotate the mustache to match the angle of the face
    double angle = -atan((marks[16].y - marks[0].y) / (marks[16].x - marks[0].x));
    cv::Point2f center = cv::Point(ornament.cols/2, ornament.rows/2); 
    cv::Mat rotateMatrix = cv::getRotationMatrix2D(center, angle * 180 / 3.14, 1.0);
    cv::Mat rotated;
    cv::warpAffine(ornament, rotated, rotateMatrix, ornament.size(),
                   cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
    
    // paint the mustache on the face, to be placed between nose and mouth
    center = cv::Point(marks[30].x , marks[30].y );
    cv::Rect rec(center.x - rotated.cols / 2, center.y - rotated.rows / 2, rotated.cols, rotated.rows);
    
    // Test if the ROI can be placed on the frame
    bool test1 = 0 <= rec.x; 
    bool test2 = 0 <= rec.width;
    bool test3 = (rec.x + rec.width) <= frame.cols ;
    bool test4 = 0 <= rec.y ;
    bool test5 = 0 <= rec.height ;
    bool test6 = (rec.y + rec.height) <= frame.rows ;
    if (test1 && test2 && test3 && test4 && test4 && test5 && test6)
        frame(rec) &= rotated; 
}

#endif // endif withface

#endif // endif withobjdetect

/**
 * @brief MyImage::thresholdImage
 * @return the value for the thresholding
 * 
 * Performs the thresholding (global/local)
 */
double MyImage::thresholdImage() {
    double value = this->threshold_value;
    switch (this->threshold_method) {
    case 1: // cv::ThresholdTypes THRESH_BINARY       = 0
    case 2: // cv::ThresholdTypes THRESH_BINARY_INV   = 1
    case 3: // cv::ThresholdTypes THRESH_TRUNC        = 2
    case 4: // cv::ThresholdTypes THRESH_TOZERO       = 3
    case 5: { // cv::ThresholdTypes THRESH_TOZERO_INV = 4
        cv::cvtColor(this->image,this->image,cv::COLOR_RGB2GRAY);
        int type = this->threshold_method - 1 ;
        cv::threshold( this->image, this->image, this->threshold_value, 255, type );
        cv::cvtColor(this->image,this->image,cv::COLOR_GRAY2BGR);
        break;
    }
    case 6: { // cv::ThresholdTypes THRESH_OTSU = 8
        cv::cvtColor(this->image,this->image,cv::COLOR_RGB2GRAY);
        value = cv::threshold( this->image, this->image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU );
        cv::cvtColor(this->image,this->image,cv::COLOR_GRAY2BGR);
        break;
    }
    case 7: { // cv::ThresholdTypes THRESH_TRIANGLE = 16
        cv::cvtColor(this->image,this->image,cv::COLOR_RGB2GRAY);
        value = cv::threshold( this->image, this->image, 0, 255, cv::THRESH_BINARY | cv::THRESH_TRIANGLE  );
        cv::cvtColor(this->image,this->image,cv::COLOR_GRAY2BGR);
        break;
    }
    case 8: { // adaptive threshold simple mean
        cv::cvtColor(this->image,this->image,cv::COLOR_RGB2GRAY);
        if (this->threshold_type == 1)
            cv::adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY ,this->threshold_blocksize, 0. );
        else
            cv::adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV ,this->threshold_blocksize, 0. );
        cv::cvtColor(this->image,this->image,cv::COLOR_GRAY2BGR);
        break;
    }
    case 9: { // adaptive threshold Gaussian
        cv::cvtColor(this->image,this->image,cv::COLOR_RGB2GRAY);
        if (this->threshold_type == 1)
            cv::adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY ,this->threshold_blocksize, 0. );
        else
            cv::adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV ,this->threshold_blocksize, 0. );
        cv::cvtColor(this->image,this->image,cv::COLOR_GRAY2BGR);
        break;
    }
    default:
        std::cerr << "MyImage::thresholdImage(): Unknown kind of threshold "<<this->threshold_method<<std::endl ;
        break;
    }
    return value;
}

/**
 * @brief MyImage::transformImage
 * 
 * Performs the geometrical transformation
 */
void MyImage::transformImage() {
    switch (this->transformation_method) {
    case 1: { // Rotation around center of image
        int centerX = this->image.rows / 2;
        int centerY = this->image.cols / 2;

        cv::Mat matRotation = cv::getRotationMatrix2D( cv::Point(centerY, centerX), (transf_rotation_value), 1 );
        cv::warpAffine( this->image, this->image, matRotation, this->image.size() );
        break;
    }
    default:
        std::cerr << "MyImage::transformImage(): Unknown kind of transformation "<<this->transformation_method<<std::endl ;
        break;
    }

}

/**
 * @brief MyImage::equalizeHistogram
 * 
 * Performs the histogram equalization
 */
void MyImage::equalizeHistogram() {
    std::vector<cv::Mat> channels;
    cv::cvtColor(this->image, this->image, cv::COLOR_BGR2YCrCb); //change the color image from BGR to YCrCb format
    cv::split(this->image,channels); //split the image into channels

    switch (this->histo_eq_method) {
        case 1: {
            cv::equalizeHist(channels[0], channels[0]); //equalize histogram on the 1st channel (Y)
            break;
        }
        case 2:{
            cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
            clahe->setClipLimit(this->histo_clip_limit);
            clahe->setTilesGridSize(cv::Size(this->histo_tiles,this->histo_tiles));
            clahe->apply(channels[0],channels[0]);
            break;
        }
        default:{
            std::cerr << "MyImage::equalizeHistogram(): Unknown type of histogram operation\n";
            break;
        }
    }
    cv::merge(channels,this->image); //merge 3 channels including the modified 1st channel into one image
    cv::cvtColor(this->image, this->image, cv::COLOR_YCrCb2BGR);
}

/**
 * @brief MyImage::modulephoto
 * 
 * Applies operations from the module Photo
 */
void MyImage::modulephoto(){
    switch (this->photo_method){
        case 1:{ // Contrast Preserving Decolorization
            cv::Mat gray, color_boost;
            cv::decolor( this->image, gray, color_boost );
            this->image = color_boost;
            break;
        }
        case 2:{ // Denoising
            cv::fastNlMeansDenoisingColored(this->image,this->image,3.,3.,7,21);
            break;
        }
        case 3:{ // Non-Photorealistic Rendering: edge preserving filter
            cv::edgePreservingFilter(this->image,this->image, cv::RECURS_FILTER, this->photo_sigmas, this->photo_sigmar);
            // cv::NORMCONV_FILTER
            // cv::RECURS_FILTER
            break;
        }
        case 4:{ // Non-Photorealistic Rendering: detail enhance
            cv::detailEnhance(this->image,this->image,this->photo_sigmas, this->photo_sigmar);
            break;
        }
        case 5:{ // Non-Photorealistic Rendering: pencil sketch
            cv::Mat img1;
            pencilSketch(this->image,img1, this->image, this->photo_sigmas, this->photo_sigmar, 0.03f);
            break;
        }
        case 6:{ // Non-Photorealistic Rendering: stylization
            cv::stylization(this->image,this->image,this->photo_sigmas, this->photo_sigmar);
            break;
        }
#ifdef withxphoto
        case 7: { // xphoto white balance
            cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB(); // significant modification
            // Ptr<xphoto::WhiteBalancer> wb = xphoto::createGrayworldWB(); // not wonderful with my camera
            wb->balanceWhite(this->image, this->image);
            break;
        }
#endif
        default:{
            std::cerr << "MyImage::modulephoto(): Unknown type of photo operation "<<this->photo_method<<std::endl;
            break;
        }
    }

}

#ifdef withstitching
/**
 * @brief MyImage::panorama_insert_image
 * 
 * Insert the current frame to the stack of images for stitching
 */
void MyImage::panorama_insert_image() {
    if (!this->image.empty())
        this->Panorama_vector.push_back(this->image);
}
#endif

#ifdef withstitching
/**
 * @brief MyImage::panorama_pop_up_image
 * 
 * Removes the lastly inserted image for the stitching
 */
void MyImage::panorama_pop_up_image() {
    if (!this->Panorama_vector.empty())
        this->Panorama_vector.pop_back();
}
#endif

#ifdef withstitching
/**
 * @brief MyImage::panorama_compute_result
 * @return a string that describes the result of the stitching operation
 * 
 * Performs the image stitching operation
 */
std::string MyImage::panorama_compute_result() {
    cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA; // PANORAMA or SCANS
    std::string return_status;

    if (this->Panorama_vector.size()==1)
        this->Panorama_stitcher = cv::Stitcher::create(mode);
    cv::Stitcher::Status status = Panorama_stitcher->stitch(this->Panorama_vector, this->panorama);
    switch (status) {
    case cv::Stitcher::OK: {
        std::cout << "Stitcher normal return status " << status << std::endl;
        return_status = "Stitcher normal return status "+std::to_string(status);
        break;
    }
    case cv::Stitcher::ERR_NEED_MORE_IMGS:{
        std::cout << "Stitcher requires additionnal images " << status << std::endl;
        return_status = "Stitcher requires additionnal images "+std::to_string(status);
        break;
    }
    case cv::Stitcher::ERR_HOMOGRAPHY_EST_FAIL:{
        std::cout << "Stitcher error: homography estimation fail " << status << std::endl;
        std::cout << "   Removing the problematic image from the panorama" << std::endl;
        return_status = "Stitcher error: homography estimation fail "+std::to_string(status);
        this->Panorama_vector.pop_back();
        break;
    }
    case cv::Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL:{
        std::cout << "Stitcher error: camera parameters adjust fail " << status << std::endl;
        std::cout << "   Removing the problematic image from the panorama" << std::endl;
        return_status = "Stitcher error: camera parameters adjust fail "+std::to_string(status);
        this->Panorama_vector.pop_back();
        break;
    }
    default:{
        std::cout << "MyImage::panorama_compute_result(): Unknown type of return status from Panorama_stitcher.stitch()"<<std::endl;
        return_status = "Unknown type of return status from Panorama_stitcher.stitch()";
        break;
    }
    }
    return return_status;
}
#endif

#ifdef withstitching
/**
 * @brief MyImage::panorama_reset
 * 
 * Empties the stack of images for stitching
 */
void MyImage::panorama_reset() {
    if (!this->Panorama_vector.empty())
        this->Panorama_vector.clear();
    this->panorama = this->image;
}
#endif

#ifdef withstitching
/**
 * @brief MyImage::panorama_get_size
 * @return integer the actual number of images in the stack for stitching
 */
int MyImage::panorama_get_size(){
    return (int)this->Panorama_vector.size();
}
#endif

#ifdef withzbar
/**
 * @brief MyImage::getQRcode
 * 
 * Performs the detection and decoding of QR codes + barcodes with the help of ZBar
 */
void MyImage::getQRcode(){
    // Create zbar scanner
    zbar::ImageScanner scanner;
    // Configure scanner
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
    
    // Convert image to grayscale
    cv::Mat imGray;
    cv::cvtColor(this->image, imGray,cv::COLOR_BGR2GRAY); 
    
    // Wrap image data in a zbar image
    zbar::Image image(this->image.cols, this->image.rows, "Y800", (uchar *)imGray.data, this->image.cols * this->image.rows);
    
    // Scan the image for barcodes and QRCodes
    int nscan = scanner.scan(image);
    
    int current_code = 0;
    if (nscan>0) {
        for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
            // Print type and data
            
            this->qrcodedata = symbol->get_data();
            this->qrcodetype = symbol->get_type_name() ;
            
            std::cout << "Type : " << this->qrcodedata << std::endl;
            std::cout << "Data : " << this->qrcodetype << std::endl << std::endl;
            
            // Obtain location
            std::vector <cv::Point> location;
            for(int i = 0; i< symbol->get_location_size(); i++) {
                location.push_back(cv::Point(symbol->get_location_x(i),symbol->get_location_y(i)));
            }
            
            std::vector<cv::Point> hull;
            // If the points do not form a quad, find convex hull
            if(location.size() > 4)
                cv::convexHull(location, hull);
            else
                hull = location;
            
            // Plot the convex hull
            int n = hull.size();
            for(int j = 0; j < n; j++) {
                cv::line(this->image, hull[j], hull[ (j+1) % n], cv::Scalar(255,0,0), 3);
            }
            
            // Print the text
            cv::putText(this->image, //target image
                        this->qrcodetype, //text
                        cv::Point(10, current_code * 50 + 40 ), //top-left position
                        cv::FONT_HERSHEY_DUPLEX,
                        1.0,
                        CV_RGB(118, 185, 0), //font color
                        2);
            
            current_code++;
            cv::putText(this->image, //target image
                        this->qrcodedata, //text
                        cv::Point(10, current_code * 50 + 40 ), //top-left position
                        cv::FONT_HERSHEY_DUPLEX,
                        1.0,
                        CV_RGB(118, 185, 0), //font color
                        2);
            
            current_code++;
            
        }
    }
    
    
}

/**
 * @brief MyImage::getQRcodedata
 * @param data string the type of code decoded (QR, barcode, ISBN, etc.)
 * @param type string the content of the decoded code
 * @return true if a QR code or barcode has been decoded
 */
bool MyImage::getQRcodedata(std::string &data, std::string &type) {
    if ( ! this->qrcodedata.empty() ){
        data = this->qrcodedata ;
        type = this->qrcodetype ;
        return true;
    }
    return false;
}

#endif


#ifdef withtesseract
/**
 * @brief MyImage::detectTextAreas
 * @param areas
 * @return 
 */
cv::Mat MyImage::textAreasDetect(std::vector<cv::Rect> &areas, bool detectAreas) {
    float confThreshold = 0.5;
    float nmsThreshold = 0.4;
    int inputWidth = 320;
    int inputHeight = 320;

    std::vector<cv::Mat> outs;
    std::vector<std::string> layerNames(2);
    layerNames[0] = "feature_fusion/Conv_7/Sigmoid";
    layerNames[1] = "feature_fusion/concat_3";

    cv::Mat frame = this->image.clone();
    
    if (detectAreas) {
        cv::Mat blob;
    
        cv::dnn::blobFromImage(frame, blob,
                               1.0, cv::Size(inputWidth, inputHeight),
                               cv::Scalar(123.68, 116.78, 103.94), true, false
        );
        this->net.setInput(blob);
        this->net.forward(outs, layerNames);
    
        cv::Mat scores = outs[0];
        cv::Mat geometry = outs[1];
    
        std::vector<cv::RotatedRect> boxes;
        std::vector<float> confidences;
        textAreasdecode(scores, geometry, confThreshold, boxes, confidences);
    
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    
        // Render detections.
        cv::Point2f ratio((float)frame.cols / inputWidth, (float)frame.rows / inputHeight);
        cv::Scalar green = cv::Scalar(0, 255, 0);
    
        for (size_t i = 0; i < indices.size(); ++i) {
            cv::RotatedRect& box = boxes[indices[i]];
            cv::Rect area = box.boundingRect();
            area.x *= ratio.x;
            area.width *= ratio.x;
            area.y *= ratio.y;
            area.height *= ratio.y;
            areas.push_back(area);
            cv::rectangle(frame, area, green, 1);
            cv::String index = std::to_string(i);
            cv::putText( frame, index, cv::Point2f(area.x, area.y - 2),cv::FONT_HERSHEY_SIMPLEX, 0.5, green, 1 );
        }
    }
    return frame;
}

void MyImage::textAreasdecode(const cv::Mat& scores, const cv::Mat& geometry, float scoreThresh,
                              std::vector<cv::RotatedRect>& detections, std::vector<float>& confidences)
{
    CV_Assert(scores.dims == 4); CV_Assert(geometry.dims == 4);
    CV_Assert(scores.size[0] == 1); CV_Assert(scores.size[1] == 1);
    CV_Assert(geometry.size[0] == 1);  CV_Assert(geometry.size[1] == 5);
    CV_Assert(scores.size[2] == geometry.size[2]);
    CV_Assert(scores.size[3] == geometry.size[3]);

    detections.clear();
    const int height = scores.size[2];
    const int width = scores.size[3];
    for (int y = 0; y < height; ++y) {
        const float* scoresData = scores.ptr<float>(0, 0, y);
        const float* x0_data = geometry.ptr<float>(0, 0, y);
        const float* x1_data = geometry.ptr<float>(0, 1, y);
        const float* x2_data = geometry.ptr<float>(0, 2, y);
        const float* x3_data = geometry.ptr<float>(0, 3, y);
        const float* anglesData = geometry.ptr<float>(0, 4, y);
        for (int x = 0; x < width; ++x) {
            float score = scoresData[x];
            if (score < scoreThresh)
                continue;

            // Decode a prediction.
            // Multiple by 4 because feature maps are 4 time less than input image.
            float offsetX = x * 4.0f, offsetY = y * 4.0f;
            float angle = anglesData[x];
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            float h = x0_data[x] + x2_data[x];
            float w = x1_data[x] + x3_data[x];

            cv::Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
            cv::Point2f p1 = cv::Point2f(-sinA * h, -cosA * h) + offset;
            cv::Point2f p3 = cv::Point2f(-cosA * w, sinA * w) + offset;
            cv::RotatedRect r(0.5f * (p1 + p3), cv::Size2f(w, h), -angle * 180.0f / (float)CV_PI);
            detections.push_back(r);
            confidences.push_back(score);
        }
    }
}

#endif
