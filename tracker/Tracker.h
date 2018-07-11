#ifndef CPP_TRACKER_H
#define CPP_TRACKER_H


#include "../util/Detection.h"
#include "../util/Tracking.h"
#include "Predictor.h"
#include "../util/ObjData.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <queue>

class Tracker {
public:
    Tracker() = default;

    virtual ~Tracker() = default;

    // Prevent copying and moving
    Tracker(const Tracker &) = delete;

    Tracker(Tracker &&) = delete;

    Tracker &operator=(const Tracker &) = delete;

    Tracker &operator=(Tracker &&) = delete;

    /**
     * For each timestep, tracks the given Detections.
     * Returns the Trackings.
     */
    virtual std::vector<Tracking> track(const std::vector<Detection> &detections, cv::Mat & frame,
                                        std::vector<std::shared_ptr<ObjData>> &upload,
                                        std::queue<std::shared_ptr<Predictor>> &request) = 0;
};


#endif //CPP_TRACKER_H
