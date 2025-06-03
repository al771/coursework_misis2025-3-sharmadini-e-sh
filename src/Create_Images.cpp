// Добавьте эту функцию в main.cpp для создания тестов
void createTestImages() {
    // 1. Однородный серый квадрат
    cv::Mat uniform = cv::Mat::ones(200, 200, CV_8UC1) * 128;
    cv::imwrite("test_images/uniform_gray.png", uniform);
    
    // 2. Белый круг на черном фоне
    cv::Mat circle = cv::Mat::zeros(200, 200, CV_8UC1);
    cv::circle(circle, cv::Point(100, 100), 50, 255, -1);
    cv::imwrite("test_images/white_circle.png", circle);
    
    // 3. Шахматная доска
    cv::Mat chess = cv::Mat::zeros(200, 200, CV_8UC1);
    for (int i = 0; i < 200; i += 25) {
        for (int j = 0; j < 200; j += 25) {
            if (((i/25) + (j/25)) % 2 == 0) {
                cv::rectangle(chess, cv::Point(j, i), cv::Point(j+25, i+25), 255, -1);
            }
        }
    }
    cv::imwrite("test_images/chessboard.png", chess);
    
    // 4. Квадрат 100x100
    cv::Mat square = cv::Mat::zeros(200, 200, CV_8UC1);
    cv::rectangle(square, cv::Point(50, 50), cv::Point(150, 150), 255, -1);
    cv::imwrite("test_images/square.png", square);
    
    // 5. Случайный шум
    cv::Mat noise = cv::Mat::zeros(200, 200, CV_8UC1);
    cv::randu(noise, 0, 255);
    cv::imwrite("test_images/noise.png", noise);
    
    std::cout << "Test images created in test_images/ folder" << std::endl;
}

int main(){
createTestImages();

return 0;
}
