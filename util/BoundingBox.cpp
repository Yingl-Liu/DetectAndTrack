#include "BoundingBox.h"

// Constructors
BoundingBox::BoundingBox():cx(-1),cy(-1),width(0),height(0),cols(1920),rows(1080){}

BoundingBox::BoundingBox(const BoundingBox & bb):cx(bb.cx),cy(bb.cy),width(bb.width),height(bb.height),cols(bb.cols),rows(bb.rows){}

BoundingBox::BoundingBox(double cx, double cy, double width, double height, int cols, int rows)
        : cx(cx), cy(cy), width(width), height(height), cols(cols), rows(rows) {}

// Functions

std::ostream &operator<<(std::ostream &os, const BoundingBox &bb) {
    os << "(" << bb.cx << "," << bb.cy << ") - " << bb.width << "x" << bb.height;
    return os;
}
