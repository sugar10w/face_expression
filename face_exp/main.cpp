#include<opencv2/opencv.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "camera/camera_opencv.hpp"
#include "stasm_lib.h"

using namespace std;
using namespace cv;

// StasmFaceModel.hpp -----

struct FacePoints {
    bool valid;
    std::vector<cv::Point2f> point_list; 
};

class StasmFaceModel {
public:

    StasmFaceModel(std::string filename) 
    : filename_(filename)
    { }

    FacePoints getFacePoints(cv::Mat raw) {
        FacePoints fp;
        cv::Mat img;

        fp.valid = false;

        if (raw.channels() == 3)
            cv::cvtColor(raw, img, CV_BGR2GRAY);
        else if (raw.channels() == 1)
            img = raw.clone();
        else return fp;

        int foundface;
        float landmarks[2 * stasm_NLANDMARKS];
        if (!stasm_search_single(&foundface, landmarks, (const char*)img.data, img.cols, img.rows, "", filename_.c_str())) {
            std::cout << "[Error] stasm_search_single: " << stasm_lasterr() << std::endl;
            return fp;
        }

        if (!foundface) {
            std::cout << "[Error] stasm_search_single: no face detected." << std::endl;
            return fp;
        }

        fp.valid = true;
        stasm_force_points_into_image(landmarks, img.cols, img.rows);
        for (size_t i = 0; i < stasm_NLANDMARKS; ++i) {
            fp.point_list.push_back(cv::Point2f(landmarks[i*2], landmarks[i*2+1]));
        }
        return fp;

    }


private:
    std::string filename_;

};

// ----

int main() {
    Camera camera(1);
    StasmFaceModel face_model("../data");


    bool running_flag = true;

    while (running_flag) {

        Mat img = camera.getFrame();
        

        FacePoints fp = face_model.getFacePoints(img);
        
        if (fp.valid) {
            for (cv::Point2f& p : fp.point_list) 
                circle(img, p, 5, Scalar(255, 0, 0), 4, 4, 0);
            circle(img, Point2f(0,0), 5, Scalar(255, 0, 0));
        }

        imshow("img", img);
        char c = waitKey(1);
        
        switch (c) {
        case 'q':
            running_flag = false; break;
        case ' ':
            imwrite("test.png", img); break;
        default:
            break;
        }

    }

}