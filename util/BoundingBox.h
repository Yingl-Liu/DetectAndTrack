#ifndef CPP_BOUNDINGBOX_H
#define CPP_BOUNDINGBOX_H


#include <ostream>

struct BoundingBox {
    BoundingBox();
    BoundingBox(const BoundingBox & bb);
    BoundingBox(double cx, double cy, double width, double height);

    double cx;
    double cy;
    double width;
    double height;

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
        double brx = (cx + width / 2) > 1920 ? 1919 : (cx + width / 2);
        return brx;
    }
    inline double y2() const {
        double bry = (cy + height / 2) > 1080 ? 1079 : (cy + height / 2);
        return bry;
    }
    inline double area() const { return width * height; }
    inline double ratio() const { return width / height; }

    BoundingBox & operator =(const BoundingBox & bb){
        cx = bb.cx;
        cy = bb.cy;
        width = bb.width;
        height = bb.height;
    }
};

std::ostream &operator<<(std::ostream &os, const BoundingBox &bb);


#endif //CPP_BOUNDINGBOX_H
