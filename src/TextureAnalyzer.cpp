#include "TextureAnalyzer.h"

TextureAnalyzer::TextureAnalyzer(int levels) : levels_(levels), totalPairs_(0) {
    glcm_.assign(levels_, std::vector<int>(levels_, 0));
}

void TextureAnalyzer::buildGLCM(const cv::Mat& image, int dx, int dy) {
    if (image.empty() || image.type() != CV_8UC1) {
        std::cerr << "Error: Image must be grayscale" << std::endl;
        return;
    }
    
    clear();
    
    int maxY = image.rows - std::abs(dy);
    int maxX = image.cols - std::abs(dx);
    
    for (int y = 0; y < maxY; y++) {
        for (int x = 0; x < maxX; x++) {
            int currentPixel = image.at<uchar>(y, x);
            int neighborPixel = image.at<uchar>(y + dy, x + dx);
            
            if (currentPixel >= 0 && currentPixel < levels_ && 
                neighborPixel >= 0 && neighborPixel < levels_) {
                glcm_[currentPixel][neighborPixel]++;
                totalPairs_++;
            }
        }
    }
    
    std::cout << "GLCM built: direction (" << dx << "," << dy 
              << "), total pairs: " << totalPairs_ << std::endl;
}

double TextureAnalyzer::calculateIDM() {
    if (totalPairs_ == 0) {
        std::cerr << "Error: GLCM not built or empty" << std::endl;
        return 0.0;
    }
    
    double idm = 0.0;
    
    for (int i = 0; i < levels_; i++) {
        for (int j = 0; j < levels_; j++) {
            if (glcm_[i][j] > 0) {
                double p_ij = static_cast<double>(glcm_[i][j]) / totalPairs_;
                double denominator = 1.0 + (i - j) * (i - j);
                idm += p_ij / denominator;
            }
        }
    }
    
    std::cout << "IDM calculated: " << std::fixed << std::setprecision(6) << idm << std::endl;
    return idm;
}

std::vector<std::vector<double>> TextureAnalyzer::getNormalizedGLCM() const {
    std::vector<std::vector<double>> normalized(levels_, std::vector<double>(levels_, 0.0));
    
    if (totalPairs_ > 0) {
        for (int i = 0; i < levels_; i++) {
            for (int j = 0; j < levels_; j++) {
                normalized[i][j] = static_cast<double>(glcm_[i][j]) / totalPairs_;
            }
        }
    }
    
    return normalized;
}

void TextureAnalyzer::printGLCMStats() const {
    if (totalPairs_ == 0) {
        std::cout << "GLCM not built" << std::endl;
        return;
    }
    
    int maxValue = 0;
    int nonZeroElements = 0;
    
    for (int i = 0; i < levels_; i++) {
        for (int j = 0; j < levels_; j++) {
            if (glcm_[i][j] > maxValue) {
                maxValue = glcm_[i][j];
            }
            if (glcm_[i][j] > 0) {
                nonZeroElements++;
            }
        }
    }
    
    std::cout << "GLCM Statistics:" << std::endl;
    std::cout << "Matrix size: " << levels_ << "x" << levels_ << std::endl;
    std::cout << "Total pixel pairs: " << totalPairs_ << std::endl;
    std::cout << "Non-zero elements: " << nonZeroElements << std::endl;
    std::cout << "Maximum value: " << maxValue << std::endl;
    std::cout << "Fill ratio: " << (100.0 * nonZeroElements) / (levels_ * levels_) 
              << "%" << std::endl;
}

double TextureAnalyzer::analyzeMultiDirectional(const cv::Mat& image) {
    if (image.empty()) {
        return 0.0;
    }
    
    std::vector<std::pair<int, int>> directions = {
        {1, 0},   // Horizontal
        {0, 1},   // Vertical
        {1, 1},   // Diagonal
        {1, -1}   // Anti-diagonal
    };
    
    double totalIDM = 0.0;
    int validDirections = 0;
    
    std::cout << "Multi-directional texture analysis" << std::endl;
    
    for (const auto& dir : directions) {
        buildGLCM(image, dir.first, dir.second);
        double idm = calculateIDM();
        
        if (idm > 0) {
            totalIDM += idm;
            validDirections++;
            
            std::cout << "Direction (" << dir.first << "," << dir.second 
                      << "): IDM = " << std::fixed << std::setprecision(4) << idm << std::endl;
        }
    }
    
    double averageIDM = (validDirections > 0) ? totalIDM / validDirections : 0.0;
    std::cout << "Average IDM: " << std::fixed << std::setprecision(4) 
              << averageIDM << std::endl;
    
    return averageIDM;
}

void TextureAnalyzer::clear() {
    for (auto& row : glcm_) {
        std::fill(row.begin(), row.end(), 0);
    }
    totalPairs_ = 0;
}

