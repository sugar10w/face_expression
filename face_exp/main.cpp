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


// 队列模版类，通过该队列实现5张连续帧输出同一表情时才更换显示表情
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
    bool point_flag = false;

    SimpleStreakPass<int> streak_pass(5, Unknown);
    FaceExpression f = Unknown;

    VideoWriter video_writer("VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30.0, Size(640, 480)); 

    while (running_flag) {

        Mat img = camera.getFrame();//读入当前帧
        
        flip(img, img, 1);//因为读入图片是反向的，所以再次水平翻转

        FacePoints fp = face_model.getFacePoints(img);//读入FacePoints，详细见stasm/stasm_face_model.hpp

        if (fp.valid) {//在识别到人脸的情况下
            if (point_flag)
                for (cv::Point2f& p : fp.point_list) 
                    circle(img, p, 1, Scalar(255, 0, 0), -1, 8, 0);//显示特征点

            FaceExpression curr_f = fp.getFaceExpression();//读取表情，详见stasm/stasm_face_model.hpp

            if (streak_pass.add(curr_f)) f = curr_f;//防抖处理
            std::string str = FacePoints::getFaceExpressionString(f);
            Scalar color = FacePoints::getFaceExpressionColor(f);//不同的表情对应不同的颜色

            Point2f p = fp.point_list[14];//头顶特征点，在此显示表情的文本

            p.y -= 30;
			p.x -= 50;

			
            putText(img, str, p, 0, 1, color, 2);//显示文本和边框
			line(img, fp.bound_point[0], fp.bound_point[1], color, 2);
			line(img, fp.bound_point[0], fp.bound_point[2], color, 2);
			line(img, fp.bound_point[1], fp.bound_point[3], color, 2);
			line(img, fp.bound_point[2], fp.bound_point[3], color, 2);
			

        }

        imshow("img", img);

        video_writer << img;//写入视频

        char c = waitKey(1);
        
        switch (c) {//按q时退出，按空格时显示/隐藏特征点
        case 'q':
		case 'Q':
            running_flag = false; break;
        case ' ':
            point_flag = !point_flag; break;
        default:
            break;
        }

    }


}