/*
 * Copyright: Xavier Dechamps
 *
 * PURPOSE
 *  The OpenCV class that contains the opencv images and all the operations for image treatment.
 *  No Qt thing here -> it is completely independent from Qt and focusses on image treatment.
 *  Receives signals from the main window and perform corresponding image treatment.
*/

#include "myimage.h"

MyImage::MyImage()
{
    // Initializations for the all image operations
    this->face_cascade_name = "";

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

    this->motion_detected              = false;
    this->motion_detection_method      = 1;
    this->motion_background_first_time = true;
}

// Set of functions called by external world to toggle given types of image treatments
void MyImage::toggleBW() {
    this->coloured = ! (this->coloured);
}

void MyImage::toggleInverse() {
    this->inversed = ! (this->inversed);
}

void MyImage::toggleBlur() {
    this->blurred = ! (this->blurred);
}

void MyImage::toggleEdge() {
    this->edge_detect = ! (this->edge_detect);
}

void MyImage::toggleThreshold() {
    this->thresholded = ! (this->thresholded);
}

void MyImage::toggleTransformation() {
    this->transformed = ! (this->transformed);
}

void MyImage::toggleFace_Recon() {
    this->face_recon = ! (this->face_recon);
}

void MyImage::toggleHistoEq() {
    this->histo_eq = ! (this->histo_eq);
}

void MyImage::toggleObjectDetection() {
    this->object_detected = ! (this->object_detected);
}

#ifdef withstitching
void MyImage::togglePanorama() {
    this->panorama_activated = ! (this->panorama_activated);
}
#endif

void MyImage::toggleMotionDetection() {
    this->motion_detected = ! (this->motion_detected);
    if (this->motion_detected)
        this->motion_background_first_time = true;
}

// Set of functions called by external world to set parameters for the image treatments
void MyImage::set_image_content(Mat &content) {
    // Receives a new image from the outside world and applies the image treatments

    // Save the old image for motion detection
    if (this->motion_detected)
        this->previmage = this->image;

    this->image = content;

    if (!(this->coloured))
        toBlackandWhite();

    if (this->inversed)
        inverseImage();

    if (this->histo_eq)
        equalizeHistogram();

    if (this->face_recon) {
        detect_faces() ;

        // this->mask contains the region of interest
        this->image.copyTo( this->smoothed , this->mask ); // copy the region of interest to this->smoothed
        smoothImage(this->smoothed, 31, 1);                // smooth this region
        this->smoothed.copyTo(this->image , this->mask);   // replace the region in the original image by the smoothed one
    }

    if (this->edge_detect)
        detectEdges();

    if (this->blurred)
        smoothImage(this->image, this->blur_range, this->blur_method);

    if (this->thresholded)
        thresholdImage();

    if (this->transformed)
        transformImage();
}

void MyImage::set_size_blur(int value) {
    assert(value>0);
    this->blur_range = value;
}

void MyImage::set_blur_method(int method) {
    assert (method>0);
    this->blur_method = method;
}

void MyImage::set_morpho_element(int element) {
    assert(element>0);
    this->morpho_element = element;
}

void MyImage::set_edge_method(int method) {
    assert (method>0);
    this->edge_method = method;
}

void MyImage::set_threshold_method(int method) {
    assert (method>0);
    this->threshold_method = method;
}

void MyImage::set_threshold_type(int type) {
    assert (type>0);
    this->threshold_type = type;
}

void MyImage::set_threshold_value(int value) {
    assert (value>=0);
    assert (value<=255);
    this->threshold_value = value;
}

void MyImage::set_threshold_blocksize(int value) {
    assert (value>0);
    this->threshold_blocksize = value;
}

void MyImage::set_transf_method(int method) {
    assert (method>0);
    this->transformation_method = method;
}

void MyImage::set_transf_rotation_value(int value) {
    this->transf_rotation_value = value;
}

void MyImage::set_canny_threshold(int value) {
    assert (value>=0);
    assert (value<=255);
    this->canny_threshold = value;
}

void MyImage::set_canny_ratio(double value) {
    assert (value>0.);
    this->canny_ratio = value;
}

void MyImage::set_histo_eq_method(int method) {
    assert (method>0);
    this->histo_eq_method = method;
}

void MyImage::set_histo_eq_tiles(int value) {
    assert (value>0);
    this->histo_tiles = value;
}

void MyImage::set_histo_eq_clip_limit(int value) {
    assert (value>0);
    this->histo_clip_limit = value;
}

