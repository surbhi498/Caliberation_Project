#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

int main() {
    // Define the checkerboard dimensions
    cv::Size CHECKERBOARD(9, 6);
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    // Calibration data storage
    std::vector<std::vector<cv::Point2f>> corner_list; // List of 2D image points
    std::vector<std::vector<cv::Vec3f>> point_list;    // List of 3D world points
    std::vector<cv::Vec3f> point_set;                  // 3D world points for one image

    // Create the 3D world points for the checkerboard
    for (int i = 0; i < CHECKERBOARD.height; i++) {
        for (int j = 0; j < CHECKERBOARD.width; j++) {
            point_set.push_back(cv::Vec3f(j, -i, 0));
        }
    }

    // Directory containing the calibration images
    std::string image_directory = "images";

    cv::Mat gray; // Define gray here so it's accessible later

    // Iterate over all images in the directory
    for (const auto& entry : std::filesystem::directory_iterator(image_directory)) {
        std::string image_path = entry.path().string();

        // Skip non-image files
        if (entry.path().extension() != ".jpeg" && entry.path().extension() != ".jpg" && entry.path().extension() != ".png") {
            continue;
        }

        cv::Mat image = cv::imread(image_path);

        // Check if the image is loaded successfully
        if (image.empty()) {
            std::cerr << "Error: Could not load image at " << image_path << std::endl;
            continue;
        }

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
            cv::waitKey(1000); // Display each image for 1000 ms

            // Save the detected corners and corresponding 3D world points
            corner_list.push_back(corners);
            point_list.push_back(point_set);
            std::cout << "Corners and 3D world points saved for image: " << image_path << std::endl;
        } else {
            std::cerr << "Error: Could not find chessboard corners in image: " << image_path << std::endl;
        }

        cv::destroyAllWindows();
    }

    // If at least 5 calibration images have been selected, run the calibration
    if (corner_list.size() >= 5) {
        cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
        camera_matrix.at<double>(0, 2) = gray.cols / 2.0;
        camera_matrix.at<double>(1, 2) = gray.rows / 2.0;
        cv::Mat dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);
        std::vector<cv::Mat> rvecs, tvecs;

        std::cout << "Camera matrix before calibration:\n" << camera_matrix << std::endl;
        std::cout << "Distortion coefficients before calibration:\n" << dist_coeffs << std::endl;

        double reprojection_error = cv::calibrateCamera(point_list, corner_list, gray.size(), camera_matrix, dist_coeffs, rvecs, tvecs);

        std::cout << "Calibration successful!" << std::endl;
        std::cout << "Reprojection error: " << reprojection_error << std::endl;
        std::cout << "Camera matrix after calibration:\n" << camera_matrix << std::endl;
        std::cout << "Distortion coefficients after calibration:\n" << dist_coeffs << std::endl;

        // Save the intrinsic parameters to a file
        std::ofstream file("calibration_parameters.txt");
        file << "Camera matrix:\n" << camera_matrix << "\n";
        file << "Distortion coefficients:\n" << dist_coeffs << "\n";
        file << "Reprojection error:\n" << reprojection_error << "\n";
        file.close();
        std::cout << "Calibration parameters saved to calibration_parameters.txt" << std::endl;

        // Save the rotations and translations
        std::ofstream rt_file("rotations_translations.txt");
        for (size_t i = 0; i < rvecs.size(); ++i) {
            rt_file << "Image " << i + 1 << ":\n";
            rt_file << "Rotation vector:\n" << rvecs[i] << "\n";
            rt_file << "Translation vector:\n" << tvecs[i] << "\n";
        }
        rt_file.close();
        std::cout << "Rotations and translations saved to rotations_translations.txt" << std::endl;
    } else {
        std::cerr << "Not enough calibration images. At least 5 are required." << std::endl;
    }

    return 0;
}