#include "face_detection.hpp"



FaceConetxt::FaceConetxt(const std::string& cascade_path, const std::string& image_path)
{
    if (!face_cascade.load(cascade_path))
    {
        std::cerr << "Error loading cascade file: " << cascade_path << std::endl;
        exit(1);
    }
    
    cv::Mat image = cv::imread(image_path);
    if (image.empty())
    {
        std::cerr << "Error loading image file: " << image_path << std::endl;
        exit(1);
    }
    

}
\


    FaceConetxt(const std::string& cascade_path = "haarcascade_frontalface_default.xml", 
                const std::string& image_path = "test.jpg");

    ~FaceConetxt();

    void init();
    void detect_face(const cv::Mat& image, std::vector<cv::Rect>& faces);
    void draw_face(cv::Mat& image, const std::vector<cv::Rect>& faces);