void MyImage::set_object_detection_method(int method) {
    assert (method>0);
    this->object_detection_method = method;
}

void MyImage::set_hough_line_threshold(int value) {
    assert (value>=0);
    this->hough_line_threshold = value;
}

void MyImage::set_motion_detection_method(int method) {
    assert (method>0);
    this->motion_detection_method = method;
    this->motion_background_first_time = true;
}

// Set of functions called by the external world to get the content of images
Mat& MyImage::get_image_content() {
    return this->image;
}

Mat& MyImage::get_image_histogram() {
    vector<Mat> bgr_planes;
    split( this->image, bgr_planes );

    int histSize = 256;
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;

    Mat b_hist, g_hist, r_hist;
    calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histograms for B, G and R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );

    this->histogram = Mat( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    normalize(b_hist, b_hist, 0, this->histogram.rows, NORM_MINMAX, -1, Mat() );
    normalize(g_hist, g_hist, 0, this->histogram.rows, NORM_MINMAX, -1, Mat() );
    normalize(r_hist, r_hist, 0, this->histogram.rows, NORM_MINMAX, -1, Mat() );
    for( int i = 1; i < histSize; i++ )
    {
        line( this->histogram, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                               Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                               Scalar( 255, 0, 0), 2, 8, 0  );
        line( this->histogram, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                               Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                               Scalar( 0, 255, 0), 2, 8, 0  );
        line( this->histogram, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                               Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                               Scalar( 0, 0, 255), 2, 8, 0  );
    }
    return this->histogram;
}

#ifdef withstitching
Mat& MyImage::get_image_panorama() {
    return this->panorama;
}
#endif

Mat& MyImage::get_object_detected() {
    this->objects = Mat::zeros( this->image.size(), CV_8UC3 );
    Mat objectsBW = Mat::zeros( this->image.size(), CV_8UC1 );

    switch (this->object_detection_method) {
        case 1: { // Hough line transform
            Canny(this->image, objectsBW, 50, 200, 3);
            cvtColor(objectsBW, this->objects, COLOR_GRAY2BGR);
            vector<Vec2f> lines;
            HoughLines(objectsBW, lines, 1, CV_PI/180, this->hough_line_threshold, 0, 0 );
            for( size_t i = 0; i < lines.size(); i++ )
            {
                float rho = lines[i][0], theta = lines[i][1];
                Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                line( this->objects, pt1, pt2, Scalar(255,0,0), 3, LINE_AA);
            }
            break;
        }
        case 2: { // Hough circle transform
            cvtColor(this->image, objectsBW, COLOR_RGB2GRAY);
            cvtColor(objectsBW, this->objects, COLOR_GRAY2BGR);
            GaussianBlur(objectsBW, objectsBW, Size(9, 9), 2, 2 );
            vector<Vec3f> circles;
            HoughCircles(objectsBW, circles, HOUGH_GRADIENT, 1, 10, 100, this->hough_line_threshold, 5);
            for(size_t i = 0; i < circles.size(); i++) {
                Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                int radius = cvRound(circles[i][2]);
                // draw the circle center
                circle(this->objects, center, 3, Scalar(0, 255, 0), -1);
                // draw the circle outline
                circle(this->objects, center, radius, Scalar(255, 0, 0), 3, 8, 0);
            }
            break;
        }
        case 3: { // Harris corner detection
            int blockSize = 2;
            int apertureSize = 3;
            double k = 0.04;
            Mat dst = Mat::zeros( this->image.size(), CV_32FC1 );

            cvtColor(this->image, objectsBW, COLOR_RGB2GRAY);
            cvtColor(objectsBW, this->objects, COLOR_GRAY2BGR);
            cornerHarris( objectsBW, dst, blockSize, apertureSize, k, BORDER_DEFAULT );
            normalize( dst, dst, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );

            for( int j = 0; j < dst.rows ; j++ )
                for( int i = 0; i < dst.cols; i++ )
                    if( (int) dst.at<float>(j,i) > this->hough_line_threshold )
                        circle( this->objects, Point( i, j ), 5,  Scalar(255, 0, 0), 2, 8, 0 );
            break;
        }
        default :
            cout << "MyImage::get_object_detected(): Unknown kind of object detection\n" ;
            break;
    }
    return this->objects;
}

