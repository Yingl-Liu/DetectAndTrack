#include "BoundingBox.h"

// Constructors
BoundingBox::BoundingBox():cx(-1),cy(-1),width(0),height(0){}

BoundingBox::BoundingBox(const BoundingBox & bb):cx(bb.cx),cy(bb.cy),width(bb.width),height(bb.height){}

BoundingBox::BoundingBox(double cx, double cy, double width, double height)
        : cx(cx), cy(cy), width(width), height(height) {}

// Functions

std::ostream &operator<<(std::ostream &os, const BoundingBox &bb) {
    os << "(" << bb.cx << "," << bb.cy << ") - " << bb.width << "x" << bb.height;
    return os;
}
