#include "PAOT.h"

#include "Affinity.h"
#include "KalmanPredictor.h"
const std::string currentPath = "../objImage/";
// Methods

std::vector<Tracking> PAOT::track(const std::vector<Detection> &detections, cv::Mat &frame,
                                  std::vector<std::shared_ptr<ObjData> > &upload,
                                  std::queue<std::shared_ptr<Predictor>> &request) {
    frameCount++;

    // Filter detections on confidence
    std::vector<Detection> strongDetections;
    std::vector<Detection> carStrongDetections;
    for (const auto &detection : detections) {
        if (detection.confidence > detectionThreshold && 0 == detection.label) {
            strongDetections.push_back(detection);
        }
        else if(detection.confidence > detectionThreshold && 2 == detection.label){
            carStrongDetections.push_back(detection);
//            std::cout << "car num:" << carStrongDetections.size() << std::endl;
        }
    }
    //std::cout << "predictors:" << predictors.size() << std::endl;
    Association association = associateDetectionsToPredictors(strongDetections, predictors,
                                                              Affinity::expAndFeaCost, affinityThreshold);

    Association carAssociation = associateDetectionsToPredictors(carStrongDetections, carPredictors,
                                                                 Affinity::expAndFeaCost, carAffinityThreshold);
//    std::cout << "unmatchedDetections size:" << carAssociation.unmatchedDetections.size() << std::endl;

    std::vector<Tracking> trackings;
    updateAssociations(association, predictors, strongDetections,
                       frame, trackCount, trackings, maxAge, minHits, upload, request);
    updateAssociations(carAssociation, carPredictors, carStrongDetections,
                       frame, carTrackCount, trackings, carMaxAge, carMinHits, upload, request);

    std::cout << "predictors:" << predictors.size() << std::endl;
    std::cout << "carPredictors:" << carPredictors.size() << std::endl;
    return trackings;
}

PAOT::Association PAOT::associateDetectionsToPredictors(
        const std::vector<Detection> &detections,
        const std::vector<std::shared_ptr<Predictor>> &predictors,
        double (*affinityMeasure)(const Detection &a, const Detection &b),
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
            printf("(%d, %d) ", row, col);
            cost(row, col) = int(DOUBLE_PRECISION *
                                 affinityMeasure(detections.at(row), predictors.at(col)->getPredictedNextDetection()));
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
    printf("matches:\n");
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
            printf("(%d, %d)", d, assignment[d]);
        }
    }
    return PAOT::Association{matches, unmatchedDetections, unmatchedPredictors};
}

void PAOT::updateAssociations(Association &association,
                              std::vector<std::shared_ptr<Predictor> > &predictors,
                              std::vector<Detection> &detections,
                              cv::Mat &frame,
                              int &track_count,
                              std::vector<Tracking> &trackings,
                              const int maxage,
                              const int minhits,
                              std::vector<std::shared_ptr<ObjData> > &upload,
                              std::queue<std::shared_ptr<Predictor> > &request){
    for(const auto &match : association.matching){
        predictors.at(match.second)->update(detections.at(match.first));
    }
    for(const auto &p : association.unmatchedPredictors){
        predictors.at(p)->update();
    }
    for(const auto &p : association.unmatchedDetections){
        if(detections.at(p).bb.area()>100){
            auto predict = std::make_shared<KalmanPredictor>(detections.at(p), ++track_count);
            predictors.push_back(predict);
        }
    }
    for(const auto predict : predictors){
        if(predict->getTimeSinceUpdate() < 1 && predict->getHitStreak() >= minhits && !predict->isSaveImage){
            /*cv::Point2f tl = cv::Point2f(predict->currentBox.x1(), predict->currentBox.y1());
            cv::Point2f br = cv::Point2f(predict->currentBox.x2(), predict->currentBox.y2());
            tl.x = tl.x < 0 ? 0 : tl.x;
            tl.y = tl.y < 0 ? 0 : tl.y;
            br.x = br.x >= frame.cols ? frame.cols - 1 : br.x;
            br.y = br.y >= frame.rows ? frame.rows - 1 : br.y;
            cv::Rect roi = cv::Rect(tl, br);
            cv::Mat objImage = frame(roi).clone();*/
            cv::Mat objImage = predict->detection.p_objImage->clone();
            if(predict->getLabel() == std::string("person"))
                cv::resize(objImage, objImage, cv::Size(48, 128));
            else
                cv::resize(objImage, objImage, cv::Size(128, 64));
            std::string imagePath = currentPath + predict->getLabel() + "_" + std::to_string(predict->getID()) + ".jpg";
            cv::imwrite(imagePath, objImage);
            predict->imagePath = imagePath;
            predict->isSaveImage = true;
            std::cout << "success save object image:" << imagePath << std::endl;
            if(predict->getLabel() == std::string("person"))
                request.push(predict);
        }
        else if(predict->getTimeSinceUpdate()>maxage && predict->isSaveImage){
            auto object = std::make_shared<ObjData>(predict);
            upload.push_back(object);
        }
    }

    for(auto itor = predictors.begin(); itor != predictors.end(); itor++){
        if ((*itor)->getTimeSinceUpdate() > maxage){
            itor = predictors.erase(itor);
            itor--;
        }
    }

    for (auto it = predictors.begin(); it != predictors.end(); ++it) {
        if ((*it)->getTimeSinceUpdate() < 1 &&
            ((*it)->getHitStreak() >= minhits || frameCount <= minhits)) {
            trackings.push_back((*it)->getTracking());
        }
    }
}
