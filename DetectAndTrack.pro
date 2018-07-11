TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


SOURCES += main.cpp \
    util/BoundingBox.cpp \
    util/Detection.cpp \
    util/Tracking.cpp \
    tracker/PAOT.cpp \
    tracker/Tracker.cpp \
    tracker/Affinity.cpp \
    detector/Detector.cpp \
    detector/YOLODetector.cpp \
    util/ObjData.cpp \
    tracker/KalmanPredictor.cpp \
    tracker/Predictor.cpp

HEADERS += \
    util/BoundingBox.h \
    util/Detection.h \
    util/Tracking.h \
    tracker/PAOT.h \
    tracker/Tracker.h \
    tracker/Affinity.h \
    detector/Detector.h \
    detector/YOLODetector.h \
    util/ObjData.h \
    tracker/KalmanPredictor.h \
    tracker/Predictor.h

INCLUDEPATH += "/usr/include"\
            += "/usr/local/include"\
            += "/usr/local/include/opencv"\
            += "/usr/local/include/opencv2"\
            += "/usr/local/cuda/include"\
            += "/home/nvidia/lyl/yolov3/include"

LIBS += -L"/home/nvidia/lyl/yolov3" -ldarknet
LIBS += -L"/usr/local/lib/" \
        -lcurl\
        -ldlib
LIBS += -L"/usr/lib"\
        -llapack\
        -lopencv_core\
        -lopencv_highgui\
        -lopencv_imgproc\
        -lopencv_video
LIBS += -L"/usr/local/cuda/lib64"\
        -lcuda \
        -lcudart\
        -lcublas\
        -lcurand
LIBS += -lpthread