Mat& MyImage::get_motion_detected() {
    this->motion = Mat::zeros( this->image.size(), CV_8UC3 );

    switch (this->motion_detection_method) {
        case 1: { // Farneback dense optical flow
            UMat input_fback_prev, input_fback_current, uflow; // UMat
            Mat flow; // Mat

            cvtColor(this->image, input_fback_current, COLOR_BGR2GRAY);
            cvtColor(this->previmage, input_fback_prev, COLOR_BGR2GRAY);

            calcOpticalFlowFarneback(input_fback_prev, input_fback_current, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
            uflow.copyTo(flow);
            cvtColor(input_fback_prev, this->motion, COLOR_GRAY2BGR);

            // colour with amplitude / phase in HSV format
            Mat out,tmp,umag,uphase,components_flow[3];
            cv::split(flow,components_flow);
            cv::cartToPolar(components_flow[0],components_flow[1],umag,uphase, true);
            vector<Mat> channels_out;
            tmp = uphase * 0.5;
            tmp.convertTo(tmp, CV_8U);
            channels_out.push_back( tmp  ); // * 180 / 3.14159265359
            channels_out.push_back( Mat::ones(uphase.rows, uphase.cols,CV_8U)*255 );
            normalize( umag, tmp, 0,255,NORM_MINMAX );

            Mat mask = tmp < 0.4; // threshold to remove background noise
            tmp.setTo(0, mask);

            tmp.convertTo(tmp, CV_8U);
            channels_out.push_back( tmp );
            merge(channels_out, out);
            cv::cvtColor( out , this->motion , COLOR_HSV2BGR );


            // add line to get direction of motion
            int step = 8;
            for(int y = 0; y < this->motion.rows; y += step) {
                for(int x = 0; x < this->motion.cols; x += step) {
                    const Point2f& fxy = flow.at<Point2f>(y, x);

                    if (abs(fxy.x) >= 0.2 && abs(fxy.y) >=0.2) // threshold to remove background noise
                        line(this->motion, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),Scalar(0, 0, 255));
                    //circle(this->motion, Point(x,y), 2, Scalar(0, 255, 0), -1);
                }
            }

            break;
        }
        case 2: { // Foreground extraction
            Mat Mask;
            if (this->motion_background_first_time) {
                this->motion_background_first_time = false;
                this->pMOG2 = createBackgroundSubtractorMOG2();
           }
            pMOG2->apply(this->image, Mask); // this->motion contains the foreground mask

            this->image.copyTo(this->motion, Mask);
            //cvtColor(Mask, this->motion, COLOR_GRAY2BGR);
            break;
        }
        default: {
            cout << "MyImage::get_motion_detected(): Unknown kind of motion detection\n" ;
            break;
        }
    }

    return this->motion;
}

bool MyImage::set_Face_Cascade_Name(String &new_name) {
    this->face_cascade_name = new_name;

    if ( ! this->face_cascade.load( this->face_cascade_name ) ){
        cout << "MyImage::set_Face_Cascade_Name(): Error loading face cascade"<<endl;
        return false;
    }
    return true;
}

void MyImage::toBlackandWhite() {
    // Convert image to black and white with 3 channels
    cvtColor(this->image,this->image,COLOR_RGB2GRAY);
    cvtColor(this->image,this->image,COLOR_GRAY2BGR);
}

void MyImage::inverseImage() {
    // Inverse the content of the image
/*    Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for( int i = 0; i < 256; ++i)
        p[i] = 255 - i ;
    LUT(this->image, lookUpTable, this->image);
*/
    // Replaced by
    bitwise_not(this->image, this->image);
}

void MyImage::smoothImage(Mat &imag, int blur_range, int method) {
    switch (method) {
        case 1:
            blur( imag, imag, Size(blur_range,blur_range) );
            break;
        case 2:
            GaussianBlur( imag, imag, Size(blur_range,blur_range) , 0 , 0 );
            break;
        case 3:
            medianBlur( imag, imag, blur_range );
            break;
        case 4:
            imag.copyTo( this->mask );
            bilateralFilter( this->mask, imag, blur_range, blur_range*2.0 , blur_range*0.5 );
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
            morphologyEx(imag, imag, op, getStructuringElement(shape, Size(blur_range,blur_range)) );
            break;
        }
        case 12: // cv::MorphTypes MORPH_HITMISS  = 7
        {
            cvtColor(imag,imag,COLOR_RGB2GRAY); // Image must be in BW
            int op = method - 5;
            Mat kernel = (Mat_<int>(3, 3) <<
                0, 1, 0,
                1, -1, 1,
                0, 1, 0);
            morphologyEx(imag, imag, op, kernel );
            break;
        }
        default :
            cout << "MyImage::smoothImage(): Unknown kind of blur\n" ;
            break;
    }
}

