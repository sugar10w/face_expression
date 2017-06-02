#include<opencv2/opencv.hpp>

#include <queue>
#include <vector>
#include <string>
#include <iostream>

#include "camera/camera_opencv.hpp"
#include "stasm_lib.h"
#include "stasm_face_model.hpp"

using namespace std;
using namespace cv;


// simple streak pass
template<typename T> class SimpleStreakPass {

public:
    SimpleStreakPass(int n, T t) : n_(n), p_(0) {
        list_.resize(n, t);
    }

    bool add(T t) {
        list_[p_] = t;
        ++ p_;
        p_ %= n_;

        for (T& tt : list_) if (tt != list_[0]) return false;
        return true;
    }

private:
    int n_;
    int p_;
    vector<T> list_;
};


// main
int main() {
    Camera camera(0);
    StasmFaceModel face_model("../data");


    bool running_flag = true;
    bool point_flag = true;

    SimpleStreakPass<int> streak_pass(5, Unknown);
    FaceExpression f = Unknown;

    VideoWriter video_writer("VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30.0, Size(640, 480)); 

    while (running_flag) {

        Mat img = camera.getFrame();
        
        flip(img, img, 1);

        FacePoints fp = face_model.getFacePoints(img);

        if (fp.valid) {
            if (point_flag)
                for (cv::Point2f& p : fp.point_list) 
                    circle(img, p, 1, Scalar(255, 0, 0), -1, 8, 0);

            FaceExpression curr_f = fp.getFaceExpression();

            if (streak_pass.add(curr_f)) f = curr_f;
            std::string str = FacePoints::getFaceExpressionString(f);
            Scalar color = FacePoints::getFaceExpressionColor(f);

            Point2f p = fp.point_list[14];
            p.x -= 50;

            putText(img, str, p, 0, 1, color, 2);

        }

        imshow("img", img);

        video_writer << img;

        char c = waitKey(1);
        
        switch (c) {
        case 'q':
            running_flag = false; break;
        case ' ':
            point_flag = !point_flag; break;
        default:
            break;
        }

    }


}