// src/backend-cpp/src/main_debug.cpp
#include <iostream>
#include <opencv2/opencv.hpp>
#include "io/VideoSource.hpp"
#include "vision/YoloDetector.hpp"
#include "vision/TeamClassifier.hpp"

// Utility function for drawing
void drawDebug(cv::Mat& frame, const Detection& det, EntityType type, const cv::Scalar& meanColor) {
    cv::Scalar colorBox;
    std::string label;

    switch (type) {
        case EntityType::TeamA:
            colorBox = cv::Scalar(255, 255, 255); // White for Team A
            label = "Team A";
            break;
        case EntityType::TeamB:
            colorBox = cv::Scalar(255, 0, 0); // Blue for Team B
            label = "Team B";
            break;
        case EntityType::Ball:
            colorBox = cv::Scalar(0, 255, 255); // Yellow for Ball
            label = "Ball";
            break;
        default:
            colorBox = cv::Scalar(0, 0, 255); // Red for Unknown
            label = "Unknown";
            break;
    }

    // Draw the box
    cv::rectangle(frame, det.box, colorBox, 2);

    // Prepare debug text
    std::stringstream ss;
    ss << label << " Conf:" << std::fixed << std::setprecision(2) << det.confidence;

    // If this is a person, print HSV values for calibration
    if (type == EntityType::TeamA || type == EntityType::TeamB || type == EntityType::Unknown) {
        if (det.class_id == 0) { // Person
             ss << " H:" << (int)meanColor[0] << " S:" << (int)meanColor[1] << " V:" << (int)meanColor[2];
        }
    }

    cv::putText(frame, ss.str(), cv::Point(det.box.x, det.box.y - 5),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, colorBox, 1);
}

int main() {
    try {
        // 1. Load components
        // Make sure the paths are correct relative to your executable!
        std::string videoPath = "/home/ygonos/side-project/Handball-Tactician/data/videohand.mp4";
        std::string modelPath = "/home/ygonos/side-project/Handball-Tactician/data/yolov8n.onnx";

        std::cout << "[DEBUG] Loading video..." << std::endl;
        VideoSource video(videoPath);

        std::cout << "[DEBUG] Loading AI model..." << std::endl;

        YoloDetector detector(modelPath);

        std::cout << "[DEBUG] Everything is loaded!" << std::endl;

        TeamClassifier classifier;

        cv::Mat frame;
        int frameCount = 0;

        // 2. Read loop (press ESC to quit)
        while (video.getFrame(frame)) {
            frameCount++;

            // Process 1 frame out of 3 for faster visual debugging
            if (frameCount % 3 != 0) continue;

            auto start = std::chrono::steady_clock::now();

            // A. Detection
            auto detections = detector.detect(frame);

            // B. Classification & drawing
            for (const auto& det : detections) {
                // Check bounds
                cv::Rect roiBox = det.box & cv::Rect(0, 0, frame.cols, frame.rows);
                if (roiBox.area() <= 0) continue;

                cv::Mat roi = frame(roiBox);

                // Compute mean color only to display it in the console/UI
                cv::Mat hsv;
                cv::cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
                cv::Scalar meanColor = cv::mean(hsv);

                EntityType type = classifier.predict(roi, det.class_id);

                drawDebug(frame, det, type, meanColor);
            }

            auto end = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            // C. Display
            std::string fpsInfo = "Inference: " + std::to_string(diff) + "ms";
            cv::putText(frame, fpsInfo, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

            cv::imshow("Handball Debugger (Press ESC to quit)", frame);

            // Press SPACE to pause frame-by-frame, ESC to quit
            char key = (char)cv::waitKey(30);
            if (key == 27) break; // ESC
            if (key == 32) cv::waitKey(0); // PAUSE
        }

    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}