#include "PAOT.h"

#include "Affinity.h"
#include "predictor/kalman/KalmanPredictor.h"
#include <dlib/optimization.h>

// Methods

std::vector<Tracking> PAOT::track(const std::vector<Detection> &detections, cv::Mat &frame,
                                  std::vector<std::shared_ptr<ObjData> > &upload,
                                  std::queue<std::shared_ptr<Predictor>> &request) {
    frameCount++;

    // Filter detections on confidence
    std::vector<Detection> strongDetections;
    for (const auto &detection : detections) {
        if (detection.confidence > detectionThreshold) {
            strongDetections.push_back(detection);
        }
    }
    //std::cout << "predictors:" << predictors.size() << std::endl;
    Association association = associateDetectionsToPredictors(strongDetections, predictors,
                                                              Affinity::expCost, affinityThreshold);

    // Update matched predictors with assigned detections

    for (const auto &match : association.matching) {
        //std::cout << predictors.at(match.second)->getID();
        predictors.at(match.second)->update(strongDetections.at(match.first));
        //std::cout << " associate with " << predictors.at(match.second)->getID() << std::endl;
    }

    for (const auto p : association.unmatchedPredictors) {
        //std::cout << "unmatchedPredictions:"  << predictors.at(p)->getID()
                  //<< " predictDetection:" << predictors.at(p)->getPredictedNextDetection();
        predictors.at(p)->update();
        //std::cout << " predictNextDetection:" << predictors.at(p)->getPredictedNextDetection() << std::endl;
    }

    // Create and initialise new predictors for unmatched detections
    for (const auto id : association.unmatchedDetections) {
        auto predictor = std::make_shared<KalmanPredictor>(strongDetections.at(id), ++trackCount);
        predictors.push_back(predictor);
/*        std::cout << "new ID:" << predictor->getID()
                  << " Hits:" << predictor->getHitStreak()
                  << " detection" << strongDetections.at(id)
                  << " predictNextDetection:" << predictor->getPredictedNextDetection() << std::endl;
*/    }

    // Remove predictors that have been inactive for too long
    for(const auto & predict : predictors){
        if(predict->getTimeSinceUpdate() < 1 && predict->getHitStreak() >= minHits && !predict->isSaveImage){
            cv::Point2f tl = cv::Point2f(predict->currentBox.x1(), predict->currentBox.y1());
            cv::Point2f br = cv::Point2f(predict->currentBox.x2(), predict->currentBox.y2());
            tl.x = tl.x < 0 ? 0 : tl.x;
            tl.y = tl.y < 0 ? 0 : tl.y;
            br.x = br.x >= frame.cols ? frame.cols - 1 : br.x;
            br.y = br.y >= frame.rows ? frame.rows - 1 : br.y;
            cv::Rect roi = cv::Rect(tl, br);
            std::cout << "roi:(" << roi.tl().x << "," << roi.tl().y << "), (" << roi.br().x << "," << roi.br().y << ")" << std::endl;
            cv::Mat objImage = frame(roi).clone();
            cv::resize(objImage, objImage, cv::Size(48, 128));
            std::string imagePath = "/home/nvidia/lyl/QtProject/DetectAndTrack/objImage/" + std::to_string(predict->getID()) + ".jpg";
            cv::imwrite(imagePath, objImage);
            predict->imagePath = imagePath;
            predict->isSaveImage = true;
            std::cout << "success save object image:" << imagePath << std::endl;
            request.push(predict);
        }
        else if(predict->getTimeSinceUpdate()>maxAge&&predict->isSaveImage){
            /*std::string personID = cameraNum + "_" + std::to_string(predict->getID());
            std::cout << "delete ID:" << personID << std::endl;
            std::cout << "image:" << predict->imagePath << std::endl;
            std::cout << "trace:" << predict->objTrace.size() << std::endl;*/
            auto object = std::make_shared<ObjData>(predict);
            upload.push_back(object);
        }
    }
    predictors.erase(std::remove_if(
            predictors.begin(), predictors.end(),
            [this](const std::shared_ptr<Predictor> &predictor) {
                return predictor->getTimeSinceUpdate() > maxAge;
            }), predictors.end());

    // Return trackings from active predictors
    std::vector<Tracking> trackings;
    for (auto it = predictors.begin(); it != predictors.end(); ++it) {
        if ((*it)->getTimeSinceUpdate() < 1 &&
            ((*it)->getHitStreak() >= minHits || frameCount <= minHits)) {
            trackings.push_back((*it)->getTracking());
        }
    }
    return trackings;
}

PAOT::Association PAOT::associateDetectionsToPredictors(
        const std::vector<Detection> &detections,
        const std::vector<std::shared_ptr<Predictor>> &predictors,
        double (*affinityMeasure)(const BoundingBox &a, const BoundingBox &b),
        double affinityThreshold) {

    const int DOUBLE_PRECISION = 100;
    std::vector<std::pair<int, int>> matches;
    std::vector<int> unmatchedDetections;
    std::vector<int> unmatchedPredictors;

    if (predictors.empty()) {
        for (int i = 0; i < detections.size(); ++i)
            unmatchedDetections.push_back(i);
        return PAOT::Association{matches, unmatchedDetections, unmatchedPredictors};
    }

    dlib::matrix<int> cost(detections.size(), predictors.size());
    for (size_t row = 0; row < detections.size(); ++row) {
        for (size_t col = 0; col < predictors.size(); ++col) {
            cost(row, col) = int(DOUBLE_PRECISION * affinityMeasure(
                    detections.at(row).bb,
                    predictors.at(col)->getPredictedNextDetection().bb));
        }
    }

    // Zero-pad to make it square
    if (cost.nr() > cost.nc()) {
        cost = dlib::join_rows(cost, dlib::zeros_matrix<int>(1, cost.nr() - cost.nc()));
    } else if (cost.nc() > cost.nr()) {
        cost = dlib::join_cols(cost, dlib::zeros_matrix<int>(cost.nc() - cost.nr(), 1));
    }

    std::vector<long> assignment = dlib::max_cost_assignment(cost);

    // Filter out matches with low IoU, including those for indices from padding
    for (int d = 0; d < assignment.size(); ++d) {
        if (cost(d, assignment[d]) < affinityThreshold * DOUBLE_PRECISION) {
            if (d < detections.size()) {
                unmatchedDetections.push_back(d);
            }
            if (assignment[d] < predictors.size()) {
                unmatchedPredictors.push_back(int(assignment[d]));
            }
        } else {
            matches.push_back(std::pair<int, int>(d, assignment[d]));
        }
    }
    return PAOT::Association{matches, unmatchedDetections, unmatchedPredictors};
}
