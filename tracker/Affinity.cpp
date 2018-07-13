#include "Affinity.h"

#include <cmath>

double Affinity::euclDist(const BoundingBox &a, const BoundingBox &b) {
    return std::sqrt(std::pow(a.cx - b.cx, 2) + std::pow(a.cy - b.cy, 2));
}

double Affinity::iou(const BoundingBox &a, const BoundingBox &b) {
    double xx1 = std::max(a.x1(), b.x1());
    double yy1 = std::max(a.y1(), b.y1());
    double xx2 = std::min(a.x2(), b.x2());
    double yy2 = std::min(a.y2(), b.y2());
    double width = std::max(xx2 - xx1, 0.);
    double height = std::max(yy2 - yy1, 0.);
    double intersection = width * height;
    double uni = a.area() + b.area() - intersection;
    return intersection / uni;
}

double Affinity::expAndFeaCost(const Detection &a, const Detection &b) {
    double positionWeight = 0.5;
    double shapeWeight = 1.5;
    double positionCost = std::exp(-positionWeight * (
            std::pow((a.bb.cx - b.bb.cx) / a.bb.width, 2) +
            std::pow((a.bb.cy - b.bb.cy) / a.bb.height, 2)));
    double shapeCost = std::exp(-shapeWeight * (
            std::abs(a.bb.width - b.bb.width) / (a.bb.width + b.bb.width) +
            std::abs(a.bb.height - b.bb.height) / (a.bb.height + b.bb.height)));
//    double featureCost = 1 / (1 + std::exp(12 * (0.7 - CosDistance(a.feature, b.feature))));
    double featureCost = CosDistance(a.feature, b.feature);
    std::cout << " featureCost:" << featureCost << " positionAndShapeCost:" << 100 * positionCost * shapeCost << std::endl;
    return positionCost * shapeCost * featureCost;
}

double Affinity::expCost(const Detection &a, const Detection &b) {
    // It is expected that a is detection and b is prediction
    double positionWeight = 0.5;
    double shapeWeight = 1.5;
    double positionCost = std::exp(-positionWeight * (
            std::pow((a.bb.cx - b.bb.cx) / a.bb.width, 2) +
            std::pow((a.bb.cy - b.bb.cy) / a.bb.height, 2)));
    double shapeCost = std::exp(-shapeWeight * (
            std::abs(a.bb.width - b.bb.width) / (a.bb.width + b.bb.width) +
            std::abs(a.bb.height - b.bb.height) / (a.bb.height + b.bb.height)));
    return positionCost * shapeCost;
}

double Affinity::CosDistance(const cv::Mat &feature1, const cv::Mat &feature2){
    double innerProduct = feature1.dot(feature2);
    double norm = sqrt(feature1.dot(feature1)*feature2.dot(feature2));
    return innerProduct/norm;
}
