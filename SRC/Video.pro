# If you don't have the OpenCV stiching library, comment the following line
CONFIG += stitching
# If you don't have the OpenCV object detect library, comment the following line
CONFIG += objdetect
#
############# DO NOT MODIFY BELOW THIS LINE #############
#
stitching: DEFINES+=withstitching
objdetect: DEFINES+=withobjdetect

QT       = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Video
TEMPLATE = app

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
        dialog_photo.cpp
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
            dialog_photo.h
stitching: HEADERS += dialog_panorama.h

# Compilator flags
QMAKE_CXXFLAGS += -std=c++11

# Path to OpenCV include directory
INCLUDEPATH += /Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.1.2/install/include/opencv4

# Linker flags
QMAKE_LFLAGS += -Wl,-rpath,/Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.1.2/install/lib

# Required OpenCV libraries
LIBS = -L/Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.1.2/install/lib
LIBS += -lopencv_imgcodecs \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_imgproc \
        -lopencv_video\
        -lopencv_photo
stitching: LIBS += -lopencv_stitching
objdetect: LIBS += -lopencv_objdetect
