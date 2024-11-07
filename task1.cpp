#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>

int main() {
    // Define the checkerboard dimensions
    cv::Size CHECKERBOARD(9, 6);
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    // Load the checkerboard image
    std::string image_path = "checkerboard.png";
    cv::Mat image = cv::imread(image_path);

    // Check if the image is loaded successfully
    if (image.empty()) {
        std::cerr << "Error: Could not load image at " << image_path << std::endl;
        return -1;
    }

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Find the chess board corners
    std::vector<cv::Point2f> corners;
    bool ret = cv::findChessboardCorners(gray, CHECKERBOARD, corners);

    // If found, refine the corner locations and draw them
    if (ret) {
        cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), criteria);
        
        // Draw and display the corners
        cv::drawChessboardCorners(image, CHECKERBOARD, corners, ret);
        
        // Print the number of corners and the coordinates of the first corner
        std::cout << "Number of corners found: " << corners.size() << std::endl;
        std::cout << "Coordinates of the first corner: " << corners[0].x << ", " << corners[0].y << std::endl;

        // Display the image with corners
        cv::imshow("Checkerboard", image);
        cv::waitKey(0);
    } else {
        std::cerr << "Error: Could not find chessboard corners." << std::endl;
    }

    cv::destroyAllWindows();

    // Generate a sample ARuco marker image
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::Mat markerImage;
    int markerId = 23; // Example marker ID
    int markerSize = 200; // Size of the marker image
    cv::aruco::generateImageMarker(dictionary, markerId, markerSize, markerImage);

    // Save the marker image to a file
    cv::imwrite("marker23.png", markerImage);

    // Load the marker image
    cv::Mat marker = cv::imread("marker23.png");

    // Check if the marker image is loaded successfully
    if (marker.empty()) {
        std::cerr << "Error: Could not load marker image." << std::endl;
        return -1;
    }

    cv::cvtColor(marker, gray, cv::COLOR_BGR2GRAY);

    // ARuco marker detection using ArucoDetector
    cv::aruco::DetectorParameters detectorParams;
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> aruco_corners;
    std::vector<std::vector<cv::Point2f>> rejectedCandidates;
    detector.detectMarkers(gray, aruco_corners, ids, rejectedCandidates);

    // Debug: Print the number of detected markers
    std::cout << "Number of ARuco markers detected: " << ids.size() << std::endl;

    // Draw detected markers
    if (!ids.empty()) {
        cv::aruco::drawDetectedMarkers(marker, aruco_corners, ids);
        cv::imshow("ARuco Markers", marker);
        cv::waitKey(0);
    } else {
        std::cout << "No ARuco markers detected." << std::endl;
    }

    cv::destroyAllWindows();

    return 0;
}