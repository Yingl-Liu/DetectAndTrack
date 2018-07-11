#ifndef CPP_TRACKING_H
#define CPP_TRACKING_H


#include "BoundingBox.h"

#include <ostream>

struct Tracking {
    Tracking(std::string label, int ID, const BoundingBox &bb);

    Tracking(std::string label, int ID, const BoundingBox &bb, std::string trackingName);

    Tracking(const Tracking &rhs);

    Tracking(Tracking &&rhs);

    // Prevent assignment
    Tracking &operator=(const Tracking &rhs) = delete;

    Tracking &operator=(Tracking &&rhs) = delete;

    const std::string label;//class "P" or "car"
    const int ID;
    const BoundingBox bb;
    std::string trackingName;//this object name. "car" + "_" + "ID" or "camera id" + "_" + "db_id"
};

std::ostream &operator<<(std::ostream &os, const Tracking &t);


#endif //CPP_TRACKING_H
