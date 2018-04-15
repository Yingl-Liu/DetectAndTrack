#include "BoundingBox.h"

// Constructors

BoundingBox::BoundingBox(double cx, double cy, double width, double height)
        : cx(cx), cy(cy), width(width), height(height) {}

//BoundingBox::BoundingBox(int left, int top, int right, int bot)
//        : left(left), top(top), right(right), bot(bot) {}
// Functions

std::ostream &operator<<(std::ostream &os, const BoundingBox &bb) {
    os << "(" << bb.cx << "," << bb.cy << ") - " << bb.width << "x" << bb.height;
    return os;
}
