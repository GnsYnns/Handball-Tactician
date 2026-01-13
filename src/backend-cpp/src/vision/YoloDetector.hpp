#pragma once
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>
#include <mutex>

struct Detection {
    int class_id;
    float confidence;
    cv::Rect box;
};

class YoloDetector {
public:
    static constexpr int input_width = 640;
    static constexpr int input_height = 640;

    // Constructor
    explicit YoloDetector(const std::string& modelPath);

    // Main method
    std::vector<Detection> detect(const cv::Mat& frame);

private:
    cv::dnn::Net net_;

    // Optimized parsing without unnecessary allocations
    std::vector<Detection> parseOutput(const std::vector<cv::Mat>& outputs, cv::Size frameSize);
};