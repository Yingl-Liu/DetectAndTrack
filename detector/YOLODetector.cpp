#include "YOLODetector.h"


YOLODetector::YOLODetector(const std::string & dataFile,
                           const std::string & cfgFile,
                           const std::string & weightFile,
                           const int w, const int h, const int c,
                           float thresh,
                           float hier_thresh):width(w), height(h), channel(c), thresh(thresh), hier_thresh(hier_thresh)
{
    list *options = read_data_cfg(const_cast<char*>(dataFile.c_str()));
    totalclass = option_find_int(options, "classes", 80);
    char *nameList = option_find_str(options, "names", "data/coco.names");
    namelables = get_labels(nameList);
    net = load_network(const_cast<char*>(cfgFile.c_str()), const_cast<char*>(weightFile.c_str()), 0);
    set_batch_network(net, 1);
    std::cout << "success load weight" << std::endl;
    im = make_image(width, height, channel);
    sized = letterbox_image(im, net->w, net->h);
    srand(2222222);
    nms = 0.45;
    carLable.insert({{2, "car"}, {5, "bus"}, {7, "trunk"}});
    detectROI = cv::Rect(cv::Point2i(int(0.05*w), int(0.13*h)), cv::Point2i((w - int(0.05*w)), (h - int(0.13*h))));
}

YOLODetector::~YOLODetector(){
    free_image(im);
    free_image(sized);
}

std::vector<Detection> YOLODetector::detect(const cv::Mat &image){
    std::vector<Detection> detections;
    double w, h, x, y;
    frame =new IplImage(image);
    ipl_into_image(frame, im);
    rgbgr_image(im);
    letterbox_image_into(im, net->w, net->h, sized);
    l = net->layers[net->n-1];
    float * X = sized.data;
    network_predict(net, X);
    detection *dets = 0;
    dets = get_network_boxes(net, width, height, thresh, hier_thresh, 0, 1, &nboxes);
    if (nms) do_nms_sort(dets, nboxes, totalclass, nms);
    for(int i = 0; i < nboxes; ++i){
        for(int j = 0; j < totalclass; ++j){
            if (0 == j && dets[i].prob[j] > thresh){
                box b = dets[i].bbox;
                x = b.x * width;
                y = b.y * height;
                w = b.w * width;// + 10;
                h = b.h * height;// + 20;
                BoundingBox bb = BoundingBox(x, y, w, h, image.cols, image.rows);
                std::shared_ptr<cv::Mat> pimage = std::make_shared<cv::Mat>(image(bb.rect()));
                //cv::Point2f center = cv::Point2f(x, y);
                if((detectROI.contains(cv::Point2f(x, y)))&&((h / w)>1.4))
                    detections.push_back(Detection(std::string("person"),
                                                   dets[i].prob[j],
                                                   bb,
                                                   0, pimage));
            }
            else if((2 == j || 5 == j || 7 == j) && dets[i].prob[j] > thresh){
                    box b = dets[i].bbox;
                    x = b.x * width;
                    y = b.y * height;
                    w = b.w * width;
                    h = b.h * height;
                    BoundingBox bb = BoundingBox(x, y, w, h, image.cols, image.rows);
                    std::shared_ptr<cv::Mat> pimage = std::make_shared<cv::Mat>(image(bb.rect()));
                    if(w>0.3*width||h>0.2*height)
                        detections.push_back(Detection(carLable[j], dets[i].prob[j],
                                                   bb,
                                                   2, pimage));
            }
        }
    }
    free_detections(dets, nboxes);
    return detections;
}

void YOLODetector::mat_into_im(const cv::Mat &image){
    unsigned char *data = (unsigned char *)image.data;
    int h = image.rows;
    int w = image.cols;
    int c = image.channels();
    int step = image.step1();
    int i, j, k;

    for(i = 0; i < h; ++i){
        for(k= 0; k < c; ++k){
            for(j = 0; j < w; ++j){
                im.data[k*w*h + i*w + j] = data[i*step + j*c + k]/255.;
            }
        }
    }
}

