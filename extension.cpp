#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Global variables for OpenGL
cv::Mat frame;
cv::VideoCapture cap;
cv::Mat camera_matrix, dist_coeffs;
cv::Mat rvec, tvec;
std::vector<cv::Point3f> cube_points = {
    {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, // Base
    {0, 0, -1}, {1, 0, -1}, {1, 1, -1}, {0, 1, -1} // Top
};

GLuint VAO, VBO;
GLuint shaderProgram;

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

void setupShaders() {
    std::cout << "Setting up shaders..." << std::endl;

    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Shaders set up successfully." << std::endl;
}

void setupCube() {
    std::cout << "Setting up cube..." << std::endl;

    GLfloat vertices[] = {
        // Base
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // Top
        0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        0.0f, 1.0f, -1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Cube set up successfully." << std::endl;
}

void drawCube() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_QUADS, 0, 8);
    glBindVertexArray(0);
}

void display() {
    std::cout << "Capturing frame..." << std::endl;
    cap >> frame;
    if (frame.empty()) {
        std::cerr << "Error: Could not capture frame" << std::endl;
        return;
    }

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Detect checkerboard corners
    std::vector<cv::Point2f> corners;
    cv::Size CHECKERBOARD(9, 6);
    bool ret = cv::findChessboardCorners(gray, CHECKERBOARD, corners);

    if (ret) {
        cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001));
        cv::drawChessboardCorners(frame, CHECKERBOARD, corners, ret);

        // Solve for pose
        std::vector<cv::Point3f> point_set;
        for (int i = 0; i < CHECKERBOARD.height; i++) {
            for (int j = 0; j < CHECKERBOARD.width; j++) {
                point_set.push_back(cv::Vec3f(j, -i, 0));
            }
        }
        cv::solvePnP(point_set, corners, camera_matrix, dist_coeffs, rvec, tvec);

        // Project cube points
        std::vector<cv::Point2f> image_points;
        cv::projectPoints(cube_points, rvec, tvec, camera_matrix, dist_coeffs, image_points);

        // Draw cube
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the view and projection matrices
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

        // Set up the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

        // Combine the matrices
        glm::mat4 mvp = projection * view * model;

        // Pass the MVP matrix to the shader
        GLuint mvpLocation = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

        drawCube();
    }

    cv::imshow("Video", frame);
    if (cv::waitKey(30) >= 0) exit(0);
}

int main(int argc, char** argv) {
    std::cout << "Reading calibration parameters..." << std::endl;

    // Read the camera calibration parameters from a file
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

    std::cout << "Calibration parameters read successfully." << std::endl;

    // Start video capture
    std::cout << "Starting video capture..." << std::endl;
    cap.open(0); // Use the default camera (index 0)
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video capture" << std::endl;
        return -1;
    }

    std::cout << "Video capture started successfully." << std::endl;

    // Initialize GLFW
    std::cout << "Initializing GLFW..." << std::endl;
    if (!glfwInit()) {
        std::cerr << "Error: Could not initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    std::cout << "Creating GLFW window..." << std::endl;
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Cube", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Error: Could not create GLFW window" << std::endl;
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    std::cout << "Initializing GLEW..." << std::endl;
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error: Could not initialize GLEW" << std::endl;
        return -1;
    }

    std::cout << "GLEW initialized successfully." << std::endl;

    // Setup shaders and cube
    setupShaders();
    setupCube();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render here
        display();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}