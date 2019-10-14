QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Video
TEMPLATE = app

SOURCES += main.cpp\
        myimage.cpp \
        mainwindow.cpp \
        dialog_blur.cpp \
        dialog_edge.cpp \
        dialog_threshold.cpp \
        dialog_transformation.cpp \
        dialog_histogram.cpp \
        dialog_object_detection.cpp \
        dialog_panorama.cpp \
        dialog_motion_detection.cpp \
        secondarywindow.cpp

HEADERS  += myimage.h \
            mainwindow.h \
            dialog_blur.h \
            dialog_edge.h \
            dialog_threshold.h \
            dialog_transformation.h \
            dialog_histogram.h \
            dialog_object_detection.h \
            dialog_panorama.h \
            dialog_motion_detection.h \
            secondarywindow.h

QMAKE_CXXFLAGS += -std=c++11

QMAKE_LFLAGS += -Wl,-rpath,/Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.1.2/install/lib
INCLUDEPATH += /Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.1.2/install/include/opencv4
LIBS += -L/Users/dechamps/Documents/Codes/Cpp/Images/Libraries/opencv-4.1.2/install/lib

LIBS += -lopencv_imgcodecs \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_imgproc \
        -lopencv_objdetect \
        -lopencv_stitching \
        -lopencv_video
