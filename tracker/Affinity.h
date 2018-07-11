#ifndef CPP_AFFINITY_H
#define CPP_AFFINITY_H


#include "../util/Detection.h"


class Affinity {
public:
    /**
     * Euclidian distance
     * Small value indicates similarity
     */
    static double euclDist(const BoundingBox &a, const BoundingBox &b);

    /**
     * Intersection over union
     * Large value indicates similarity
     * From: Simple Online and Realtime Tracking
     * (Alex Bewley, Zongyuan Ge, Lionel Ott, Fabio Ramos, Ben Upcroft)
     */
    static double iou(const BoundingBox &a, const BoundingBox &b);

    /**
     * Combination of exponential similarity in position and shape and HSV histogram feature
     * Small value indicates similarity
     * From: Online multi-target tracking with strong and weak detections
     * (Ricardo Sanchez-Matilla, Fabio Poiesi, Andrea Cavallaro Centre)
     */
    static double expAndFeaCost(const Detection &a, const Detection &b);

    /**
     * Combination of exponential similarity in position and shape
     * Large value indicates similarity
     * From: POI: Multiple Object Tracking with High Performance Detection and Appearance Feature
     * (Fengwei Yu, Wenbo Li, Quanquan Li, Yu Liu, Xiaohua Shi, Junjie Yan)
     * @param a BoundingBox from Detection
     * @param b BoundingBox from Predictor
     */
    static double expCost(const Detection &a, const Detection &b);

private:
    // Prevent instantiation
    Affinity() {};

    static double CosDistance(const cv::Mat &feature1, const cv::Mat &feature2);
};


#endif //CPP_AFFINITY_H