void MyImage::detectEdges() {
    switch (this->edge_method) {
        case 1: { // Sobel
            Mat gray,grad,grad_x,abs_grad_x,grad_y,abs_grad_y;
            smoothImage(this->image, 3, 2); // Gaussian filter
            cvtColor( this->image, gray, COLOR_BGR2GRAY );
            // Gradient X
            Sobel( gray, grad_x, CV_16S, 1, 0, 3 );
            convertScaleAbs( grad_x, abs_grad_x );
            // Gradient Y
            Sobel( gray, grad_y, CV_16S, 0, 1, 3 );
            convertScaleAbs( grad_y, abs_grad_y );
            // Total Gradient (approximate)
            addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
            cvtColor(grad,this->image,COLOR_GRAY2BGR);
            break;
            }
        case 2: { // Laplacian
            Mat gray,abs_laplacian,laplacian;
            smoothImage(this->image, 3, 2); // Gaussian filter
            cvtColor( this->image, gray, COLOR_BGR2GRAY );
            Laplacian( gray, laplacian, CV_16S, 3 );
            convertScaleAbs( laplacian, abs_laplacian );
            cvtColor(abs_laplacian,this->image,COLOR_GRAY2BGR);
            break;
            }
        case 3: { // Canny edge detector
            Mat dst,gray,detected_edges;
            smoothImage(this->image, 3, 2); // Gaussian filter
            cvtColor( this->image, gray, COLOR_BGR2GRAY );
            Canny( gray, detected_edges, this->canny_threshold, this->canny_threshold*this->canny_ratio, 3 );
            dst = Scalar::all(0);
            this->image.copyTo(dst,detected_edges);
            dst.copyTo(this->image);
            break;
            }
        default :
            cout << "MyImage::detectEdges(): Unknown kind of edge detection\n" ;
            break;
        }
}

void MyImage::detect_faces() {
    this->image.copyTo(this->mask);
    this->mask.setTo(Scalar(255,255,255));

    std::vector<Rect> faces;
    std::vector<Point> ROI_Poly;

    //-- Detect faces
    face_cascade.detectMultiScale( this->image, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    // Draw ellipses around faces and get regions of interest
    for( size_t i = 0; i < faces.size(); i++ )
    {
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        //ellipse( image, center, Size( faces[i].width*0.6, faces[i].height*0.8), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

        // approximate ellipse by a polygon
        ellipse2Poly(center, Size( faces[i].width*0.6, faces[i].height*0.8), 0, 0, 360, 10, ROI_Poly) ;
        // fill the polygon by zeros in the mask
        fillConvexPoly(this->mask, &ROI_Poly[0], ROI_Poly.size(), Scalar( 0, 0, 0 ), 4, 0);
    }
}

double MyImage::thresholdImage() {
    double value = this->threshold_value;
    switch (this->threshold_method) {
    case 1: // cv::ThresholdTypes THRESH_BINARY       = 0
    case 2: // cv::ThresholdTypes THRESH_BINARY_INV   = 1
    case 3: // cv::ThresholdTypes THRESH_TRUNC        = 2
    case 4: // cv::ThresholdTypes THRESH_TOZERO       = 3
    case 5: { // cv::ThresholdTypes THRESH_TOZERO_INV = 4
        cvtColor(this->image,this->image,COLOR_RGB2GRAY);
        int type = this->threshold_method - 1 ;
        threshold( this->image, this->image, this->threshold_value, 255, type );
        cvtColor(this->image,this->image,COLOR_GRAY2BGR);
        break;
    }
    case 6: { // cv::ThresholdTypes THRESH_OTSU = 8
        cvtColor(this->image,this->image,COLOR_RGB2GRAY);
        value = threshold( this->image, this->image, 0, 255, THRESH_BINARY | THRESH_OTSU );
        cvtColor(this->image,this->image,COLOR_GRAY2BGR);
        break;
    }
    case 7: { // cv::ThresholdTypes THRESH_TRIANGLE = 16
        cvtColor(this->image,this->image,COLOR_RGB2GRAY);
        value = threshold( this->image, this->image, 0, 255, THRESH_BINARY | THRESH_TRIANGLE  );
        cvtColor(this->image,this->image,COLOR_GRAY2BGR);
        break;
    }
    case 8: { // adaptive threshold simple mean
        cvtColor(this->image,this->image,COLOR_RGB2GRAY);
        if (this->threshold_type == 1)
            adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY ,this->threshold_blocksize, 0. );
        else
            adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV ,this->threshold_blocksize, 0. );
        cvtColor(this->image,this->image,COLOR_GRAY2BGR);
        break;
    }
    case 9: { // adaptive threshold simple mean
        cvtColor(this->image,this->image,COLOR_RGB2GRAY);
        if (this->threshold_type == 1)
            adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY ,this->threshold_blocksize, 0. );
        else
            adaptiveThreshold( this->image, this->image, 255,cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV ,this->threshold_blocksize, 0. );
        cvtColor(this->image,this->image,COLOR_GRAY2BGR);
        break;
    }
    default:
        cout << "MyImage::thresholdImage(): Unknown kind of threshold\n" ;
        break;
    }
    return value;
}

