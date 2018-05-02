#ifndef OBJDATA_H
#define OBJDATA_H

#include "tracker/predictor/Predictor.h"
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

    int updateData(std::string IP);
    /*ouccure frames */
    int age;
    /*the id in database*/
    int db_id;

    std::string imagePath;

    std::string traceX;

    std::string traceY;

    std::vector<BoundingBox> objTrace;

    std::string objLable;

    int ID;

    BoundingBox currentBox;

private:
    void vectorToStr();
};

std::ostream &operator<<(std::ostream &os, const ObjData &obj);

std::string findPerson(std::string &IP, std::string &imagePath, std::string &cameraID);

CURLMcode multi_request(curl_httppost *formpost, CURL *curl, std::string &result);
#endif // OBJDATA_H
