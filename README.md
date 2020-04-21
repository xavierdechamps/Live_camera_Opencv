# Live_camera_Opencv
This C++ program uses the built-in camera or any connected camera to perform image treatments. The image manipulations are performed by [OpenCV](https://github.com/opencv/opencv) and the interface is managed by Qt.

The implemented image manipulations are:
- Black and white + colour inversion
- Blur filters
  * Simple
  * Gaussian
  * Median
  * Bilateral
- Morphological filters
  * Erode + Dilate
  * Opening + Closing
  * Morphological gradient + Top hat + Black hat
  * Hit / Miss
- Threshold
  * Global threshold (binary, binary inverted, truncate, to zero, to zero inverted, Otsu, triangle method)
  * Adaptive threshold (mean and Gaussian)
- Histogram equalization (global + CLAHE)
- Edge detectors (Sobel derivatives, Laplacian and Canny edge detector)
- Face recognition
- Object recognition (Hough line, Hough circle and Harris corner detection)
- Image stitching to create a panorama
- Motion detection (Farneback dense optical flow and foreground extraction)
- Module Photo
  *  Contrast Preserving Decolorization
  *   Denoizing (Non-Local Means)
  *   Non-Photorealistic Rendering (Edge preserving, Detail enhancing, Pencil sketch, Stylization)

# Requirements
This code requires an installed version of OpenCV. The program is linked against the following libraries of OpenCV:

* libopencv_core
* libopencv_imgcodecs
* libopencv_highgui
* libopencv_videoio
* libopencv_imgproc
* libopencv_video
* libopencv_photo

The code may also be compiled against two optional libraries:
* libopencv_objdetect
* libopencv_stitching

The first one enables face detection and the second one creates a panorama from a series of picked up frames. If you don't have these libraries inside your OpenCV installation directory, just comment the top lines in the file [Video.pro](SRC/Video.pro). If you have the objdetect library, a face-detetection cascade from OpenCV is also required. In this program the face-detection cascade file is hard-coded as opencv-4.1.0/data/haarcascades/haarcascade_frontalface_default.xml in the file [mainwindow.cpp](SRC/mainwindow.cpp). This path must be adapted according to your installation.

Your C++ compiler must accept c++11 directives.

## On a MacOS
I developed this program on a old Mac. The graphic card supports only OpenCL 1.0 directives, which is not sufficient for for image stitching. Luckily my CPU manages version 1.2 of OpenCL. To set OpenCL to use the CPU, I had to export the environment variable OPENCV_OPENCL_DEVICE=":CPU:0". Moreover if your program complains about not finding OpenCV libraries, don't forget to export the path to the OpenCV libraries in the environment variable DYLD_LIBRARY_PATH.

Edit the file [Video.pro](SRC/Video.pro) and provide the path where your OpenCV is installed. To create a Makefile or a XCode project, run the Qt command:
```
qmake Video.pro
```

## On Linux
Not tested. Should be similar to what is done for MacOS

## On Windows
Not tested. No idea.