void MyImage::transformImage() {
    switch (this->transformation_method) {
    case 1: { // Rotation around center of image
        int centerX = this->image.rows / 2;
        int centerY = this->image.cols / 2;

        Mat matRotation = getRotationMatrix2D( Point(centerY, centerX), (transf_rotation_value), 1 );
        warpAffine( this->image, this->image, matRotation, this->image.size() );
        break;
    }
    default:
        cout << "MyImage::transformImage(): Unknown kind of transformation\n" ;
        break;
    }

}

void MyImage::equalizeHistogram() {
    vector<Mat> channels;
    cvtColor(this->image, this->image, COLOR_BGR2YCrCb); //change the color image from BGR to YCrCb format
    split(this->image,channels); //split the image into channels

    switch (this->histo_eq_method) {
    case 1: {
        equalizeHist(channels[0], channels[0]); //equalize histogram on the 1st channel (Y)
        break;
    }
    case 2:{
        Ptr<CLAHE> clahe = createCLAHE();
        clahe->setClipLimit(this->histo_clip_limit);
        clahe->setTilesGridSize(Size(this->histo_tiles,this->histo_tiles));
        clahe->apply(channels[0],channels[0]);
        break;
    }
    default:{
        cout << "MyImage::equalizeHistogram(): Unknown type of histogram operation\n";
        break;
    }
    }
    merge(channels,this->image); //merge 3 channels including the modified 1st channel into one image
    cvtColor(this->image, this->image, COLOR_YCrCb2BGR);
}

#ifdef withstitching
void MyImage::panorama_insert_image() {
    if (!this->image.empty())
        this->Panorama_vector.push_back(this->image);
}
#endif

#ifdef withstitching
void MyImage::panorama_pop_up_image() {
    if (!this->Panorama_vector.empty())
        this->Panorama_vector.pop_back();
}
#endif

#ifdef withstitching
std::string MyImage::panorama_compute_result() {
    Stitcher::Mode mode = Stitcher::PANORAMA; // PANORAMA or SCANS
    std::string return_status;

    if (this->Panorama_vector.size()==1)
        this->Panorama_stitcher = Stitcher::create(mode);
    Stitcher::Status status = Panorama_stitcher->stitch(this->Panorama_vector, this->panorama);
    switch (status) {
    case Stitcher::OK: {
        cout << "Stitcher normal return status " << status << endl;
        return_status = "Stitcher normal return status "+std::to_string(status);
        break;
    }
    case Stitcher::ERR_NEED_MORE_IMGS:{
        cout << "Stitcher requires additionnal images " << status << endl;
        return_status = "Stitcher requires additionnal images "+std::to_string(status);
        break;
    }
    case Stitcher::ERR_HOMOGRAPHY_EST_FAIL:{
        cout << "Stitcher error: homography estimation fail " << status << endl;
        cout << "   Removing the problematic image from the panorama\n";
        return_status = "Stitcher error: homography estimation fail "+std::to_string(status);
        this->Panorama_vector.pop_back();
        break;
    }
    case Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL:{
        cout << "Stitcher error: camera parameters adjust fail " << status << endl;
        cout << "   Removing the problematic image from the panorama\n";
        return_status = "Stitcher error: camera parameters adjust fail "+std::to_string(status);
        this->Panorama_vector.pop_back();
        break;
    }
    default:{
        cout << "MyImage::panorama_compute_result(): Unknown type of return status from Panorama_stitcher.stitch()\n";
        return_status = "Unknown type of return status from Panorama_stitcher.stitch()";
        break;
    }
    }
    return return_status;
}
#endif

#ifdef withstitching
void MyImage::panorama_reset() {
    if (!this->Panorama_vector.empty())
        this->Panorama_vector.clear();
    this->panorama = this->image;
}
#endif

#ifdef withstitching
int MyImage::panorama_get_size(){
    return this->Panorama_vector.size();
}
#endif
