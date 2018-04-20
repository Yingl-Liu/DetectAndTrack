#include "Tracking.h"

// Constructors

Tracking::Tracking(int label, int ID, const BoundingBox &bb)
        : label(label), ID(ID), bb(bb) {}

Tracking::Tracking(int label, int ID, const BoundingBox &bb, std::string trackingName)
        : label(label), ID(ID), bb(bb),trackingName(trackingName) {}

Tracking::Tracking(const Tracking &rhs)
        : label(rhs.label), ID(rhs.ID), bb(rhs.bb),trackingName(rhs.trackingName) {}

Tracking::Tracking(Tracking &&rhs)
        : label(std::move(rhs.label)), ID(std::move(rhs.ID)), bb(std::move(rhs.bb)), trackingName(std::move(rhs.trackingName)) {}

// Functions

std::ostream &operator<<(std::ostream &os, const Tracking &t) {
    os << "Label: " << t.label << " ID: " << t.ID << " " << t.bb;
    return os;
}
