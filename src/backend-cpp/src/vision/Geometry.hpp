#pragma once
#include <opencv2/opencv.hpp>


class PerspectiveTransformer {
public:
    PerspectiveTransformer() = default;

    explicit PerspectiveTransformer(const cv::Mat& h) : homographyMatrix_(h.clone()) {}

    void setHomography(cv::Mat h) {
        homographyMatrix_ = std::move(h);
    }

    cv::Point2f transform(const cv::Rect& box) const;

private:
    cv::Mat homographyMatrix_;
};
