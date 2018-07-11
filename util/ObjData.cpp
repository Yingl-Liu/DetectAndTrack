#include "ObjData.h"

ObjData::ObjData(){}

ObjData::ObjData(std::shared_ptr<Predictor> predict)
    :imagePath(predict->imagePath), objLable(predict->getLabel()),ID(predict->getID()),
     objTrace(predict->objTrace), db_id(predict->db_id), objName(predict->objName){}

ObjData::ObjData(const ObjData & obj)
    :imagePath(obj.imagePath), objLable(obj.objLable), ID(obj.ID),
     objTrace(obj.objTrace), db_id(obj.db_id), objName(obj.objName){}

ObjData::ObjData(ObjData &&obj)
    :ID(obj.ID), imagePath(std::move(obj.imagePath)), objLable(std::move(obj.objLable)),
     objTrace(std::move(obj.objTrace)), db_id(obj.db_id), objName(std::move(obj.objName)){}

std::ostream & operator <<(std::ostream &os, const ObjData &obj){
    os << "ID:" << obj.ID << std::endl;
    os << "Lable:" << obj.objLable << std::endl;
    os << "imagePath:" << obj.imagePath << std::endl;
    os << "duration:" << obj.objTrace.size() << std::endl;
    return os;
}

static int writer(char *data, size_t size, size_t nmemb, std::string *writerData){
    if(writerData == NULL) return 0;
    int len = size * nmemb;
    writerData->append(data, len);
    return len;
}

std::vector<std::string> split(std::string input, const char* delim){
    int pos = 0;
    int npos = 0;
    std::vector<std::string> result;
    int delimlen = strlen(delim);
    while((npos=input.find(delim, pos))!=-1){
        std::string temp = input.substr(pos,npos-pos);
        result.push_back(temp);
        pos = npos+delimlen;
    }
    result.push_back(input.substr(pos,input.length()-pos));
    return result;
}

int ObjData::PostStructData(std::string &postIP, std::string &cameraID){
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    std::string result;
    CURL *curl = curl_easy_init();
    /*Add update form*/
    if(std::string("person") == objLable){
        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME,     "image",
                     CURLFORM_FILE,         const_cast<char*>(imagePath.c_str()),
                     CURLFORM_CONTENTTYPE,  "image/*",
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME,     "camera",
                     CURLFORM_COPYCONTENTS,  const_cast<char*>(cameraID.c_str()),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "person_id",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(std::to_string(ID).c_str()),
                     CURLFORM_END);

        if(curl){
            curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(postIP.c_str()));
        }

        multi_request(formpost, curl, result);
    }
    else{
        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME,     "image",
                     CURLFORM_FILE,         const_cast<char*>(imagePath.c_str()),
                     CURLFORM_CONTENTTYPE,  "image/*",
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME,     "camera",
                     CURLFORM_COPYCONTENTS,  const_cast<char*>(cameraID.c_str()),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "person_id",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(std::to_string(ID).c_str()),
                     CURLFORM_END);

        if(curl){
            curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(postIP.c_str()));
        }

        multi_request(formpost, curl, result);
    }
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    std::cout << "result:" << result << std::endl;

    return 0;
}

int ObjData::updateData(const std::string &IP, std::string &cameraID){
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    std::string result;
    CURL *curl = curl_easy_init();
    vectorToStr();
    if(std::string("person") == objLable){
        std::string updateIP = IP + "person/" + std::to_string(db_id);
        /*Add update form, update person struct data*/        
        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "traceX",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(traceX.c_str()),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "traceY",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(traceY.c_str()),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "age",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(std::to_string(age).c_str()),
                     CURLFORM_END);

        if(curl){
            curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(updateIP.c_str()));
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        }
        multi_request(formpost, curl, result);
    }
    else{
        /*post car struct data*/
        std::string updateIP = IP + "car/";
        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME,     "image",
                     CURLFORM_FILE,         const_cast<char*>(imagePath.c_str()),
                     CURLFORM_CONTENTTYPE,  "image/*",
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME,     "camera",
                     CURLFORM_COPYCONTENTS,  const_cast<char*>(cameraID.c_str()),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "classification",
                     CURLFORM_COPYCONTENTS, objLable.c_str(),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "traceX",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(traceX.c_str()),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "traceY",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(traceY.c_str()),
                     CURLFORM_END);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "age",
                     CURLFORM_COPYCONTENTS, const_cast<char*>(std::to_string(age).c_str()),
                     CURLFORM_END);
        if(curl){
            curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(updateIP.c_str()));
        }

        multi_request(formpost, curl, result);
    }
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    std::cout << "result:" << result << std::endl;
    return 0;
}

