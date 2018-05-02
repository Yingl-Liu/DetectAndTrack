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

    int PostStructData(std::string &IP, std::string &cameraID);

    std::string imagePath;

    std::vector<BoundingBox> objTrace;

    std::string objLable;

    int ID;

    BoundingBox currentBox;
};

std::ostream &operator<<(std::ostream &os, const ObjData &obj);

std::string findPerson(std::string &IP, std::string &imagePath, std::string &cameraID);

#endif // OBJDATA_H
