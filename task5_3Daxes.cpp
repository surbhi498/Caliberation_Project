#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

// Function to project and draw 3D coordinate axes on the image
void project3DAxes(cv::Mat &frame, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, const cv::Vec3d &rvec, const cv::Vec3d &tvec) {
    std::vector<cv::Point3f> axis = { {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, -1} };
    std::vector<cv::Point2f> imagePoints;
    cv::projectPoints(axis, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    // Draw the axes
    cv::line(frame, imagePoints[0], imagePoints[1], cv::Scalar(0, 0, 255), 5); // X-axis in red
    cv::line(frame, imagePoints[0], imagePoints[2], cv::Scalar(0, 255, 0), 5); // Y-axis in green
    cv::line(frame, imagePoints[0], imagePoints[3], cv::Scalar(255, 0, 0), 5); // Z-axis in blue
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

            // Project and draw 3D coordinate axes on the image
            project3DAxes(frame, camera_matrix, dist_coeffs, rvec, tvec);

            // Save the frame to a file
            std::string output_filename = "output1_" + entry.path().filename().string();
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