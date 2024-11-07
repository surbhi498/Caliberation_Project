#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    // Define the checkerboard dimensions
    cv::Size CHECKERBOARD(9, 6);
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    // Calibration data storage
    std::vector<std::vector<cv::Point2f>> corner_list;
    std::vector<std::vector<cv::Vec3f>> point_list;
    std::vector<cv::Vec3f> point_set;

    // Create the 3D world points for the checkerboard
    for (int i = 0; i < CHECKERBOARD.height; i++) {
        for (int j = 0; j < CHECKERBOARD.width; j++) {
            point_set.push_back(cv::Vec3f(j, -i, 0));
        }
    }

    while (true) {
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
        } else {
            std::cerr << "Error: Could not find chessboard corners." << std::endl;
        }

        // Wait for a key press
        char key = (char)cv::waitKey(0);
        if (key == 's' && ret) {
            corner_list.push_back(corners);
            point_list.push_back(point_set);
            std::cout << "Corners and 3D world points saved." << std::endl;

            // Save the image used for calibration
            std::string save_path = "calibration_image_" + std::to_string(corner_list.size()) + ".png";
            cv::imwrite(save_path, image);
            std::cout << "Calibration image saved as " << save_path << std::endl;
        } else if (key == 'q') {
            break;
        } else {
            std::cerr << "No corners saved." << std::endl;
        }

        cv::destroyAllWindows();
    }

    return 0;
}