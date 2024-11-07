#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <vector>

class ArucoDetector {
public:
    ArucoDetector(cv::aruco::PredefinedDictionaryType dict_type = cv::aruco::DICT_6X6_250) {
        dictionary = cv::aruco::getPredefinedDictionary(dict_type);
        parameters = cv::aruco::DetectorParameters::create();
    }

    // Function to generate an ARuco marker
    void generateMarker(int id, int size_pixels, const std::string& filename) {
        cv::Mat marker_image;
        cv::aruco::drawMarker(dictionary, id, size_pixels, marker_image);
        cv::imwrite(filename, marker_image);
        std::cout << "Generated marker " << id << " saved as " << filename << std::endl;
    }

    // Function to detect markers in an image
    bool detectMarkers(const cv::Mat& image, bool draw_output = true) {
        // Convert to grayscale
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        // Detect markers
        std::vector<std::vector<cv::Point2f>> corners;
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> rejected;
        
        cv::aruco::detectMarkers(gray, dictionary, corners, ids, parameters, rejected);

        // Print results and draw markers if found
        if (ids.size() > 0) {
            std::cout << "Found " << ids.size() << " markers" << std::endl;
            
            // Print details for each marker
            for (size_t i = 0; i < ids.size(); i++) {
                std::cout << "Marker " << ids[i] << " corners:" << std::endl;
                for (size_t j = 0; j < corners[i].size(); j++) {
                    std::cout << "  Corner " << j << ": (" 
                             << corners[i][j].x << ", " << corners[i][j].y << ")" << std::endl;
                }
            }

            if (draw_output) {
                cv::aruco::drawDetectedMarkers(image, corners, ids);
            }
            return true;
        } else {
            std::cout << "No markers detected" << std::endl;
            return false;
        }
    }

private:
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> parameters;
};

int main() {
    ArucoDetector detector;

    // Generate some example markers (optional)
    std::cout << "Generating example markers..." << std::endl;
    for (int i = 0; i < 4; i++) {
        detector.generateMarker(i, 200, "marker" + std::string(1, '0' + i) + ".png");
    }

    // Load an image containing ARuco markers
    std::string image_path = "checkerboard.png"; // Replace with your image path
    cv::Mat image = cv::imread(image_path);

    if (image.empty()) {
        std::cerr << "Error: Could not load image at " << image_path << std::endl;
        return -1;
    }

    // Create a window for display
    cv::namedWindow("ARuco Detection", cv::WINDOW_NORMAL);

    // Make a copy of the image for drawing
    cv::Mat output_image = image.clone();

    // Detect and draw markers
    std::cout << "Detecting markers..." << std::endl;
    if (detector.detectMarkers(output_image)) {
        // Display the result
        cv::imshow("ARuco Detection", output_image);
        
        // Save the result
        cv::imwrite("detected_markers.png", output_image);
        std::cout << "Detection result saved as 'detected_markers.png'" << std::endl;
    }

    std::cout << "Press any key to exit..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}