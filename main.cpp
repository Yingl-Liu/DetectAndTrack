#include <iostream>
#include <chrono>
#include <opencv2/highgui.hpp>
#include "util/DetectionFileParser.h"
#include "tracker/PAOT.h"
#include "tracker/Tracker.h"
#include "detector/YOLODetector.h"
#include <boost/filesystem.hpp>

extern "C"{
#include "darknet.h"
extern detection* detect_in_ipl(IplImage *input, int *nboxes, char **names, int *classes, char *datafile, char *cfgfile, char *weightfile, float thresh, float hier_thresh);
}

using namespace std;

typedef chrono::duration<double, milli> milliduration;
pthread_mutex_t mutexUpload;
pthread_mutex_t mutexRequest;
pthread_mutex_t mutexRead;

bool isContinue = true;
std::string uploadIP = "http://192.168.153.158:8000/person/";
std::string requestIP = "http://192.168.153.158:8000/findperson/";
cv::VideoCapture video;
std::string cameraNum = "1";
std::vector<std::shared_ptr<ObjData>> upload;
std::queue<std::shared_ptr<Predictor>> request;
std::queue<std::shared_ptr<cv::Mat>> frames;

int getFileName(const string & directory, vector<string> & fileName){
    boost::filesystem::path fileDirectoryPath(directory);

    boost::filesystem::directory_iterator iter_end;
    for(boost::filesystem::directory_iterator iter_begin(fileDirectoryPath); iter_begin!=iter_end; ++iter_begin){
            fileName.push_back(iter_begin->path().string());
    }
    sort(fileName.begin(),fileName.end(), less<string>());
    return fileName.size();
}

void *uploadThread(void*){
    bool isUpload = false;
    std::vector<ObjData> data;
    while(isContinue){
        pthread_mutex_lock(&mutexUpload);
        if(upload.size() > 0){
            std::cout << "upload.size:" << upload.size() << std::endl;
            for(auto p : upload){
                data.push_back(*p);
            }
            upload.erase(upload.begin(), upload.end());
            isUpload = true;
        }
        pthread_mutex_unlock(&mutexUpload);

        if(isUpload){
            for(auto obj : data){
//                obj.PostStructData(uploadIP, cameraNum);
                std::cout << obj << std::endl;
            }
            data.clear();
            isUpload = false;
        }
        sleep(1);
    }
}

void *requestThread(void*){
    bool isRequest = false;
    std::string path;
    while(isContinue){
        pthread_mutex_lock(&mutexRequest);
        if(!request.empty()){
            path = request.front()->imagePath;
            isRequest = true;
        }
        pthread_mutex_unlock(&mutexRequest);
        if(isRequest){
            std::string persionID = findPerson(requestIP, path, cameraNum);
            std::cout << "persionID:" << persionID << std::endl;
            pthread_mutex_lock(&mutexRequest);
            request.front()->objName = persionID;
            request.pop();
            pthread_mutex_unlock(&mutexRequest);
            isRequest = false;
        }
    }
}

void *readThread(void*){
    while(isContinue){
        cv::Mat temFrame;
        video>>temFrame;
        auto tmp = std::make_shared<cv::Mat>(temFrame);
        pthread_mutex_lock(&mutexRead);
        if(frames.size()<15){
            frames.push(tmp);
        }
        else{
            frames.pop();
            frames.push(tmp);
        }
        pthread_mutex_unlock(&mutexRead);
    }
}

int main()
{
///////////////////////////////////Detect///////////////////////////////////
    string datafile = "/home/nvidia/lyl/yolov3/cfg/coco.data";
    string cfgfile = "/home/nvidia/lyl/yolov3/cfg/yolov3.cfg";
    string weightfile = "/home/nvidia/lyl/yolov3/yolov3.weights";
    video.open("rtmp://192.168.153.158:1935/live/202");
    cv::Size videoSize((int)video.get(CV_CAP_PROP_FRAME_WIDTH), (int)video.get(CV_CAP_PROP_FRAME_HEIGHT));
    cv::VideoWriter writer("../result2.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25, videoSize);
    pthread_t upload_id, request_id, read_id;
    cv::Mat image;
    video.read(image);
    PAOT tracker;
    cv::namedWindow("predict");
    if(!video.isOpened()) {
        cout << "open video faild!" << endl;
        return 0;
    }
    char fps[20] = {0};
    int num = 0;
    double start = 0.0;
    pthread_mutex_init(&mutexUpload, NULL);
    pthread_mutex_init(&mutexRequest, NULL);
    pthread_mutex_init(&mutexRead, NULL);
    pthread_create(&read_id, NULL, readThread, NULL);
    //pthread_create(&upload_id, NULL, uploadThread, NULL);
    //pthread_create(&request_id, NULL, requestThread, NULL);

    YOLODetector detector(datafile, cfgfile, weightfile, image.cols, image.rows, image.channels(), 0.6);
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
    while(isContinue){
            start = what_time_is_it_now();
            cout << "Frame:" << ++num << endl;
            auto dets = detector.detect(image);

            pthread_mutex_lock(&mutexUpload);
            pthread_mutex_lock(&mutexRequest);
            vector<Tracking> trackings = tracker.track(dets, image, upload, request);
            pthread_mutex_unlock(&mutexUpload);
            pthread_mutex_unlock(&mutexRequest);

            for(const auto & c : dets){
                cv::rectangle(image, cv::Point2f(c.bb.x1(), c.bb.y1()), cv::Point2f(c.bb.x2(), c.bb.y2()), cv::Scalar(0, 0, 255));
//                cout << c.namelabel << ":" << c.confidence << endl;
            }

            for(const auto & t : trackings){
                //std::cout << "PutText:" << t.trackingName << std::endl;
                cv::putText(image, std::to_string(t.ID), cv::Point2f(t.bb.x1(), t.bb.y1() + 20), CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255 ,255 ,0));
                cv::rectangle(image, cv::Point2f(t.bb.x1(), t.bb.y1()), cv::Point2f(t.bb.x2(), t.bb.y2()), cv::Scalar(255, 0, 0));
            }
            writer << image;
            sprintf(fps, "FPS:%2f", 1/(what_time_is_it_now() - start));
            cv::putText(image, fps, cv::Point(10, 30), 0, 1, cv::Scalar(0 ,0 ,255));
            cv::imshow("predict", image);
            cv::waitKey(1);
            pthread_mutex_lock(&mutexRead);
            if(!frames.empty()){
                image = frames.front()->clone();
                frames.pop();
            }
            else{
                isContinue = false;
            }
            pthread_mutex_unlock(&mutexRead);
//            video.read(image);
        }
    pthread_join(read_id, NULL);
    //pthread_join(upload_id, NULL);
    //pthread_join(request_id, NULL);
    return 0;
}

