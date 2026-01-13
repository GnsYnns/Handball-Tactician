#pragma once
#include <opencv2/opencv.hpp>
#include <mutex>
#include <string>
#include <string_view>
#include <stdexcept>

class VideoSource {
public:
    explicit VideoSource(std::string_view filepath) {
        cap_.open(std::string(filepath));
        if (!cap_.isOpened()) {
            throw std::runtime_error("Failed to open video file: " + std::string(filepath));
        }
    }

    ~VideoSource() = default;

    VideoSource(const VideoSource&) = delete;
    VideoSource& operator=(const VideoSource&) = delete;

    VideoSource(VideoSource&& other) noexcept {
        std::scoped_lock lock(other.mtx_);
        cap_ = std::move(other.cap_);
    }

    VideoSource& operator=(VideoSource&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(mtx_, other.mtx_);
            cap_ = std::move(other.cap_);
        }
        return *this;
    }

    bool getFrame(cv::Mat& frame) {
        std::lock_guard<std::mutex> lock(mtx_);
        return cap_.read(frame);
    }

    cv::Size getResolution() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return { static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH)),
                 static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT)) };
    }

private:
    cv::VideoCapture cap_;
    mutable std::mutex mtx_;
};