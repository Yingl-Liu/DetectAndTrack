#include "ObjData.h"

ObjData::ObjData(){}

ObjData::ObjData(std::shared_ptr<Predictor> predict)
    :imagePath(predict->imagePath), objLable(predict->objName),ID(predict->getID()),
     objTrace(predict->objTrace), db_id(predict->db_id){}

ObjData::ObjData(const ObjData & obj)
    :imagePath(obj.imagePath), objLable(obj.objLable),ID(obj.ID), objTrace(obj.objTrace), db_id(obj.db_id){}

ObjData::ObjData(ObjData &&obj)
    :ID(obj.ID), imagePath(std::move(obj.imagePath)), objLable(std::move(obj.objLable)),
     objTrace(std::move(obj.objTrace)), db_id(obj.db_id){}

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

int ObjData::PostStructData(std::string &postIP, std::string &cameraID){
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    std::string result;
    CURL *curl = curl_easy_init();
    /*Add update form*/

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
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    std::cout << "result:" << result << std::endl;

    return 0;
}


int ObjData::updateData(std::__cxx11::string IP){
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    std::string result;
    CURL *curl = curl_easy_init();
    std::string updateIP = IP + std::to_string(db_id);
    /*Add update form*/

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
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    std::cout << "result:" << result << std::endl;
    return 0;
}

void ObjData::vectorToStr(){
    int num = 0;
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

std::string findPerson(std::string &searchIP, std::string &imagePath, std::string &cameraID){
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

    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(searchIP.c_str()));
    }

    multi_request(formpost, curl, result);

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    std::cout << "result:" << result << std::endl;
    return result;
}
