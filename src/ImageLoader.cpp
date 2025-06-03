#include "ImageLoader.h"

cv::Mat ImageLoader::loadImage(const std::string& filepath) {
    cv::Mat image = cv::imread(filepath, cv::IMREAD_COLOR);
    
    if (image.empty()) {
        std::cerr << "Error: Cannot load image " << filepath << std::endl;
        return cv::Mat();
    }
    
    std::cout << "Image loaded: " << image.cols << "x" << image.rows << " pixels" << std::endl;
    return image;
}

cv::Mat ImageLoader::convertToGrayscale(const cv::Mat& image) {
    if (image.empty()) {
        std::cerr << "Error: Empty image for conversion" << std::endl;
        return cv::Mat();
    }
    
    cv::Mat grayImage;
    
    if (image.channels() == 3) {
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    } else if (image.channels() == 1) {
        grayImage = image.clone();
    } else {
        std::cerr << "Error: Unsupported number of channels: " << image.channels() << std::endl;
        return cv::Mat();
    }
    
    return grayImage;
}

void ImageLoader::displayImage(const cv::Mat& image, const std::string& windowName) {
    if (image.empty()) {
        std::cerr << "Error: Empty image for display" << std::endl;
        return;
    }
    
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    cv::imshow(windowName, image);
    cv::waitKey(0);
}

bool ImageLoader::saveImage(const cv::Mat& image, const std::string& filepath) {
    if (image.empty()) {
        std::cerr << "Error: Empty image for saving" << std::endl;
        return false;
    }
    
    bool success = cv::imwrite(filepath, image);
    
    if (success) {
        std::cout << "Image saved: " << filepath << std::endl;
    } else {
        std::cerr << "Error saving image: " << filepath << std::endl;
    }
    
    return success;
}

bool ImageLoader::validateImage(const cv::Mat& image) {
    if (image.empty()) {
        std::cerr << "Validation error: Image is empty" << std::endl;
        return false;
    }
    
    if (image.cols < 10 || image.rows < 10) {
        std::cerr << "Validation error: Image too small" << std::endl;
        return false;
    }
    
    if (image.type() != CV_8UC1 && image.type() != CV_8UC3) {
        std::cerr << "Validation error: Unsupported image type" << std::endl;
        return false;
    }
    
    return true;
}

cv::Mat ImageLoader::resizeImage(const cv::Mat& image, int maxSize) {
    if (image.empty()) {
        return cv::Mat();
    }
    
    int currentMax = std::max(image.cols, image.rows);
    
    if (currentMax <= maxSize) {
        return image.clone();
    }
    
    double scale = static_cast<double>(maxSize) / currentMax;
    int newWidth = static_cast<int>(image.cols * scale);
    int newHeight = static_cast<int>(image.rows * scale);
    
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(newWidth, newHeight));
    
    std::cout << "Image resized: " << image.cols << "x" << image.rows 
              << " -> " << newWidth << "x" << newHeight << std::endl;
    
    return resized;
}
