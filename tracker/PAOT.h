#ifndef CPP_PAOT_H
#define CPP_PAOT_H


#include "Tracker.h"
#include "Predictor.h"
#include "KalmanPredictor.h"

#include <memory>
#include <vector>

class PAOT : public Tracker {
    struct Association;

public:
    PAOT() = default;

    virtual ~PAOT() = default;

    /**
      * Uses a linear velocity Kalman filters to predict locations of objects from previous frame.
      * Associates detections to Kalman filters using an Affinity measure and the Hungarian Algorithm.
      */
    std::vector<Tracking> track(const std::vector<Detection> &detections, cv::Mat & frame,
                                std::vector<std::shared_ptr<ObjData> > &upload,
                                std::queue<std::shared_ptr<Predictor> > &request) override;

private:
    const int maxAge = 8; // Original: 1
    const int minHits = 2; // Original: 3
    const int carMaxAge = 3;
    const int carMinHits = 1;
    const double detectionThreshold = 0.4;
    const double affinityThreshold = 0.1;
    const double carAffinityThreshold = 0.001;
    std::vector<std::shared_ptr<Predictor>> predictors;
    std::vector<std::shared_ptr<Predictor>> carPredictors;
    int trackCount = 0;
    int carTrackCount = 0;
    int frameCount = 0;

    /**
     * Uses an Affinity measure and Hungarian algorithm to determine which detection corresponds to which Kalman filter.
     */
    static Association associateDetectionsToPredictors(
            const std::vector<Detection> &detections,
            const std::vector<std::shared_ptr<Predictor>> &predictors,
            double (*affinityMeasure)(const Detection &a, const Detection &b),
            double affinityThreshold);
    /**
     * Update matched predictors with assigned detections
     */
    void updateAssociations(Association &association,
                            std::vector<std::shared_ptr<Predictor>> &predictors,
                            std::vector<Detection> &detections,
                            cv::Mat &frame,
                            int &track_count,
                            std::vector<Tracking> &trackings,
                            const int maxage,
                            const int minhits,
                            std::vector<std::shared_ptr<ObjData> > &upload,
                            std::queue<std::shared_ptr<Predictor> > &request);

    struct Association {
        std::vector<std::pair<int, int>> matching;
        std::vector<int> unmatchedDetections;
        std::vector<int> unmatchedPredictors;
    };
};

#endif //CPP_MCSORT_H
