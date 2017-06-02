// Created by sugar10w, 2017.05


#ifndef STASM_FACE_MODEL_HPP
#define STASM_FACE_MODEL_HPP

#include<string>
#include<vector>
#include<opencv2/opencv.hpp>

#include "stasm_lib.h"


enum FaceExpression {
    Calm,
    Happy,      // mouth
    Kiss,       // mouth
    Nervous,    // brow
    Unknown
};

struct FacePoints {
    bool valid;
    std::vector<cv::Point2f> point_list; 

    FaceExpression getFaceExpression() {
        if (!valid) return Unknown;

        double mouth_width =  cv::norm(point_list[59] - point_list[65]);
        double mouth_height =  cv::norm(point_list[62] - point_list[74]);
        double nose_width =  cv::norm(point_list[58] - point_list[54]);
        double brow_distance = cv::norm(point_list[21] - point_list[22]);

        std::cout << mouth_width / nose_width << "\t" 
            << brow_distance / nose_width << std::endl;;


        if (mouth_width > nose_width * 1.3) return Happy;
        if (mouth_width < nose_width * 1.1 && mouth_height > mouth_width * 0.55) return Kiss;
        if (brow_distance < nose_width * 0.65) return Nervous;

        return Calm;
    }

    static cv::Scalar getFaceExpressionColor(FaceExpression f) {
        switch (f)
        {
        case Calm:
            return cv::Scalar(255,255,0);
            break;
        case Happy:
            return cv::Scalar(0, 128, 255);
            break;
        case Kiss:
            return cv::Scalar(192, 128, 255);
            break;
        case Nervous:
            return cv::Scalar(0,0,255);
            break;
        case Unknown: default: 
            return cv::Scalar(0,0,0);;
            break;
        }
    }

    static std::string getFaceExpressionString(FaceExpression f) {
        switch (f)
        {
        case Calm:
            return "Calm -_-";
            break;
        case Happy:
            return "Happy ^_^";
            break;
        case Kiss:
            return "Kiss ^3^";
            break;
        case Nervous:
            return "Nervous >_<";
            break;
        case Unknown: default: 
            return "Unknown ?_?";
            break;
        }
    }
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

#endif // STASM_FACE_MODEL_HPP