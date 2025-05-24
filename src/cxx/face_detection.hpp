#ifndef FACE_DETECTION_HPP
#define FACE_DETECTION_HPP

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "module_test.h"

class FaceConetxt
{
private:
    cv::CascadeClassifier face_cascade;
    std::vector<std::pair<std::string, cv::Mat>> faces;


    // Test function
    DEF_TEST_FUNCTION(detect_image_test_and_show, void, (const cv::CascadeClassifier& face_cascade, const cv::Mat& image), {
        
        cv::Mat image_copy = image.clone();
        
        cv::cvtColor(image_copy, gray_image, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray_image, gray_image);
        
        face_cascade.detectMultiScale(gray_image, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
        
        for (size_t i = 0; i < faces.size(); i++)
        {
            cv::rectangle(image_copy, faces[i], cv::Scalar(255, 0, 0), 2);
        }
        cv::imshow("Detected Faces", image_copy);

        cv::waitKey(0);
    })


public:
    FaceConetxt(const std::string& cascade_path = "haarcascade_frontalface_default.xml", 
                const std::string& image_path = "test.jpg");

    ~FaceConetxt();

    void init();
    void detect_face(const cv::Mat& image, std::vector<cv::Rect>& faces);
    void draw_face(cv::Mat& image, const std::vector<cv::Rect>& faces);
};



#endif