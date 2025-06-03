#include "MorphologyAnalyzer.h"

cv::Mat MorphologyAnalyzer::binarizeImage(const cv::Mat& grayImage, int threshold) {
    if (grayImage.empty() || grayImage.type() != CV_8UC1) {
        std::cerr << "Error: Image must be grayscale" << std::endl;
        return cv::Mat();
    }
    
    cv::Mat binaryImage;
    cv::threshold(grayImage, binaryImage, threshold, 255, cv::THRESH_BINARY);
    
    std::cout << "Binarization completed with threshold: " << threshold << std::endl;
    return binaryImage;
}

cv::Mat MorphologyAnalyzer::binarizeImageOtsu(const cv::Mat& grayImage) {
    if (grayImage.empty() || grayImage.type() != CV_8UC1) {
        std::cerr << "Error: Image must be grayscale" << std::endl;
        return cv::Mat();
    }
    
    cv::Mat binaryImage;
    double otsuThreshold = cv::threshold(grayImage, binaryImage, 0, 255, 
                                        cv::THRESH_BINARY + cv::THRESH_OTSU);
    
    std::cout << "Otsu binarization: threshold = " 
              << std::fixed << std::setprecision(1) << otsuThreshold << std::endl;
    
    return binaryImage;
}

std::vector<std::vector<cv::Point>> MorphologyAnalyzer::findContours(const cv::Mat& binaryImage) {
    if (binaryImage.empty() || binaryImage.type() != CV_8UC1) {
        std::cerr << "Error: Image must be binary" << std::endl;
        return std::vector<std::vector<cv::Point>>();
    }
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    
    std::cout << "Found contours: " << contours.size() << std::endl;
    
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        std::cout << "Contour " << i << ": points = " << contours[i].size() 
                  << ", area = " << std::fixed << std::setprecision(1) << area << std::endl;
    }
    
    return contours;
}

DiameterResult MorphologyAnalyzer::calculateMaxDiameter(const std::vector<cv::Point>& contour) {
    DiameterResult result;
    result.maxDiameter = 0.0;
    result.area = 0.0;
    result.perimeter = 0.0;
    result.contourPoints = contour.size();
    result.circularity = 0.0;
    
    if (contour.size() < 2) {
        std::cerr << "Error: Contour has less than 2 points" << std::endl;
        return result;
    }
    
    std::cout << "Calculating maximum diameter for contour with " 
              << contour.size() << " points..." << std::endl;
    
    for (size_t i = 0; i < contour.size(); i++) {
        for (size_t j = i + 1; j < contour.size(); j++) {
            double distance = euclideanDistance(
                cv::Point2f(contour[i]), 
                cv::Point2f(contour[j])
            );
            
            if (distance > result.maxDiameter) {
                result.maxDiameter = distance;
                result.point1 = cv::Point2f(contour[i]);
                result.point2 = cv::Point2f(contour[j]);
            }
        }
    }
    
    result.area = cv::contourArea(contour);
    result.perimeter = cv::arcLength(contour, true);
    
    if (result.perimeter > 0) {
        result.circularity = (4.0 * M_PI * result.area) / (result.perimeter * result.perimeter);
    }
    
    std::cout << "Maximum diameter: " << std::fixed << std::setprecision(2) 
              << result.maxDiameter << " pixels" << std::endl;
    std::cout << "Diameter points: (" << result.point1.x << "," << result.point1.y 
              << ") - (" << result.point2.x << "," << result.point2.y << ")" << std::endl;
    std::cout << "Area: " << std::fixed << std::setprecision(1) << result.area << std::endl;
    std::cout << "Perimeter: " << std::fixed << std::setprecision(1) << result.perimeter << std::endl;
    std::cout << "Circularity: " << std::fixed << std::setprecision(3) << result.circularity << std::endl;
    
    return result;
}

double MorphologyAnalyzer::euclideanDistance(const cv::Point2f& p1, const cv::Point2f& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}

int MorphologyAnalyzer::findLargestContour(const std::vector<std::vector<cv::Point>>& contours) {
    if (contours.empty()) {
        return -1;
    }
    
    int largestIndex = 0;
    double largestArea = cv::contourArea(contours[0]);
    
    for (size_t i = 1; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > largestArea) {
            largestArea = area;
            largestIndex = static_cast<int>(i);
        }
    }
    
    std::cout << "Largest contour: index " << largestIndex 
              << ", area " << std::fixed << std::setprecision(1) << largestArea << std::endl;
    
    return largestIndex;
}

cv::Mat MorphologyAnalyzer::visualizeResults(const cv::Mat& image, 
                                           const std::vector<cv::Point>& contour,
                                           const DiameterResult& result) {
    cv::Mat visualization;
    
    if (image.channels() == 1) {
        cv::cvtColor(image, visualization, cv::COLOR_GRAY2BGR);
    } else {
        visualization = image.clone();
    }
    
    std::vector<std::vector<cv::Point>> contours = {contour};
    cv::drawContours(visualization, contours, -1, cv::Scalar(0, 255, 0), 2);
    
    cv::line(visualization, result.point1, result.point2, cv::Scalar(0, 0, 255), 3);
    
    cv::circle(visualization, result.point1, 5, cv::Scalar(255, 0, 0), -1);
    cv::circle(visualization, result.point2, 5, cv::Scalar(255, 0, 0), -1);
    
    std::string diameterText = "Diameter: " + std::to_string(static_cast<int>(result.maxDiameter)) + " px";
    std::string areaText = "Area: " + std::to_string(static_cast<int>(result.area)) + " px²";
    
    cv::putText(visualization, diameterText, cv::Point(10, 30), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    cv::putText(visualization, areaText, cv::Point(10, 60), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    
    return visualization;
}

