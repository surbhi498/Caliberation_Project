#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

int main() {
    // Read the camera calibration parameters from a file
    cv::Mat camera_matrix, dist_coeffs;
    std::ifstream file("calibration_parameters.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Camera matrix:") != std::string::npos) {
                camera_matrix = cv::Mat::eye(3, 3, CV_64F);
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        file >> camera_matrix.at<double>(i, j);
                    }
                }
            } else if (line.find("Distortion coefficients:") != std::string::npos) {
                dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);
                for (int i = 0; i < 8; ++i) {
                    file >> dist_coeffs.at<double>(i);
                }
            }
        }
        file.close();
    } else {
        std::cerr << "Error: Could not open calibration_parameters.txt" << std::endl;
        return -1;
    }

    // Define the checkerboard dimensions
    cv::Size CHECKERBOARD(9, 6);
    std::vector<cv::Vec3f> point_set;
    for (int i = 0; i < CHECKERBOARD.height; i++) {
        for (int j = 0; j < CHECKERBOARD.width; j++) {
            point_set.push_back(cv::Vec3f(j, -i, 0));
        }
    }

    // Start video capture
    cv::VideoCapture cap(0); // Use the default camera (index 0)
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video capture" << std::endl;
        return -1;
    }

    // Open a file to save the rotation and translation vectors
    std::ofstream rt_file("rotation_translation_vectors.txt");
    if (!rt_file.is_open()) {
        std::cerr << "Error: Could not open rotation_translation_vectors.txt" << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame, gray;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame" << std::endl;
            break;
        }

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Find the chess board corners
        std::vector<cv::Point2f> corners;
        bool ret = cv::findChessboardCorners(gray, CHECKERBOARD, corners);

        // If found, refine the corner locations and draw them
        if (ret) {
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001));
            cv::drawChessboardCorners(frame, CHECKERBOARD, corners, ret);

            // Solve for pose
            cv::Mat rvec, tvec;
            cv::solvePnP(point_set, corners, camera_matrix, dist_coeffs, rvec, tvec);

            // Print rotation and translation vectors
            std::cout << "Rotation vector: " << rvec.t() << std::endl;
            std::cout << "Translation vector: " << tvec.t() << std::endl;

            // Save rotation and translation vectors to file
            rt_file << "Rotation vector: " << rvec.t() << std::endl;
            rt_file << "Translation vector: " << tvec.t() << std::endl;

            // Project 3D points onto the image plane
            std::vector<cv::Point3f> axes_points = { {0, 0, 0}, {3, 0, 0}, {0, 3, 0}, {0, 0, -3} };
            std::vector<cv::Point2f> image_points;
            cv::projectPoints(axes_points, rvec, tvec, camera_matrix, dist_coeffs, image_points);

            // Draw the axes
            cv::line(frame, image_points[0], image_points[1], cv::Scalar(0, 0, 255), 2);
            cv::line(frame, image_points[0], image_points[2], cv::Scalar(0, 255, 0), 2);
            cv::line(frame, image_points[0], image_points[3], cv::Scalar(255, 0, 0), 2);
        }

        // Display the frame
        cv::imshow("Video", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    rt_file.close();
    return 0;
}