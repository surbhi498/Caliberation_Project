# README for Augmented Reality Project using OpenCV
## Augmented Reality Project using OpenCV

This project involves several key tasks related to computer vision and augmented reality using OpenCV. The primary objectives are to calibrate a camera, detect a checkerboard pattern, estimate the camera's pose, and overlay a virtual object onto the real-world scene. Additionally, we explore feature detection techniques to identify robust features in a video stream.

## Table of Contents
Prerequisites
Installation
Usage
Project Structure
Features
Troubleshooting
Acknowledgements

## Prerequisites
OpenCV: Ensure you have OpenCV installed on your system. You can install it using Homebrew on macOS:
brew install opencv

## Installation
Clone the Repository:

git clone https://github.com/yourusername/augmented-reality-project.git
cd augmented-reality-project
Compile the Code:

Use the following command to compile the code with the correct include and library paths:
g++ -std=c++17 -I/usr/local/include/opencv4 -L/usr/local/lib -o task6 task6.cpp `pkg-config --cflags --libs opencv4`
g++ -std=c++17 -I/usr/local/include/opencv4 -L/usr/local/lib -o task7 task7.cpp `pkg-config --cflags --libs opencv4`

## Usage
Run the Camera Calibration and Virtual Object Projection:

./task6
Run the Feature Detection:

./task7

## Project Structure
task6.cpp: This file contains the code for camera calibration, pose estimation, and virtual object projection.
task7.cpp: This file contains the code for detecting robust features (Shi-Tomasi corners) in a video stream.
calibration_parameters.txt: This file contains the camera calibration parameters (camera matrix and distortion coefficients).
rotation_translation_vectors.txt: This file contains the rotation and translation vectors for each frame.
saved_frame.png: This file contains a saved frame with detected corners or features.
## Features
Camera Calibration:

Calibrate the camera using a checkerboard pattern to obtain the camera's intrinsic and extrinsic parameters.
## Pose Estimation:

Estimate the camera's pose relative to the checkerboard pattern using cv::solvePnP.

## Virtual Object Projection:

Design a 3D star-shaped virtual object and project it onto the image plane using the estimated pose.

## Feature Detection:

Detect Shi-Tomasi corners in a video stream and display the detected corners on the video frames.

## Troubleshooting
Camera Not Opening:

Ensure that your application has the necessary permissions to access the camera. On macOS, you can check this in System Preferences > Security & Privacy > Camera.
Test the camera with other applications (e.g., FaceTime, Photo Booth) to ensure it is working correctly.
If you have multiple cameras connected, try changing the camera index in the code. The default index is 0, but you can try 1 or 2 if you have multiple cameras.
Conflicts with Other Software:

Ensure that other software installations like OpenGL or XQuartz are not interfering with the camera access. Sometimes, these installations can cause conflicts.
Use the Activity Monitor on macOS to check for any running processes related to OpenGL, XQuartz, or other software that might be using the camera.
List installed packages using Homebrew to see if OpenGL, XQuartz, or other related software is installed:
brew list
## Acknowledgements
I would like to acknowledge the following resources and individuals for their contributions to this project:

## OpenCV Documentation: 
The comprehensive documentation provided by the OpenCV community was instrumental in understanding and implementing various computer vision techniques.
Online Tutorials and Forums: Numerous online tutorials and forums provided valuable insights and solutions to common issues encountered during the project.
Instructors and Peers: Guidance and feedback from instructors and peers were crucial in refining the project's approach and ensuring its successful completion.
By following the instructions in this README, you should be able to set up and run the augmented reality project using OpenCV. If you encounter any issues or have any questions, please feel free to reach out for assistance.

