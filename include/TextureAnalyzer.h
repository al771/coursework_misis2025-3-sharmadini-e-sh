#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>

class TextureAnalyzer {
private:
    std::vector<std::vector<int>> glcm_;
    int levels_;
    int totalPairs_;
    
public:
    explicit TextureAnalyzer(int levels = 256);
    void buildGLCM(const cv::Mat& image, int dx, int dy);
    double calculateIDM();
    std::vector<std::vector<double>> getNormalizedGLCM() const;
    void printGLCMStats() const;
    double analyzeMultiDirectional(const cv::Mat& image);
    void clear();
};
