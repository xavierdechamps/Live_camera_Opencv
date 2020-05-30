/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *   Launch the Qt Window
 *   nothing special done here
*/

#include <QApplication>

#include "mainwindow.h"

/**
\mainpage Welcome to Live_camera_OpenCV
\section section_intro Introduction
 
This C++ program uses the built-in camera or any connected camera to perform image treatments. The image manipulations are performed by <a href="https://github.com/opencv/opencv">OpenCV</a> and the interface is managed by Qt. This multithreaded program launches a thread dedicated to image processing and another thread to pure Qt display.
The implemented image manipulations are:
- Black and white + colour inversion
- Blur filters
  -# Simple
  -# Gaussian
  -# Median
  -# Bilateral
- Morphological filters
  -# Erode + Dilate
  -# Opening + Closing
  -# Morphological gradient + Top hat + Black hat
  -# Hit / Miss
- Threshold
  -# Global threshold (binary, binary inverted, truncate, to zero, to zero inverted, Otsu, triangle method)
  -# Adaptive threshold (mean and Gaussian)
- Histogram equalization (global + CLAHE)
- Edge detectors (Sobel derivatives, Laplacian and Canny edge detector)
- Face recognition
- Object recognition (Hough line, Hough circle and Harris corner detection)
- Image stitching to create a panorama
- Motion detection (Farneback dense optical flow and foreground extraction)
- Module Photo
  -#  Contrast Preserving Decolorization
  -#   Denoizing (Non-Local Means)
  -#   Non-Photorealistic Rendering (Edge preserving, Detail enhancing, Pencil sketch, Stylization)
  -#   White balancing (from module xphoto)
- Barcode and QR code decoder (through the external library [ZBar](https://github.com/ZBar/ZBar))

\section section_requirements Requirements

This code requires an installed version of OpenCV. The program is linked against the following libraries of OpenCV:

- libopencv_core
- libopencv_imgcodecs
- libopencv_highgui
- libopencv_videoio
- libopencv_imgproc
- libopencv_video
- libopencv_photo

The code can also be compiled against four optional libraries:
- libopencv_objdetect (enables face detection)
- libopencv_stitching (enables the panorama creation)
- libopencv_xphoto (enables features from xphoto)
- libzbar (enables decryption of barcodes and QR codes)

If you don't have these libraries, just comment the top lines in the file Video.pro. If you have the objdetect library, a face-detetection cascade from OpenCV is also required. In this program the face-detection cascade file is hard-coded as "opencv-4.3.0/data/haarcascades/haarcascade_frontalface_default.xml" in the file capturevideo.cpp. This path must be adapted according to your installation. The ZBar library must also be compiled in order to be able to decode the barcodes / QR codes. For this library to function, it is not mandatory to compile ZBar with gtk, python or qt4 options enabled (only the headers and the library are required, not the executable).

You must have Qt5 in order to fully take advantage of multithreading. If you only have Qt4, please download version 1.0 of this program.

Your C++ compiler must accept c++11 directives.

\section section_installation Installation

\subsection subsection_install_mac On a MacOS
This program has been developed on a old Mac. The graphic card supports only OpenCL 1.0 directives, which is not sufficient for for image stitching. Luckily the CPU managed version 1.2 of OpenCL. To set OpenCL to use the CPU, the environment variable OPENCV_OPENCL_DEVICE=":CPU:0" had to be exported. Moreover if your program complains about not finding OpenCV libraries, don't forget to export the path to the OpenCV libraries in the environment variable DYLD_LIBRARY_PATH.

Edit the file Video.pro and provide the path where your OpenCV and ZBar libraries are installed. To create a Makefile or a XCode project, run the Qt command:
qmake Video.pro

\subsection subsection_install_linux On Linux
Not tested. Should be similar to what is done for MacOS

\subsection subsection_install_windows On Windows
Not tested. No idea.

 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow *window =new MainWindow() ;

    window->show();

    return app.exec();
}

