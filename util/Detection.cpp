#include "Detection.h"

// Constructors

Detection::Detection(const std::string & namelabel, double confidence, const BoundingBox &bb, int label, std::shared_ptr<cv::Mat> pimage)
        : namelabel(std::move(namelabel)), confidence(confidence), bb(bb), label(label), p_objImage(pimage) {
    extractFeature();
}

Detection::Detection(const Detection &rhs)
        : label(rhs.label), confidence(rhs.confidence), bb(rhs.bb),
          namelabel(rhs.namelabel), feature(rhs.feature), p_objImage(rhs.p_objImage) {}

Detection::Detection(Detection &&rhs)
        : label(rhs.label), confidence(rhs.confidence), bb(std::move(rhs.bb)),
          namelabel(std::move(rhs.namelabel)), feature(std::move(rhs.feature)), p_objImage(std::move(rhs.p_objImage)) {}

void Detection::updateBoundingBox(const BoundingBox &boundingbox){
    bb = boundingbox;
}

void Detection::updateImage(const Detection &rhs){
    p_objImage = rhs.p_objImage;
    feature = rhs.feature.clone();
}

void Detection::extractFeature(){
    cv::Mat h, s, v;
    int histSize[] = {bins[0] * bins[1] * bins[2]};
    cv::Mat splitImage[3], hsv;
    feature = cv::MatND::zeros(batchs[0]*batchs[1]*histSize[0], 1, CV_32F);
    cv::cvtColor(*p_objImage, hsv, cv::COLOR_BGR2HSV);
    cv::split(hsv, splitImage);
    quantization(splitImage[0], h, bins[0]);
    quantization(splitImage[1], s, bins[1]);
    quantization(splitImage[2], v, bins[2]);
    cv::Mat pattern = h*bins[1]*bins[2] + s*bins[2] + v;
    getHist(pattern, feature, histSize, batchs[0], batchs[1]);
}

void Detection::getHist(cv::Mat &input, cv::MatND &hist, int *histSize, int xbatch, int ybatch){
    float range[] = {0, histSize[0]};
    const float* ranges[] = { range };
    int channel[] = { 0 };
    int xStepSize = input.cols/xbatch, yStepSize = input.rows/ybatch;
    for(int j = 0; j < ybatch; j++){
        for(int i = 0; i < xbatch; i++){
            cv::Rect temp = cv::Rect(cv::Point(i*xStepSize, j*yStepSize),cv::Point((i+1)*xStepSize, (j+1)*yStepSize));
            cv::Mat roi = input(temp), temp_hist;
            cv::calcHist( &roi, 1, channel, cv::Mat(), temp_hist, 1, histSize, ranges, true, false );
            temp_hist.copyTo(hist.rowRange((j*xbatch+i)*histSize[0], (j*xbatch+i+1)*histSize[0]));
        }
    }
}

void Detection::quantization(cv::Mat &input, cv::Mat &output, int bins){
    double minVal, maxVal;
    cv::Mat double_image;
    input.convertTo(double_image, CV_32F);
    cv::minMaxLoc(input, &minVal, &maxVal);
    output = (bins*(double_image-minVal)/(maxVal-minVal))-0.51;
    output.convertTo(output, CV_16U);
}

// Functions

std::ostream &operator<<(std::ostream &os, const Detection &d) {
    os << "Label: " << d.label << " Confidence: " << d.confidence << " " << d.bb;
    return os;
}
