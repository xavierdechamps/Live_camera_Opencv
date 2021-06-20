#
# Adapt the following parameters depending on your platform and the installed libraries (OpenCV + ZBar)
#
unix {
    # Path to global OpenCV installation directory
    MY_OPENCV_DIR = /Users/dechamps/Documents/Codes/Libraries/opencv-4.5.2/install
    #
    # If you don't have the OpenCV object detect library, comment the following 2 lines
    CONFIG += objdetect
    # (Macro) Path to the directory containing the cascade classifier files for face detection
    DEFINES += OPENCV_HAARCASCADES_DIR=\\\"/Users/dechamps/Documents/Codes/Libraries/opencv-4.5.2/install/share/opencv4/haarcascades/\\\"
    #
    # If you don't have the OpenCV extra module Face, comment the following line
    CONFIG += face
    # https://github.com/kurnianggoro/GSOC2017/blob/master/data/lbfmodel.yaml
    # (Macro) Path to the directory containing the facemark file "lbfmodel.yaml"
    DEFINES += OPENCV_FACEMARK_DIR=\\\"/Users/dechamps/Documents/Codes/Libraries/opencv-4.5.2/install/share/opencv4/lbfmodel/\\\"
    #
    # If you don't have the ZBar library, comment the following line
    CONFIG += zbar
    MY_ZBAR_DIR = /Users/dechamps/Documents/Codes/Cpp/Images/Libraries/zbar-0.10/install
#
#
    CONFIG += tesseract
    MY_TESSERACT_DIR = /Users/dechamps/Documents/Codes/Libraries/tesseract-4.1.1/install
    DEFINES += TESSERACT_DATA=\\\"/Users/dechamps/Documents/Codes/Libraries/tesseract-4.1.1/tessdata-master/\\\"
    DEFINES += TESSERACT_DNN=\\\"/Users/dechamps/Documents/Codes/Libraries/opencv-4.5.2/install/share/opencv4/dnn/frozen_east_text_detection.pb\\\"
    #
}

win32 {
    # Path to global OpenCV installation directory
    MY_OPENCV_DIR = D:\Libraries\opencv-4.5.2\build\install
    #
    # If you don't have the OpenCV object detect library, comment the following 2 lines
    CONFIG += objdetect
    # (Macro) Path to the directory containing the cascade classifier files for face detection
    DEFINES += OPENCV_HAARCASCADES_DIR=\\\"D:/Libraries/opencv-4.5.2/build/install/etc/haarcascades/\\\"
    #
    # If you don't have the OpenCV extra module Face, comment the following line
    CONFIG += face
    # https://github.com/kurnianggoro/GSOC2017/blob/master/data/lbfmodel.yaml
    # (Macro) Path to the directory containing the facemark file "lbfmodel.yaml"
    DEFINES += OPENCV_FACEMARK_DIR=\\\"D:/Libraries/opencv-4.5.2/build/install/etc/facemark/\\\"
    #
    # If you don't have the ZBar library, comment the following line
    #CONFIG += zbar
    #MY_ZBAR_DIR =
}

# If you don't have the OpenCV stiching library, comment the following line
CONFIG += stitching
#
# If you don't have the OpenCV extra module Xphoto, comment the following line
CONFIG += xphoto
#
############# DO NOT MODIFY BELOW THIS LINE #############
#
stitching: DEFINES+=withstitching
objdetect: DEFINES+=withobjdetect
xphoto: DEFINES+=withxphoto
zbar: DEFINES+=withzbar
face: DEFINES+=withface
tesseract: DEFINES+=withtesseract

QT       = core gui multimedia concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Video
TEMPLATE = app

# Resource collection
RESOURCES = images.qrc

SOURCES = main.cpp \
        dialog_choose_camera.cpp \
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
tesseract: SOURCES += window_tesseract.cpp

HEADERS  += myimage.h \
            dialog_choose_camera.h \
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
tesseract: HEADERS += window_tesseract.h

# Include the header from ZBar
zbar: INCLUDEPATH += $${MY_ZBAR_DIR}/include
tesseract: INCLUDEPATH += $${MY_TESSERACT_DIR}/include

win32 {
    message("Windows build")

    # Path to OpenCV include directory
    INCLUDEPATH += $${MY_OPENCV_DIR}\include

    # Required OpenCV libraries
    LIBS = $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_imgcodecs452.lib \
           $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_core452.lib \
           $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_highgui452.lib \
           $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_videoio452.lib \
           $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_imgproc452.lib \
           $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_video452.lib \
           $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_photo452.lib
    stitching: LIBS += $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_stitching452.lib
    objdetect: LIBS += $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_objdetect452.lib
    xphoto: LIBS += $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_xphoto452.lib
    face: LIBS += $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_face452.lib
    zbar: LIBS += $${MY_ZBAR_DIR}\lib\libzbar-0.lib
    tesseract: LIBS += $${MY_TESSERACT_DIR}\lib\tesseract41.lib $${MY_OPENCV_DIR}\x64\vc15\lib\opencv_dnn452.lib
}

unix {
    message("Unix build")

    # Path to OpenCV include directory
    INCLUDEPATH += $${MY_OPENCV_DIR}/include/opencv4

    # Compilator flags
    QMAKE_CXXFLAGS += -std=c++11

    # Linker flags
    QMAKE_LFLAGS += -Wl,-rpath,$${MY_OPENCV_DIR}/lib
    # Include libraries from ZBar
    zbar: QMAKE_LFLAGS += -Wl,-rpath,$${MY_ZBAR_DIR}/lib
    # Include libraries from Tesseract
    zbar: QMAKE_LFLAGS += -Wl,-rpath,$${MY_TESSERACT_DIR}/lib

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
    tesseract: LIBS += -lopencv_dnn -L$${MY_TESSERACT_DIR}/lib -ltesseract
    zbar: LIBS += -L$${MY_ZBAR_DIR}/lib -lzbar
}
