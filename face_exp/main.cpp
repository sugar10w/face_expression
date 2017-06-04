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


// ����ģ���࣬ͨ���ö���ʵ��5������֡���ͬһ����ʱ�Ÿ�����ʾ����
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

        Mat img = camera.getFrame();//���뵱ǰ֡
        
        flip(img, img, 1);//��Ϊ����ͼƬ�Ƿ���ģ������ٴ�ˮƽ��ת

        FacePoints fp = face_model.getFacePoints(img);//����FacePoints����ϸ��stasm/stasm_face_model.hpp

        if (fp.valid) {//��ʶ�������������
            if (point_flag)
                for (cv::Point2f& p : fp.point_list) 
                    circle(img, p, 1, Scalar(255, 0, 0), -1, 8, 0);//��ʾ������

            FaceExpression curr_f = fp.getFaceExpression();//��ȡ���飬���stasm/stasm_face_model.hpp

            if (streak_pass.add(curr_f)) f = curr_f;//��������
            std::string str = FacePoints::getFaceExpressionString(f);
            Scalar color = FacePoints::getFaceExpressionColor(f);//��ͬ�ı����Ӧ��ͬ����ɫ

            Point2f p = fp.point_list[14];//ͷ�������㣬�ڴ���ʾ������ı�

            p.y -= 30;
			p.x -= 50;

			
            putText(img, str, p, 0, 1, color, 2);//��ʾ�ı��ͱ߿�
			line(img, fp.bound_point[0], fp.bound_point[1], color, 2);
			line(img, fp.bound_point[0], fp.bound_point[2], color, 2);
			line(img, fp.bound_point[1], fp.bound_point[3], color, 2);
			line(img, fp.bound_point[2], fp.bound_point[3], color, 2);
			

        }

        imshow("img", img);

        video_writer << img;//д����Ƶ

        char c = waitKey(1);
        
        switch (c) {//��qʱ�˳������ո�ʱ��ʾ/����������
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