#include "ImageLoader.h"
#include "TextureAnalyzer.h"
#include "MorphologyAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>


struct AnalysisResults {
    double idm_value;
    DiameterResult diameter_result;
    std::string image_path;
    std::string texture_interpretation;
    std::string size_interpretation;
};

std::string interpretIDM(double idm) {
    if (idm >= 0.8) {
        return "Very homogeneous texture";
    } else if (idm >= 0.6) {
        return "Moderately homogeneous texture";
    } else if (idm >= 0.4) {
        return "Heterogeneous texture with variations";
    } else if (idm >= 0.2) {
        return "Highly heterogeneous texture";
    } else {
        return "Very complex, chaotic texture";
    }
}

std::string interpretSize(double diameter, double area) {
    std::string result = "Object ";
    
    if (diameter < 50) {
        result += "small size";
    } else if (diameter < 150) {
        result += "medium size";
    } else {
        result += "large size";
    }
    
    result += " (diameter: " + std::to_string(static_cast<int>(diameter)) + " pixels)";
    return result;
}

void printResults(const AnalysisResults& results) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           IMAGE ANALYSIS RESULTS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "File: " << results.image_path << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    std::cout << "TEXTURE ANALYSIS (IDM):" << std::endl;
    std::cout << "   IDM value: " << std::fixed << std::setprecision(6) 
              << results.idm_value << std::endl;
    std::cout << "   Interpretation: " << results.texture_interpretation << std::endl;
    std::cout << std::endl;
    
    std::cout << "MORPHOLOGICAL ANALYSIS:" << std::endl;
    std::cout << "   Maximum diameter: " << std::fixed << std::setprecision(2) 
              << results.diameter_result.maxDiameter << " pixels" << std::endl;
    std::cout << "   Object area: " << std::fixed << std::setprecision(1) 
              << results.diameter_result.area << " pixels²" << std::endl;
    std::cout << "   Perimeter: " << std::fixed << std::setprecision(1) 
              << results.diameter_result.perimeter << " pixels" << std::endl;
    std::cout << "   Circularity: " << std::fixed << std::setprecision(3) 
              << results.diameter_result.circularity << std::endl;
    std::cout << "   Interpretation: " << results.size_interpretation << std::endl;
    std::cout << std::endl;
    
    std::cout << "DIAMETER POINTS:" << std::endl;
    std::cout << "   Point 1: (" << std::fixed << std::setprecision(1) 
              << results.diameter_result.point1.x << ", " 
              << results.diameter_result.point1.y << ")" << std::endl;
    std::cout << "   Point 2: (" << std::fixed << std::setprecision(1) 
              << results.diameter_result.point2.x << ", " 
              << results.diameter_result.point2.y << ")" << std::endl;
    
    std::cout << std::string(60, '=') << std::endl;
}

void saveResultsToFile(const AnalysisResults& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error creating results file: " << filename << std::endl;
        return;
    }
    
    file << "IMAGE_PATH=" << results.image_path << "\n";
    file << "IDM_VALUE=" << std::fixed << std::setprecision(6) << results.idm_value << "\n";
    file << "IDM_INTERPRETATION=" << results.texture_interpretation << "\n";
    file << "MAX_DIAMETER=" << std::fixed << std::setprecision(2) << results.diameter_result.maxDiameter << "\n";
    file << "OBJECT_AREA=" << std::fixed << std::setprecision(1) << results.diameter_result.area << "\n";
    file << "PERIMETER=" << std::fixed << std::setprecision(1) << results.diameter_result.perimeter << "\n";
    file << "CIRCULARITY=" << std::fixed << std::setprecision(3) << results.diameter_result.circularity << "\n";
    file << "SIZE_INTERPRETATION=" << results.size_interpretation << "\n";
    file << "DIAMETER_POINT1_X=" << results.diameter_result.point1.x << "\n";
    file << "DIAMETER_POINT1_Y=" << results.diameter_result.point1.y << "\n";
    file << "DIAMETER_POINT2_X=" << results.diameter_result.point2.x << "\n";
    file << "DIAMETER_POINT2_Y=" << results.diameter_result.point2.y << "\n";
    
    file.close();
    std::cout << "Results saved to: " << filename << std::endl;
}
    


