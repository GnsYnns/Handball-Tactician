#include "YoloDetector.hpp"
#include <iostream>

// Standard thresholds
constexpr float SCORE_THRESHOLD = 0.45f;
constexpr float NMS_THRESHOLD = 0.50f;

YoloDetector::YoloDetector(const std::string& modelPath) {
    std::cout << "[YoloDetector] Loading model: " << modelPath << std::endl;
    net_ = cv::dnn::readNetFromONNX(modelPath);

    if (net_.empty()) {
        throw std::runtime_error("Failed to load YOLO model!");
    }

    // Try to use CUDA if available, otherwise fall back to CPU
    try {
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    } catch (...) {
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
}

std::vector<Detection> YoloDetector::detect(const cv::Mat& frame) {
    // 1. Preprocess
    cv::Mat blob;
    // Normalization 1/255.0, SwapRB=true (OpenCV is BGR, YOLO expects RGB), Crop=false
    cv::dnn::blobFromImage(frame, blob, 1.0 / 255.0,
                           cv::Size(input_width, input_height),
                           cv::Scalar(), true, false);

    net_.setInput(blob);

    std::vector<cv::Mat> outputs;

    // --- CRITICAL FIX FOR OPENCV 4.6 ---
    // Instead of letting OpenCV guess, explicitly request the "output0" layer.
    // If it still crashes, verify the name with Netron.app (it is very likely "output0").
    try {
        net_.forward(outputs, "output0");
    } catch (const cv::Exception& e) {
        std::cerr << "[YoloDetector] Forward error: " << e.what() << std::endl;
        return {};
    }

    return parseOutput(outputs, frame.size());
}

std::vector<Detection> YoloDetector::parseOutput(const std::vector<cv::Mat>& outputs, cv::Size frameSize) {
    if (outputs.empty()) return {};

    // YOLOv8 standard output shape: [1, 84, 8400]
    // 84 channels: [cx, cy, w, h, class0_score, class1_score, ...]
    // 8400 anchors

    // Raw pointer to the data (float32)
    const float* data = (float*)outputs[0].data;

    // Dimensions
    const int dimensions = 84; // Number of feature channels
    const int rows = 8400;     // Number of anchors

    // If OpenCV flattened the output to [84, 8400], make sure to read it correctly.
    // No transpose needed: we iterate over the strided memory layout.

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    // Resize factors to map back to the original image
    float x_factor = (float)frameSize.width / input_width;
    float y_factor = (float)frameSize.height / input_height;

    // Iterate over the 8400 columns (anchors)
    for (int i = 0; i < rows; ++i) {
        // In memory [1, 84, 8400], element (feature, anchor) is at:
        // index = feature * rows + anchor
        // This is slightly unintuitive because YOLOv8 outputs [Features x Anchors] instead of [Anchors x Features].

        // Optimization: first find the max score among classes (indices 4..83)
        float max_score = 0.0f;
        int max_class_id = -1;

        // Scan classes manually (faster than wrapping cv::Mat in a hot loop)
        for (int c = 0; c < 80; ++c) {
            float score = data[(4 + c) * rows + i]; // Strided access
            if (score > max_score) {
                max_score = score;
                max_class_id = c;
            }
        }

        if (max_score > SCORE_THRESHOLD) {
            // Fetch bbox coords (indices 0, 1, 2, 3)
            float cx = data[0 * rows + i];
            float cy = data[1 * rows + i];
            float w  = data[2 * rows + i];
            float h  = data[3 * rows + i];

            int left = int((cx - 0.5 * w) * x_factor);
            int top = int((cy - 0.5 * h) * y_factor);
            int width = int(w * x_factor);
            int height = int(h * y_factor);

            boxes.push_back(cv::Rect(left, top, width, height));
            confidences.push_back(max_score);
            class_ids.push_back(max_class_id);
        }
    }

    // NMS (Non-Maximum Suppression)
    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);

    std::vector<Detection> detections;
    for (int idx : nms_result) {
        detections.push_back({ class_ids[idx], confidences[idx], boxes[idx] });
    }

    return detections;
}