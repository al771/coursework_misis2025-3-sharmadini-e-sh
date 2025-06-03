#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <limits>

struct DiameterResult {
    double maxDiameter;
    cv::Point2f point1, point2;
    double area;
    double perimeter;
    int contourPoints;
    double circularity;
};

class MorphologyAnalyzer {
public:
    static cv::Mat binarizeImage(const cv::Mat& grayImage, int threshold = 128);
    static cv::Mat binarizeImageOtsu(const cv::Mat& grayImage);
    static std::vector<std::vector<cv::Point>> findContours(const cv::Mat& binaryImage);
    static DiameterResult calculateMaxDiameter(const std::vector<cv::Point>& contour);
    static double euclideanDistance(const cv::Point2f& p1, const cv::Point2f& p2);
    static int findLargestContour(const std::vector<std::vector<cv::Point>>& contours);
    static cv::Mat visualizeResults(const cv::Mat& image, 
                                   const std::vector<cv::Point>& contour,
                                   const DiameterResult& result);
};
