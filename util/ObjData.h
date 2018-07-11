#ifndef OBJDATA_H
#define OBJDATA_H

#include "../tracker/Predictor.h"
#include <curl/curl.h>
#include <iostream>

class ObjData
{
public:
    ObjData();

    ObjData(std::shared_ptr<Predictor> predict);

    ObjData(const ObjData & obj);

    ObjData(ObjData && obj);

    int PostStructData(std::string &postIP, std::string &cameraID);

    int updateData(const std::string &IP, std::string &cameraID);
    /*ouccure frames */
    int age;
    /*the id in database*/
    int db_id;

    int ID;

    std::string imagePath;

    std::string traceX;

    std::string traceY;

    std::vector<BoundingBox> objTrace;

    std::string objLable;//class "person" or "car" or "bus" or "trunk"

    std::string objName;//

    BoundingBox currentBox;//use yolo detect bounding box

private:
    void vectorToStr();
};

std::ostream &operator<<(std::ostream &os, const ObjData &obj);

std::string _findPerson(std::string &IP, std::string &imagePath, std::string &cameraID, int threshold, int seconds);

std::pair<int, std::string> findPerson(const std::string &IP, std::string &imagePath, std::string &cameraID, int threshold, int seconds);

CURLMcode multi_request(curl_httppost *formpost, CURL *curl, std::string &result);
#endif // OBJDATA_H