AnalysisResults analyzeImage(const std::string& imagePath) {
    AnalysisResults results;
    results.image_path = imagePath;
    results.idm_value = 0.0;
    
    std::cout << "\nStarting image analysis: " << imagePath << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    cv::Mat originalImage = ImageLoader::loadImage(imagePath);
    if (originalImage.empty()) {
        std::cerr << "Failed to load image" << std::endl;
        return results;
    }
    
    cv::Mat grayImage = ImageLoader::convertToGrayscale(originalImage);
    if (grayImage.empty()) {
        std::cerr << "Error converting to grayscale" << std::endl;
        return results;
    }
    
    cv::Mat resizedGray = ImageLoader::resizeImage(grayImage, 512);
    
    std::cout << "\nTexture analysis..." << std::endl;
    TextureAnalyzer textureAnalyzer;
    results.idm_value = textureAnalyzer.analyzeMultiDirectional(resizedGray);
    results.texture_interpretation = interpretIDM(results.idm_value);
    
    std::cout << "\nMorphological analysis..." << std::endl;
    
    cv::Mat binaryImage = MorphologyAnalyzer::binarizeImageOtsu(resizedGray);
    
    std::vector<std::vector<cv::Point>> contours = MorphologyAnalyzer::findContours(binaryImage);
    
    if (!contours.empty()) {
        int largestIndex = MorphologyAnalyzer::findLargestContour(contours);
        if (largestIndex >= 0) {
            results.diameter_result = MorphologyAnalyzer::calculateMaxDiameter(contours[largestIndex]);
            results.size_interpretation = interpretSize(results.diameter_result.maxDiameter, 
                                                       results.diameter_result.area);
            
            cv::Mat visualization = MorphologyAnalyzer::visualizeResults(
                resizedGray, contours[largestIndex], results.diameter_result);
            
            
            
        }
    } else {
        std::cout << "No objects found in image" << std::endl;
        results.size_interpretation = "No objects detected";
    }
    
    return results;
}
void createTestImages() {
    // 1. Однородный серый квадрат
    cv::Mat uniform = cv::Mat::ones(200, 200, CV_8UC1) * 128;
    cv::imwrite("../test_images/uniform_gray.png", uniform);
    
    // 2. Белый круг на черном фоне
    cv::Mat circle = cv::Mat::zeros(200, 200, CV_8UC1);
    cv::circle(circle, cv::Point(100, 100), 50, 255, -1);
    cv::imwrite("../test_images/white_circle.png", circle);
    
    // 3. Шахматная доска
    cv::Mat chess = cv::Mat::zeros(200, 200, CV_8UC1);
    for (int i = 0; i < 200; i += 25) {
        for (int j = 0; j < 200; j += 25) {
            if (((i/25) + (j/25)) % 2 == 0) {
                cv::rectangle(chess, cv::Point(j, i), cv::Point(j+25, i+25), 255, -1);
            }
        }
    }
    cv::imwrite("../test_images/chessboard.png", chess);
    
    // 4. Квадрат 100x100
    cv::Mat square = cv::Mat::zeros(200, 200, CV_8UC1);
    cv::rectangle(square, cv::Point(50, 50), cv::Point(150, 150), 255, -1);
    cv::imwrite("../test_images/square.png", square);
    
    // 5. Случайный шум
    cv::Mat noise = cv::Mat::zeros(200, 200, CV_8UC1);
    cv::randu(noise, 0, 255);
    cv::imwrite("../test_images/noise.png", noise);
    
    std::cout << "Test images created in ../test_images/ folder" << std::endl;
}

int main(int argc, char* argv[]) {

    std::cout << "============================================================" << std::endl;
    std::cout << "              IMAGE ANALYSIS                               " << std::endl;
    std::cout << "     IDM (Inverse Difference Moment) + Maximum Diameter    " << std::endl;
    std::cout << "============================================================" << std::endl;
    
    std::string imagePath;

    if (argc > 1) {
        imagePath = argv[1];
        std::cout << "\nImage path provided via command line: " << imagePath << std::endl;
    } else {
        std::cout << "\nEnter image path: ";
        std::getline(std::cin, imagePath);
    }
    
    AnalysisResults results = analyzeImage(imagePath);
    
    if (results.idm_value > 0 || results.diameter_result.maxDiameter > 0) {
        printResults(results);
        

        char saveChoice;
        std::cout << "\nSave results to file? (y/n): ";
        std::cin >> saveChoice;
        
        if (saveChoice == 'y' || saveChoice == 'Y') {
            std::string base_filename = imagePath.substr(imagePath.find_last_of("/\\") + 1);
            std::string::size_type const p(base_filename.find_last_of('.'));
            std::string file_without_ext = base_filename.substr(0, p);
            
            std::string filename = "../results/result_" + file_without_ext + ".txt";
            saveResultsToFile(results, filename);
        }

    } else {
        std::cout << "\nAnalysis failed. Check image path or file validity." << std::endl;
    }
    
    std::cout << "\nProgram completed." << std::endl;
    
    return 0;
}

