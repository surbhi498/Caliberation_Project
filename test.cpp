#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Start video capture using AVFoundation backend
    cv::VideoCapture cap(0, cv::CAP_AVFOUNDATION); // Use the default camera (index 0)
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video capture" << std::endl;
        return -1;
    } else {
        std::cout << "Video capture opened successfully." << std::endl;
    }

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame" << std::endl;
            break;
        }

        // Display the frame
        cv::imshow("Camera Test", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    return 0;
}