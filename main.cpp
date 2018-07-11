#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <chrono>
#include <opencv2/highgui.hpp>
#include "tracker/PAOT.h"
#include "tracker/Tracker.h"
#include "detector/YOLODetector.h"

typedef std::chrono::duration<double, std::milli> milliduration;
pthread_mutex_t mutexUpload;
pthread_mutex_t mutexRequest;
pthread_mutex_t mutexRead;
const std::string rtsp_211 = "rtsp://admin:admin@12345@192.168.153.211:554/h264/ch1/av_stream";
const std::string rtmp = "rtmp://192.168.153.117:1935/myapp/test4";
const std::string videoFile = "/home/lyl/video/Jogging.avi";
const std::string IP = "http://192.168.153.117:8000/";
std::string cameraNum = "2";
int threshold = 90;
int seconds = 90;
bool isContinue = true;
cv::VideoCapture video;
std::vector<std::shared_ptr<ObjData>> upload;
std::queue<std::shared_ptr<Predictor>> request;
std::queue<std::shared_ptr<cv::Mat>> frames;

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
                if(std::string("person") != obj.objLable||obj.db_id>0)
                    obj.updateData(IP, cameraNum);
                std::remove(obj.imagePath.c_str());
            }
            data.clear();
            isUpload = false;
        }
        usleep(100000);
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
            auto personID = findPerson(IP, path, cameraNum, threshold, seconds);
            pthread_mutex_lock(&mutexRequest);
            request.front()->db_id = personID.first;
            request.front()->objName = personID.second;
            request.pop();
            pthread_mutex_unlock(&mutexRequest);
            isRequest = false;
        }
    }
}

void *readThread(void*){
    while(isContinue){
        cv::Mat temFrame;
        if(video.read(temFrame)){
            auto tmp = std::make_shared<cv::Mat>(temFrame);
            pthread_mutex_lock(&mutexRead);
            if(frames.size()<25){
                frames.push(tmp);
            }
            else{
                frames.pop();
                frames.push(tmp);
            }
            pthread_mutex_unlock(&mutexRead);
            usleep(300000);
        }
        else{
            std::cout << "Read camera failed!................" << std::endl;
            break;
        }
    }
}

int main()
{
    std::string datafile = "../cfg/coco.data";
    std::string cfgfile = "../cfg/yolov3.cfg";
    std::string weightfile = "../yolov3.weights";
    video.open(videoFile);
    //cv::Size videoSize((int)video.get(CV_CAP_PROP_FRAME_WIDTH), (int)video.get(CV_CAP_PROP_FRAME_HEIGHT));
    //cv::VideoWriter writer("../result.avi", CV_FOURCC('M', 'J', 'P', 'G'), 20, videoSize);
    pthread_t upload_id, request_id, read_id;
    cv::Mat image;
    video.read(image);
    PAOT tracker;
    cv::namedWindow("result", CV_WINDOW_NORMAL);
    if(!video.isOpened()) {
        std::cout << "open video faild!" << std::endl;
        return 0;
    }
    char fps[20] = {0};
    int num = 0;
    double start = 0.0;
    //pthread_mutex_init(&mutexUpload, NULL);
    //pthread_mutex_init(&mutexRequest, NULL);
    pthread_mutex_init(&mutexRead, NULL);
    pthread_create(&read_id, NULL, readThread, NULL);
    //pthread_create(&upload_id, NULL, uploadThread, NULL);
    //pthread_create(&request_id, NULL, requestThread, NULL);

    YOLODetector detector(datafile, cfgfile, weightfile, image.cols, image.rows, image.channels(), 0.6);

    while(isContinue){
            start = what_time_is_it_now();
            std::cout << "Frame:" << ++num << std::endl;
            auto dets = detector.detect(image);
            //pthread_mutex_lock(&mutexUpload);
            //pthread_mutex_lock(&mutexRequest);
            std::vector<Tracking> trackings = tracker.track(dets, image, upload, request);
            //pthread_mutex_unlock(&mutexUpload);
            //pthread_mutex_unlock(&mutexRequest);
            for(const auto & c : dets){
//                if(c.label == 0)
                    cv::rectangle(image, cv::Point2f(c.bb.x1(), c.bb.y1()), cv::Point2f(c.bb.x2(), c.bb.y2()), cv::Scalar(0, 0, 255));
            }

            for(const auto & t : trackings){
                //cv::putText(image, t.trackingName, cv::Point2f(t.bb.x1(), t.bb.y1() + 20), 0, 0.5, cv::Scalar(255 ,255 ,0));
                cv::putText(image, std::to_string(t.ID), cv::Point2f(t.bb.x1(), t.bb.y1() + 20), 0, 0.5, cv::Scalar(255 ,255 ,0));
                cv::rectangle(image, cv::Point2f(t.bb.x1(), t.bb.y1()), cv::Point2f(t.bb.x2(), t.bb.y2()), cv::Scalar(255, 0, 0));
            }
            //writer << image;
            std::sprintf(fps, "FPS:%2f", 1/(what_time_is_it_now() - start));
            cv::putText(image, fps, cv::Point(10, 30), 0, 0.4, cv::Scalar(0 ,0 ,255));
            cv::imshow("result", image);
            cv::waitKey(1);
            pthread_mutex_lock(&mutexRead);
            if(!frames.empty()){
                image = frames.front()->clone();
                frames.pop();
            }
            else{
                isContinue = false;
                std::cout << "read frame failed!!!!!!!" << std::endl;
            }
            pthread_mutex_unlock(&mutexRead);
        }
    pthread_join(read_id, NULL);
    //pthread_join(upload_id, NULL);
    //pthread_join(request_id, NULL);
    return 0;
}

