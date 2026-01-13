#pragma once
#include <opencv2/dnn.hpp>
#include <vector>
#include <string_view>
#include <mutex>
#include <stdexcept>

struct Detection {
    int class_id;
    float confidence;
    cv::Rect box;
};

class YoloDetector {
public:
    static constexpr int input_width = 640;
    static constexpr int input_height = 640;
    static constexpr double scale_factor = 1.0 / 255.0;

    explicit YoloDetector(std::string_view modelPath) {
        net_ = cv::dnn::readNetFromONNX(std::string(modelPath));
        if (net_.empty()) {
            throw std::runtime_error("Failed to load YOLO model.");
        }
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    };

    YoloDetector(const YoloDetector&) = delete;
    YoloDetector& operator=(const YoloDetector&) = delete;

    YoloDetector(YoloDetector&&) noexcept = default;
    YoloDetector& operator=(YoloDetector&&) noexcept = default;

    std::vector<Detection> detect(const cv::Mat& frame) const {
        std::scoped_lock lock(mtx_);

        cv::Mat blob;
        cv::dnn::blobFromImage(frame, blob, scale_factor,
                               cv::Size(input_width, input_height),
                               cv::Scalar(), true, false);

        const_cast<cv::dnn::Net&>(net_).setInput(blob);
        std::vector<cv::Mat> outputs;
        const_cast<cv::dnn::Net&>(net_).forward(outputs, net_.getUnconnectedOutLayersNames());

        return parseOutput(outputs, frame.size());
    }

private:
    cv::dnn::Net net_;
    mutable std::mutex mtx_;

    std::vector<Detection> parseOutput(const std::vector<cv::Mat>& outputs, cv::Size frameSize) const;
};