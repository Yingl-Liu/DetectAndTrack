#ifndef CPP_DETECTION_H
#define CPP_DETECTION_H


#include "BoundingBox.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <memory>

struct Detection {
    Detection(const std::string &namelabel, double confidence, const BoundingBox &bb, int label, std::shared_ptr<cv::Mat> pimage);

    Detection(const Detection &rhs);

    Detection(Detection &&rhs);

    void updateBoundingBox(const BoundingBox &boundingbox);

    void updateImage(const Detection &rhs);
    // Prevent assignment

    Detection &operator=(const Detection &rhs) = delete;

    Detection &operator=(Detection &&rhs) = delete;
    std::shared_ptr<cv::Mat> p_objImage;
    int label;//the number in name.label
    std::string namelabel;//class car or person
    double confidence;
    BoundingBox bb;
    cv::MatND feature;

    int bins[3] = {4, 4, 1};//HSV bins
    int batchs[2] = {1, 4};//x : 1;y 2

private:
    void extractFeature();
    void getHist(cv::Mat &input, cv::MatND &hist, int *histSize, int xbatch = 1, int ybatch = 2);
    void quantization(cv::Mat &input, cv::Mat &output, int bins);
};

std::ostream &operator<<(std::ostream &os, const Detection &d);


#endif //CPP_DETECTION_H
