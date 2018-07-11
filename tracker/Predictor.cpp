#include "Predictor.h"

// Constructors

Predictor::Predictor(std::string label, int ID, int classes, Detection &detection)
        : label(label), ID(ID), classes(classes), timeSinceUpdate(0), hitStreak(0), detection(detection) {}

Predictor::Predictor(Predictor &&rhs)
        : label(std::move(rhs.label)), ID(rhs.ID), classes(rhs.classes), timeSinceUpdate(rhs.timeSinceUpdate),
          hitStreak(rhs.hitStreak), detection(std::move(rhs.detection)) {}

Predictor &Predictor::operator=(Predictor &&rhs) {
    label = rhs.label;
    ID = rhs.ID;
    timeSinceUpdate = rhs.timeSinceUpdate;
    hitStreak = rhs.hitStreak;
    classes = rhs.classes;
    p_objImage = rhs.p_objImage;
    return *this;
}

// Methods

void Predictor::update() {
    timeSinceUpdate++;
    hitStreak = 0;
}

void Predictor::update(const Detection &det) {
    timeSinceUpdate = 0;
    hitStreak++;
    objTrace.push_back(det.bb);
    detection.updateImage(det);
    //std::cout << "feature:" << detection.feature << std::endl;
}

// Getters

int Predictor::getTimeSinceUpdate() const {
    return timeSinceUpdate;
}

int Predictor::getHitStreak() const {
    return hitStreak;
}

std::string Predictor::getLabel() const {
    return label;
}

int Predictor::getID() const {
    return ID;
}

// Functions
//used to pretict the next stat
BoundingBox Predictor::stateToBoundingBox(const dlib::matrix<double, numStates, 1> &state) {
    double rectifiedArea = std::max(state(2), 0.);
    double width = std::sqrt(rectifiedArea * state(3));
    double height = rectifiedArea / width;
    return BoundingBox(state(0), state(1), width, height);
}
//used to update de model
dlib::matrix<double, Predictor::numObservations, 1> Predictor::boundingBoxToMeas(const BoundingBox &bb) {
    dlib::matrix<double, Predictor::numObservations, 1> z;
    z = bb.cx, bb.cy, bb.area(), bb.ratio();
    return z;
}

/*std::ostream &operator<<(std::ostream &os, const Predictor &p) {
    os << p.getPredictedNextDetection();
    return os;
}*/
