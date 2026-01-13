#pragma once
#include <opencv2/opencv.hpp>

enum class EntityType { TeamA, TeamB, Ball, Referee, Unknown };

class TeamClassifier {
public:
    TeamClassifier() = default;

    EntityType predict(const cv::Mat& roi, int class_id) const;

private:
    // Organisation des données liées dans une structure interne [4, 9].
    struct ColorRange {
        cv::Scalar lower;
        cv::Scalar upper;
    };


    // Encapsulation des seuils HSV en privé [10, 11].
    ColorRange thresholdsA_{ cv::Scalar(0, 0, 0), cv::Scalar(180, 255, 50) };
    ColorRange thresholdsB_{ cv::Scalar(0, 0, 0), cv::Scalar(180, 255, 50) };
};