

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

// Function to draw 3D objects (pyramid, cube, and prism) on the image
void draw3dObject(cv::Mat &src, cv::Mat &camera_matrix, cv::Mat &dist_coeff, cv::Mat &rot, cv::Mat &trans) {
    // PYRAMID
    std::vector<cv::Vec3f> pyramid_points;
    pyramid_points.push_back(cv::Vec3f({0, 0, -3})); // apex
    pyramid_points.push_back(cv::Vec3f({1, 1, 0})); // tr
    pyramid_points.push_back(cv::Vec3f({1, -1, 0})); // br
    pyramid_points.push_back(cv::Vec3f({-1, -1, 0})); // bl
    pyramid_points.push_back(cv::Vec3f({-1, 1, 0})); // tl
    
    std::vector<cv::Point2f> pyramid_corners;
    
    cv::projectPoints(pyramid_points, rot, trans, camera_matrix, dist_coeff, pyramid_corners);

    std::cout << "Pyramid corners: ";
    for (const auto& corner : pyramid_corners) {
        std::cout << "(" << corner.x << ", " << corner.y << ") ";
    }
    std::cout << std::endl;

    cv::line(src, pyramid_corners[0], pyramid_corners[1], cv::Scalar(0, 255, 255), 2);
    cv::line(src, pyramid_corners[0], pyramid_corners[2], cv::Scalar(0, 255, 255), 2);
    cv::line(src, pyramid_corners[0], pyramid_corners[3], cv::Scalar(0, 255, 255), 2);
    cv::line(src, pyramid_corners[0], pyramid_corners[4], cv::Scalar(0, 255, 255), 2);
    cv::line(src, pyramid_corners[1], pyramid_corners[2], cv::Scalar(0, 255, 255), 2);
    cv::line(src, pyramid_corners[2], pyramid_corners[3], cv::Scalar(0, 255, 255), 2);
    cv::line(src, pyramid_corners[3], pyramid_corners[4], cv::Scalar(0, 255, 255), 2);
    cv::line(src, pyramid_corners[4], pyramid_corners[1], cv::Scalar(0, 255, 255), 2);
        
    pyramid_points.clear();
    pyramid_corners.clear();
    
    // CUBE
    std::vector<cv::Vec3f> cube_points;
    cube_points.push_back(cv::Vec3f({2, 2, -2})); // trf
    cube_points.push_back(cv::Vec3f({2, 0, -2})); // brf
    cube_points.push_back(cv::Vec3f({0, 0, -2})); // blf
    cube_points.push_back(cv::Vec3f({0, 2, -2})); // tlf
    cube_points.push_back(cv::Vec3f({2, 2, 0})); // trb
    cube_points.push_back(cv::Vec3f({2, 0, 0})); // brb
    cube_points.push_back(cv::Vec3f({0, 0, 0})); // blb
    cube_points.push_back(cv::Vec3f({0, 2, 0})); // tlb
    
    std::vector<cv::Point2f> cube_corners;
    
    cv::projectPoints(cube_points, rot, trans, camera_matrix, dist_coeff, cube_corners);

    std::cout << "Cube corners: ";
    for (const auto& corner : cube_corners) {
        std::cout << "(" << corner.x << ", " << corner.y << ") ";
    }
    std::cout << std::endl;

    cv::line(src, cube_corners[0], cube_corners[1], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[1], cube_corners[2], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[2], cube_corners[3], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[3], cube_corners[0], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[4], cube_corners[5], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[5], cube_corners[6], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[6], cube_corners[7], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[7], cube_corners[4], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[0], cube_corners[4], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[1], cube_corners[5], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[2], cube_corners[6], cv::Scalar(255, 255, 0), 2);
    cv::line(src, cube_corners[3], cube_corners[7], cv::Scalar(255, 255, 0), 2);
    
    cube_points.clear();
    cube_corners.clear();
    
    // PRISM
    std::vector<cv::Vec3f> prism_points;
    prism_points.push_back(cv::Vec3f({-2, -2, -1})); // trf
    prism_points.push_back(cv::Vec3f({-2, -4, -1})); // brf
    prism_points.push_back(cv::Vec3f({-4, -4, -1})); // blf
    prism_points.push_back(cv::Vec3f({-4, -2, -1})); // tlf
    prism_points.push_back(cv::Vec3f({-3, -3, 1})); // apex
    
    std::vector<cv::Point2f> prism_corners;
    
    cv::projectPoints(prism_points, rot, trans, camera_matrix, dist_coeff, prism_corners);

    std::cout << "Prism corners: ";
    for (const auto& corner : prism_corners) {
        std::cout << "(" << corner.x << ", " << corner.y << ") ";
    }
    std::cout << std::endl;

    cv::line(src, prism_corners[0], prism_corners[1], cv::Scalar(255, 0, 255), 2);
    cv::line(src, prism_corners[1], prism_corners[2], cv::Scalar(255, 0, 255), 2);
    cv::line(src, prism_corners[2], prism_corners[3], cv::Scalar(255, 0, 255), 2);
    cv::line(src, prism_corners[3], prism_corners[0], cv::Scalar(255, 0, 255), 2);
    cv::line(src, prism_corners[0], prism_corners[4], cv::Scalar(255, 0, 255), 2);
    cv::line(src, prism_corners[1], prism_corners[4], cv::Scalar(255, 0, 255), 2);
    cv::line(src, prism_corners[2], prism_corners[4], cv::Scalar(255, 0, 255), 2);
    cv::line(src, prism_corners[3], prism_corners[4], cv::Scalar(255, 0, 255), 2);
}

