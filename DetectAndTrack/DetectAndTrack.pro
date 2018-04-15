TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


SOURCES += main.cpp \
    util/BoundingBox.cpp \
    util/Detection.cpp \
    util/DetectionFileParser.cpp \
    util/Tracking.cpp \
    tracker/PAOT.cpp \
    tracker/predictor/kalman/KalmanPredictor.cpp \
    tracker/predictor/Predictor.cpp \
    tracker/Tracker.cpp \
    tracker/Affinity.cpp \
    tracker/RandomTracker.cpp \
    detector/Detector.cpp \
    detector/YOLODetector.cpp

HEADERS += \
    util/BoundingBox.h \
    util/Detection.h \
    util/DetectionFileParser.h \
    util/Tracking.h \
    tracker/PAOT.h \
    tracker/predictor/kalman/KalmanPredictor.h \
    tracker/predictor/Predictor.h \
    tracker/Tracker.h \
    tracker/Affinity.h \
    tracker/RandomTracker.h \
    detector/Detector.h \
    detector/YOLODetector.h

INCLUDEPATH += "/usr/local/include"\
            += "/usr/local/include/opencv"\
            += "/usr/local/include/opencv2"\
            += "/usr/local/cuda/include"\
            += "/home/lyl/darknet-master/include"

LIBS += -L"/home/lyl/darknet-master" -ldarknet
LIBS += -L"/usr/lib" -llapack
LIBS += -L"/usr/local/lib/" \        
        -lopencv_core\
        -lopencv_highgui\
        -lopencv_imgproc\
        -lopencv_video\
        -lopencv_videoio\
        -ldlib
LIBS += -L"/usr/local/cuda/lib64"\
        -lcuda \
        -lcudart\
        -lcublas\
        -lcurand
LIBS += -lpthread

