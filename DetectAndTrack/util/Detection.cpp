#include "Detection.h"

// Constructors

Detection::Detection(int label, double confidence, const BoundingBox &bb, const std::string & namelabel)
        : label(label), confidence(confidence), bb(bb), namelabel(namelabel) {}

Detection::Detection(const Detection &rhs)
        : label(rhs.label), confidence(rhs.confidence), bb(rhs.bb), namelabel(rhs.namelabel) {}

Detection::Detection(Detection &&rhs)
        : label(rhs.label), confidence(rhs.confidence), bb(std::move(rhs.bb)), namelabel(std::move(rhs.namelabel)) {}


// Functions

std::ostream &operator<<(std::ostream &os, const Detection &d) {
    os << "Label: " << d.label << " Confidence: " << d.confidence << " " << d.bb;
    return os;
}