bool isImageFile(const std::string& filename) {
    std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp", ".tiff"};
    for (const auto& ext : extensions) {
        if (filename.size() >= ext.size() && filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0) {
            return true;
        }
    }
    return false;
}

int main() {
    // Read the camera calibration parameters from a file
    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 3798.033892402914, 0, 666.3589707850728,
                                                       0, 2547.018779697359, 544.6239271635342,
                                                       0, 0, 1);
    cv::Mat dist_coeffs = (cv::Mat_<double>(5, 1) << 4.122649975238361,
                                                     -1623.806667310265,
                                                     0.7493398563977504,
                                                     0.06095266374411317,
                                                     -34.614502945302644);

    // Print calibration parameters for debugging
    std::cout << "Camera matrix:\n" << camera_matrix << std::endl;
    std::cout << "Distortion coefficients:\n" << dist_coeffs << std::endl;

    // Define the checkerboard dimensions
    cv::Size CHECKERBOARD(9, 6);
    std::vector<cv::Vec3f> point_set;
    for (int i = 0; i < CHECKERBOARD.height; i++) {
        for (int j = 0; j < CHECKERBOARD.width; j++) {
            point_set.push_back(cv::Vec3f(j, -i, 0));
        }
    }

    // Directory containing the images
    std::string image_directory = "images"; // Change this to your image directory

    // Iterate through the images in the directory
    for (const auto& entry : std::filesystem::directory_iterator(image_directory)) {
        std::string image_path = entry.path().string();
        std::cout << "Checking file: " << image_path << std::endl;
        if (!isImageFile(image_path)) {
            std::cout << "Skipping non-image file: " << image_path << std::endl;
            continue;
        }

        std::cout << "Processing image: " << image_path << std::endl;
        cv::Mat frame = cv::imread(image_path);
        if (frame.empty()) {
            std::cerr << "Error: Could not open image " << image_path << std::endl;
            continue;
        }

        cv::Mat gray;
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

            // Draw 3D objects on the image
            std::cout << "Drawing 3D objects..." << std::endl;
            draw3dObject(frame, camera_matrix, dist_coeffs, rvec, tvec);

            // Save the frame to a file
            std::string output_filename = "output_" + entry.path().filename().string();
            cv::imwrite(output_filename, frame);
            std::cout << "Frame saved as " << output_filename << std::endl;
        } else {
            std::cerr << "Error: Could not find chessboard corners in image " << image_path << std::endl;
        }

        // Display the frame
        cv::imshow("Image", frame);
        if (cv::waitKey(0) >= 0) break; // Wait for a key press to move to the next image
    }

    return 0;
}