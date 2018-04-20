#include "ObjData.h"

ObjData::ObjData(){}

ObjData::ObjData(std::shared_ptr<Predictor> predict)
    :imagePath(predict->imagePath), objLable(predict->objName),ID(predict->getID()), objTrace(predict->objTrace){}

ObjData::ObjData(const ObjData & obj)
    :imagePath(obj.imagePath), objLable(obj.objLable),ID(obj.ID), objTrace(obj.objTrace){}

ObjData::ObjData(ObjData &&obj)
    :ID(obj.ID), imagePath(std::move(obj.imagePath)), objLable(std::move(obj.objLable)), objTrace(std::move(obj.objTrace)){}

std::ostream & operator <<(std::ostream &os, const ObjData &obj){
    os << "ID:" << obj.ID << std::endl;
    os << "Lable:" << obj.objLable << std::endl;
    os << "imagePath:" << obj.imagePath << std::endl;
    os << "duration:" << obj.objTrace.size() << std::endl;
    return os;
}

int ObjData::PostStructData(std::string &IP, std::string &cameraID){
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
    char result[1024] = { 0 };
    static const char buf[] = "Expect:";

    /*Add post form*/
    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "image",
                 CURLFORM_FILE,         const_cast<char*>(imagePath.c_str()),
                 CURLFORM_CONTENTTYPE,  "image/*",
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "person_id",
                 CURLFORM_COPYCONTENTS, const_cast<char*>(std::to_string(ID).c_str()),
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "camera",
                 CURLFORM_COPYCONTENTS,  const_cast<char*>(cameraID.c_str()),
                 CURLFORM_END);

    CURL *curl = curl_easy_init();
    CURLM *multi_handle = curl_multi_init();
    int still_running;
    headerlist = curl_slist_append(headerlist, buf);
    if(curl && multi_handle){
        /*setting transport option*/
        curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(IP.c_str())) ;
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_READDATA, static_cast<void*>(result));

        curl_multi_add_handle(multi_handle, curl);
        curl_multi_perform(multi_handle, &still_running);

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
            case -1: break;
            case 0:
            default:
                std::cout<<"perform!"<<std::endl;
                curl_multi_perform(multi_handle, &still_running);
                break;
            }
        }while(still_running);

        curl_multi_cleanup(multi_handle);
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all(headerlist);
    }
    std::cout<<result<<std::endl;
    return 0;
}

static int writer(char *data, size_t size, size_t nmemb, std::string *writerData){
    if(writerData == NULL) return 0;
    int len = size * nmemb;
    writerData->append(data, len);
    return len;
}

std::string findPerson(std::string &IP, std::string &imagePath, std::string &cameraID){
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
//    char result[1024] = { 0 };
    std::string result;
    static const char buf[] = "Expect:";

    /*Add post form*/
    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "image",
                 CURLFORM_FILE,         const_cast<char*>(imagePath.c_str()),
                 CURLFORM_CONTENTTYPE,  "image/*",
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME,     "camera",
                 CURLFORM_COPYCONTENTS,  const_cast<char*>(cameraID.c_str()),
                 CURLFORM_END);

    CURL *curl = curl_easy_init();
    CURLM *multi_handle = curl_multi_init();
    int still_running;
    headerlist = curl_slist_append(headerlist, buf);
    if(curl && multi_handle){
        /*setting transport option*/
        curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(IP.c_str())) ;
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        curl_multi_add_handle(multi_handle, curl);
        curl_multi_perform(multi_handle, &still_running);

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
            case -1: break;
            case 0:
            default:
                std::cout<<"perform!"<<std::endl;
                curl_multi_perform(multi_handle, &still_running);
                break;
            }
        }while(still_running);

        curl_multi_cleanup(multi_handle);
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all(headerlist);
    }
//    std::string personID = std::string(result);
    std::cout<< "result:" << result <<std::endl;
    return result;
}
