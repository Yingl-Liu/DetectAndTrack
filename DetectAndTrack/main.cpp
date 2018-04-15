#include <iostream>
#include <chrono>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "util/DetectionFileParser.h"
#include "tracker/PAOT.h"
#include "tracker/Tracker.h"
#include "detector/YOLODetector.h"

extern "C"{
#include "darknet.h"
}
using namespace cv;
using namespace std;

/*typedef chrono::duration<double, milli> milliduration;

int getFileName(const string & directory, vector<string> & fileName){
    boost::filesystem::path fileDirectoryPath(directory);

    boost::filesystem::directory_iterator iter_end;
    for(boost::filesystem::directory_iterator iter_begin(fileDirectoryPath); iter_begin!=iter_end; ++iter_begin){
            fileName.push_back(iter_begin->path().string());
    }
    sort(fileName.begin(),fileName.end(), less<string>());
    return fileName.size();
}*/

int main()
{
///////////////////////////////////Detect///////////////////////////////////
    string datafile = "/home/lyl/darknet-master/cfg/coco.data";
    string cfgfile = "/home/lyl/darknet-master/cfg/yolov3.cfg";
    string weightfile = "/home/lyl/darknet-master/yolov3.weights";
    Mat image;
    VideoCapture video("/home/lyl/video/test.avi");
    video.read(image);
    PAOT tracker;
    namedWindow("predict", WINDOW_NORMAL);
    if(!video.isOpened()) {
        cout << "open video faild!" << endl;
        return 0;
    }
    char fps[20] = {0};
    int num = 0;
    double start = 0.0;
    YOLODetector detector(datafile, cfgfile, weightfile, image.cols, image.rows, image.channels(), 0.55);
    /*while(!image.empty()){
        start = what_time_is_it_now();
        auto dets = detector.detect(image);
        sprintf(fps, "FPS:%f", 1/(what_time_is_it_now() - start));
        cout << ++num << endl;
        for(const auto & c : dets){
            cv::rectangle(image, cv::Point2f(c.bb.x1(), c.bb.y1()), cv::Point2f(c.bb.x2(), c.bb.y2()), cv::Scalar(0, 0, 255));
            cv::putText(image, fps, cv::Point(10, 30), 0, 1, cv::Scalar(0 ,0 ,255));
            cout << c.namelabel << ":" << c.confidence << endl;
        }
        cv::imshow("predict", image);
        cv::waitKey(1);
        video.read(image);
    }*/

//////////////////////////////////Tracking//////////////////////////////////
/*    PAOT tracker;
    int frameNum = 0;
    string filePath = "../data/PETS09-S2L1/det.txt";
    string imgDirectory = "/home/nvidia/Documents/2DMOT2015/train/PETS09-S2L1/img1/";
    vector<string> imgPath;
    map<int, vector<Detection>> detect;
    map<int, vector<Detection>> (*FileParser)(ifstream &);
    int imgNum = getFileName(imgDirectory, imgPath);
    cout << "read:" << imgNum << endl;
    cv::Mat frame;

    FileParser = DetectionFileParser::parseMOTFile;
    ifstream DetectFile(filePath);
    if(DetectFile.is_open()){
        detect = FileParser(DetectFile);
        DetectFile.close();
    }

    milliduration duration = chrono::milliseconds::duration::zero();
    auto start = chrono::high_resolution_clock::now();

    for(int num = 0; num < detect.rbegin()->first; ++num){
        if(detect.find(num) != detect.end()){
            frame = cv::imread(imgPath[num]);
            vector<Tracking> trackings = tracker.track(detect.at(num));
            for(auto & t : trackings){
                cout << "Frame:" << num << " ID:" << t.ID << " BoundingBox:" << t.bb << endl;
                cv::putText(frame, to_string(t.ID), cv::Point2f(t.bb.cx, t.bb.cy), CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255 ,0 ,0));
            }

            for(const auto & c : detect.at(num)){
                cv::rectangle(frame, cv::Point2f(c.bb.x1(), c.bb.y1()), cv::Point2f(c.bb.x2(), c.bb.y2()), cv::Scalar(0, 0, 255));
            }
            cv::imshow("frame", frame);
            cv::waitKey(300);
        }
        ++frameNum;
    }

    auto end = chrono::high_resolution_clock::now();
    duration += chrono::duration_cast<chrono::high_resolution_clock::duration>(end - start);
    auto dura = chrono::duration_cast<chrono::milliseconds>(duration).count();
    cout << "Total Frame:" << frameNum << "FPS:" << (double(frameNum)*1000.0)/dura << endl;
*/
//////////////////////////////////DetectAndTracking//////////////////////////////////
    while(!image.empty()){
            start = what_time_is_it_now();
            cout << "Frame:" << ++num << endl;
            std::vector<Detection> dets = detector.detect(image);
            for(const Detection & c : dets){
                rectangle(image, Point2f(c.bb.x1(), c.bb.y1()), Point2f(c.bb.x2(), c.bb.y2()), Scalar(0, 0, 255));
                cout << c.namelabel << ":" << c.confidence << endl;
            }
            vector<Tracking> trackings = tracker.track(dets);
            for(const Tracking & t : trackings){
                putText(image, to_string(t.ID), Point2f(t.bb.cx, t.bb.cy), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255 ,255 ,0));
            }
            sprintf(fps, "FPS:%2f", 1/(what_time_is_it_now() - start));
            putText(image, fps, Point(10, 30), 0, 1, Scalar(0 ,0 ,255));
            imshow("predict", image);
            waitKey(1);
            video.read(image);
        }
    return 0;
}