void ObjData::vectorToStr(){
    int num = 0;
//    std::cout << "the size of object Trace is : " << objTrace.size() << std::endl;
    age = objTrace.size();
    for(auto bb : objTrace){
        traceX += (std::to_string(int(bb.cx + 0.5)) + ",");
        traceY += (std::to_string(int(bb.cy + 0.5)) + ",");
        if((++num)>1024)break;
    }
}

CURLMcode multi_request(curl_httppost *formpost, CURL *curl, std::string &result){
    int still_running;
    CURLM *multi_handle = curl_multi_init();
    curl_slist *headerlist = NULL;
    static const char buf[] = "Expect:";
    headerlist = curl_slist_append(headerlist, buf);

    if(curl && multi_handle){
        /*setting transport option*/

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        if(CURLM_OK != curl_multi_add_handle(multi_handle, curl)){
            std::cout << "curl_multi_add_handle failed!" << std::endl;
            return CURLM_ADDED_ALREADY;
        }
        if(CURLM_OK != curl_multi_perform(multi_handle, &still_running)){
            std::cout << "curl_multi_perform dailed!" << std::endl;
            return CURLM_CALL_MULTI_PERFORM;
        }

        do{
            struct timeval timeout;
            int rc;//the select() return code
            fd_set fdread;
            fd_set fdwrite;
            fd_set fdexcep;
            int maxfd = -1;

            FD_ZERO(&fdread);
            FD_ZERO(&fdwrite);
            FD_ZERO(&fdexcep);
            long curl_timeout = -1;
            /*set a suitable timeout*/
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            curl_multi_timeout(multi_handle, &curl_timeout);
            if(curl_timeout>=0){
                timeout.tv_sec = curl_timeout/1000;
                if(timeout.tv_sec>1) timeout.tv_sec = 1;
                else timeout.tv_usec = (curl_timeout % 1000) * 1000;
            }
            /*get file descriptors from the transfers*/
            curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
            /*listen these port*/
            rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
            switch(rc){
            case -1:
                break;
            case 0:
            default:
                std::cout<<"perform!"<<std::endl;
                if(CURLM_OK != curl_multi_perform(multi_handle, &still_running)){
                    std::cout << "curl_multi_perform dailed!" << std::endl;
                    return CURLM_CALL_MULTI_PERFORM;
                }
                break;
            }
        }while(still_running);

        curl_multi_cleanup(multi_handle);
        curl_slist_free_all(headerlist);

        return CURLM_OK;
    }
    else
        return CURLM_BAD_HANDLE;
}

std::string _findPerson(std::string &searchIP, std::string &imagePath, std::string &cameraID, int threshold, int seconds){
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    std::string result;
    CURL *curl = curl_easy_init();
    /*Add find form*/
    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "image",
                 CURLFORM_FILE,         const_cast<char*>(imagePath.c_str()),
                 CURLFORM_CONTENTTYPE,  "image/*",
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "camera",
                 CURLFORM_COPYCONTENTS,  const_cast<char*>(cameraID.c_str()),
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "thre",
                 CURLFORM_COPYCONTENTS,  const_cast<char*>(std::to_string(threshold).c_str()),
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "seconds",
                 CURLFORM_COPYCONTENTS,  const_cast<char*>(std::to_string(seconds).c_str()),
                 CURLFORM_END);

    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(searchIP.c_str()));
    }

    multi_request(formpost, curl, result);
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    return result;
}

std::pair<int, std::string> findPerson(const std::string &IP, std::string &imagePath, std::string &cameraID, int threshold, int seconds){
    std::string findIP = IP + "findperson/";
    std::string result = _findPerson(findIP, imagePath, cameraID, threshold, seconds);
    std::cout << result << std::endl;//result format "label, id_in_db"
//    rapidjson::Document document;
//    document.Parse(result.c_str());
//    assert(document["label"].IsString());
//    assert(document["id_in_db"].IsInt());
    auto res = split(result, "\"");
    for(auto i : res)
        std::cout<<i<<std::endl;
    std::cout<<res.size()<<std::endl;
    if(res.size()==3){
        auto res1 = split(res[1],",");
        if(res1.size()==2){
            std::cout << "ID:" << atoi(res1[1].c_str()) << std::endl;
            std::cout << "persionID:" << res1[0] << std::endl;
            return std::make_pair(atoi(res1[1].c_str()), res1[0]);
        }
        else{
            return std::make_pair(0, "person");
        }
    }
    else
        return std::make_pair(0, "person");
}
