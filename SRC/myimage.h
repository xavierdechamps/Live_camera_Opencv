/*
 * Copyright: Xavier Dechamps
*/

#ifndef MYIMAGE_H
#define MYIMAGE_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#ifdef withobjdetect
#include "opencv2/objdetect/objdetect.hpp"
#endif
#ifdef withstitching
#include "opencv2/stitching.hpp"
#endif
#include "opencv2/video/tracking.hpp"
#include "opencv2/video.hpp"

#include <iostream>

using namespace cv;
using namespace std;

class MyImage
{
public:
    MyImage();

    void set_image_content(Mat &content);
#ifdef withobjdetect
    bool set_Face_Cascade_Name(String &new_name);
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

#ifdef withstitching
    void panorama_insert_image();
    void panorama_pop_up_image();
    std::string panorama_compute_result();
    void panorama_reset();
    int panorama_get_size();
#endif

    Mat& get_image_content();
    Mat& get_image_histogram();
#ifdef withstitching
    Mat& get_image_panorama();
#endif
    Mat& get_object_detected();
    Mat& get_motion_detected();

    void toggleBW();
    void toggleInverse();
    void toggleBlur();
    void toggleThreshold();
    void toggleTransformation();
    void toggleEdge();
#ifdef withobjdetect
    void toggleFace_Recon();
#endif
    void toggleHistoEq();
    void toggleObjectDetection();
#ifdef withstitching
    void togglePanorama();
#endif
    void toggleMotionDetection();

private:
    Mat image, previmage , mask , smoothed, histogram, objects, panorama, motion ;
    int blur_range,blur_method,morpho_element;
    int edge_method;
    int canny_threshold;
    int object_detection_method;
    int hough_line_threshold;
    int threshold_method,threshold_value,threshold_blocksize,threshold_type;
    int transformation_method,transf_rotation_value;
    int histo_eq_method,histo_tiles,histo_clip_limit;
    int motion_detection_method;
    double canny_ratio;
    bool coloured,inversed,blurred,edge_detect,face_recon,thresholded,object_detected;
    bool transformed;
    bool motion_detected,motion_background_first_time;
    bool histo_eq;
#ifdef withobjdetect
    String face_cascade_name ;
    CascadeClassifier face_cascade;
#endif

#ifdef withstitching
    bool panorama_activated;
    vector<Mat> Panorama_vector;
    Ptr<Stitcher> Panorama_stitcher;
#endif

    Ptr<BackgroundSubtractor> pMOG2;

    void toBlackandWhite();
    void inverseImage();
    void smoothImage(Mat &imag, int blur_range, int method);
    void detectEdges();
#ifdef withobjdetect
    void detect_faces();
#endif
    double thresholdImage();
    void transformImage();
    void equalizeHistogram();
};

#endif // MYIMAGE_H