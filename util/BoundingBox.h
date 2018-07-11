#ifndef CPP_BOUNDINGBOX_H
#define CPP_BOUNDINGBOX_H

#include <opencv2/core/core.hpp>
#include <ostream>

struct BoundingBox {
    BoundingBox();
    BoundingBox(const BoundingBox & bb);
    BoundingBox(double cx, double cy, double width, double height, int cols = 1920, int rows = 1080);

    double cx;
    double cy;
    double width;
    double height;
    int cols;
    int rows;

    // (x1,y1) - top left
    // (x2,y2) - bottom right
    inline double x1() const {
        double tlx = (cx - width / 2) < 0 ? 0 : (cx - width / 2);
        return tlx;
    }
    inline double y1() const {
        double tly = (cy - height / 2) < 0 ? 0 : (cy - height / 2);
        return tly;
    }
    inline double x2() const {
        double brx = (cx + width / 2) > cols ? cols - 1 : (cx + width / 2);
        return brx;
    }
    inline double y2() const {
        double bry = (cy + height / 2) > rows ? rows - 1 : (cy + height / 2);
        return bry;
    }
    inline double area() const { return width * height; }
    inline double ratio() const { return width / height; }

    cv::Rect rect(){
        return cv::Rect(cv::Point2f(x1(), y1()), cv::Point2f(x2(), y2()));
    }

    BoundingBox & operator =(const BoundingBox & bb){
        cx = bb.cx;
        cy = bb.cy;
        width = bb.width;
        height = bb.height;
    }
};

std::ostream &operator<<(std::ostream &os, const BoundingBox &bb);


#endif //CPP_BOUNDINGBOX_H
