#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

class ImageLoader {
public:
    static cv::Mat loadImage(const std::string& filepath);
    static cv::Mat convertToGrayscale(const cv::Mat& image);
    static void displayImage(const cv::Mat& image, const std::string& windowName = "Image");
    static bool saveImage(const cv::Mat& image, const std::string& filepath);
    static bool validateImage(const cv::Mat& image);
    static cv::Mat resizeImage(const cv::Mat& image, int maxSize = 512);
};
