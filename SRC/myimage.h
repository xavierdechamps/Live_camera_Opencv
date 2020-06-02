/*
 * Copyright (C) 2019-2020 Xavier Dechamps
*/

#ifndef MYIMAGE_H
#define MYIMAGE_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"
#ifdef withobjdetect
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#endif
#ifdef withstitching
#include "opencv2/stitching.hpp"
#endif
#ifdef withxphoto
#include "opencv2/xphoto/white_balance.hpp"
#endif
#include "opencv2/video/tracking.hpp"
#include "opencv2/video.hpp"
#ifdef withface
#include "opencv2/face/facemark.hpp" 
#endif

#ifdef withzbar
#include "zbar.h"
#endif

#include <iostream>

//using namespace cv;
//using namespace std;

class MyImage
{
public:
    MyImage();

    void set_image_content(cv::Mat &content);
#ifdef withobjdetect
    bool set_Face_Cascade_Name(cv::String &new_name);
    bool set_background_image(cv::String &filename);
#endif

    void set_size_blur(int);
    void set_blur_method(int);
    void set_morpho_element(int);

    void set_threshold_method(int);
    void set_threshold_type(int);
    void set_threshold_value(int);
    void set_threshold_blocksize(int);

    void set_transf_method(int);
    void set_transf_rotation_value(int);

    void set_edge_method(int);
    void set_canny_threshold(int);
    void set_canny_ratio(double);

    void set_histo_eq_method(int);
    void set_histo_eq_tiles(int);
    void set_histo_eq_clip_limit(int);

    void set_object_detection_method(int);
    void set_hough_line_threshold(int);

    void set_motion_detection_method(int);
    
    void set_photo_method(int);
    void set_photo_sigmas(int);
    void set_photo_sigmar(double);
    
#ifdef withstitching
    void panorama_insert_image();
    void panorama_pop_up_image();
    std::string panorama_compute_result();
    void panorama_reset();
    int panorama_get_size();
#endif

    cv::Mat& get_image_content();
    cv::Mat& get_image_histogram();
#ifdef withstitching
    cv::Mat& get_image_panorama();
#endif
    cv::Mat& get_object_detected();
    cv::Mat& get_motion_detected();

    void toggleBW(bool);
    void toggleInverse(bool);
    void toggleBlur(bool);
    void toggleThreshold(bool);
    void toggleTransformation(bool);
    void toggleEdge(bool);
#ifdef withobjdetect
    void toggleFace_Recon(bool);
    bool getFace_Status();
#ifdef withface
    void loadOrnaments(std::vector<cv::Mat> Mat2receive);
#endif
#endif
    void toggleHistoEq(bool);
    void toggleObjectDetection(bool);
#ifdef withstitching
    void togglePanorama(bool);
#endif
    void toggleMotionDetection(bool);
    void togglePhoto(bool);
#ifdef withzbar
    void toggleQRcode(bool);
    bool getQRcodedata(std::string &, std::string &);
#endif

private:
    cv::Mat image, previmage, image2export , mask , smoothed, histogram, objects, panorama, motion, background ;
    int blur_range,blur_method,morpho_element;
    int edge_method;
    int canny_threshold;
    int object_detection_method;
    int hough_line_threshold;
    int threshold_method,threshold_value,threshold_blocksize,threshold_type;
    int transformation_method,transf_rotation_value;
    int histo_eq_method,histo_tiles,histo_clip_limit;
    int motion_detection_method,photo_method,photo_sigmas;
    double canny_ratio,photo_sigmar;
    bool coloured,inversed,blurred,edge_detect,face_recon,thresholded,object_detected;
    bool transformed,photoed;
    bool motion_detected,motion_background_first_time;
    bool histo_eq;
#ifdef withobjdetect
    cv::String face_cascade_name ;
    cv::CascadeClassifier face_cascade;
    
#ifdef withface
    cv::Ptr<cv::face::Facemark> mark_detector;
    // Mask ornaments to put over face detected
    cv::Mat ornament_glasses, ornament_mustache, ornament_mouse_nose;
#endif
    
#endif

#ifdef withstitching
    bool panorama_activated;
    std::vector<cv::Mat> Panorama_vector;
    cv::Ptr<cv::Stitcher> Panorama_stitcher;
#endif
#ifdef withzbar
    bool qrcodeactivated;
    cv::String qrcodedata,qrcodetype;
#endif

    cv::Ptr<cv::BackgroundSubtractor> pMOG2;

    // Methods
    void toBlackandWhite();
    void inverseImage();
    void smoothImage(cv::Mat &imag, int blur_range, int method);
    void detectEdges();
#ifdef withobjdetect
    void detect_faces();
#ifdef withface
    void draw_glasses(cv::Mat &frame, std::vector<cv::Point2f> &marks);
    void draw_mustache(cv::Mat &frame, std::vector<cv::Point2f> &marks);
    void draw_mouse_nose(cv::Mat &frame, std::vector<cv::Point2f> &marks);
#endif // endif withface
#endif // endif withobjdetect
    double thresholdImage();
    void transformImage();
    void equalizeHistogram();
    void modulephoto();
#ifdef withzbar
    void getQRcode();
#endif
};

#endif // MYIMAGE_H
