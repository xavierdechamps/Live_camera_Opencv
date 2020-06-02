# Path to global OpenCV installation directory
MY_OPENCV_DIR = /Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.3.0/install
#
# If you don't have the OpenCV stiching library, comment the following line
CONFIG += stitching
#
# If you don't have the OpenCV object detect library, comment the following 2 lines
CONFIG += objdetect
# (Macro) Path to the directory containing the cascade classifier files for face detection
DEFINES += OPENCV_DATA_DIR=\\\"/Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.3.0/install/share/opencv4/haarcascades/\\\"
#
# If you don't have the OpenCV extra module Xphoto, comment the following line
CONFIG += xphoto
#
# If you don't have the OpenCV extra module Face, comment the following line
CONFIG += face
#
# If you don't have the ZBar library, comment the following line
CONFIG += zbar
MY_ZBAR_DIR = /Users/dechamps/Documents/Codes/Cpp/Images/Libraries/zbar-0.10/install
#
############# DO NOT MODIFY BELOW THIS LINE #############
#
stitching: DEFINES+=withstitching
objdetect: DEFINES+=withobjdetect
xphoto: DEFINES+=withxphoto
zbar: DEFINES+=withzbar
face: DEFINES+=withface

QT       = core gui multimedia concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#!versionAtLeast(QT_VERSION, 4.6):error("Use at least Qt version 4.6")

TARGET = Video
TEMPLATE = app

# Resource collection
face: RESOURCES = images.qrc

SOURCES = main.cpp \
        myimage.cpp \
        mainwindow.cpp \
        dialog_blur.cpp \
        dialog_edge.cpp \
        dialog_threshold.cpp \
        dialog_transformation.cpp \
        dialog_histogram.cpp \
        dialog_object_detection.cpp \
        dialog_motion_detection.cpp \
        secondarywindow.cpp \
        dialog_photo.cpp \
        capturevideo.cpp
stitching: SOURCES+=dialog_panorama.cpp

HEADERS  += myimage.h \
            mainwindow.h \
            dialog_blur.h \
            dialog_edge.h \
            dialog_threshold.h \
            dialog_transformation.h \
            dialog_histogram.h \
            dialog_object_detection.h \
            dialog_motion_detection.h \
            secondarywindow.h \
            dialog_photo.h \
            capturevideo.h
stitching: HEADERS += dialog_panorama.h

# Compilator flags
QMAKE_CXXFLAGS += -std=c++11

# Path to OpenCV include directory
INCLUDEPATH += $${MY_OPENCV_DIR}/include/opencv4
# Include the header from ZBar
zbar: INCLUDEPATH += $${MY_ZBAR_DIR}/include

# Linker flags
QMAKE_LFLAGS += -Wl,-rpath,$${MY_OPENCV_DIR}/lib
# Include libraries from ZBar
zbar: QMAKE_LFLAGS += -Wl,-rpath,$${MY_ZBAR_DIR}/lib

# Required OpenCV libraries
LIBS = -L$${MY_OPENCV_DIR}/lib
LIBS += -lopencv_imgcodecs \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_imgproc \
        -lopencv_video\
        -lopencv_photo
stitching: LIBS += -lopencv_stitching
objdetect: LIBS += -lopencv_objdetect
xphoto: LIBS += -lopencv_xphoto
face: LIBS += -lopencv_face
zbar: LIBS += -L$${MY_ZBAR_DIR}/lib -lzbar
