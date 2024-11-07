#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Start video capture
    cv::VideoCapture cap(0); // Use the default camera (index 0)
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video capture" << std::endl;
        return -1;
    }

    bool frame_saved = false;

    while (true) {
        cv::Mat frame, gray;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame" << std::endl;
            break;
        }

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detect Shi-Tomasi corners
        std::vector<cv::Point2f> corners;
        double qualityLevel = 0.01;
        double minDistance = 10;
        int blockSize = 3;
        bool useHarrisDetector = false;
        double k = 0.04;

        cv::goodFeaturesToTrack(gray, corners, 100, qualityLevel, minDistance, cv::Mat(), blockSize, useHarrisDetector, k);

        // Draw circles around corners
        for (size_t i = 0; i < corners.size(); i++) {
            cv::circle(frame, corners[i], 5, cv::Scalar(0, 255, 0), 2, 8, 0);
        }

        std::cout << "Number of corners detected: " << corners.size() << std::endl;

        // Save the frame to a file if not already saved and corners are detected
        if (!frame_saved && !corners.empty()) {
            std::string filename = "saved_frame.png";
            bool result = cv::imwrite(filename, frame);
            if (result) {
                std::cout << "Frame saved as " << filename << std::endl;
            } else {
                std::cerr << "Error: Could not save frame" << std::endl;
            }
            frame_saved = true;
        }

        // Display the frame
        cv::imshow("Shi-Tomasi Corners", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    return 0;
}