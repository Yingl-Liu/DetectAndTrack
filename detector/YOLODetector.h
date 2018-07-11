#ifndef YOLODETECTOR_H
#define YOLODETECTOR_H
#include "Detector.h"
#include <string>
#include <memory>
#include <iostream>
#include <map>

extern "C"{
#include "darknet.h"
extern image ipl_to_image(IplImage *src);
extern void ipl_into_image(IplImage* src, image im);
}

class YOLODetector : public Detector
{
public:
    YOLODetector(const std::string & dataFile, const std::string & cfgFile, const std::string & weightFile, const int w, const int h, const int c, float thresh = 0.6, float hier_thresh = 0.5);

    ~YOLODetector();

    std::vector<Detection> detect(const cv::Mat & image) override;
    char **namelables;
private:
    network *net;
    layer l;
    image im;
    image sized;
    int totalclass;
    float thresh;
    float hier_thresh;
    float nms;
    IplImage *frame;
    int nboxes;
    const int width;
    const int height;
    const int channel;
    void mat_into_im(const cv::Mat & image);
    std::map<int, std::string> carLable;//2:car,5:bus,7:truck
    cv::Rect detectROI;
};

#endif // YOLODETECTOR_H
