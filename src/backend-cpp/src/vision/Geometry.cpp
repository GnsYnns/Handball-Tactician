#include "Geometry.hpp"

cv::Point2f PerspectiveTransformer::transform(const cv::Rect& box) const {
    if (homographyMatrix_.empty()) {
        // Fallback si pas de calibration : on renvoie le centre de la boite
        return cv::Point2f(box.x + box.width / 2.0f, box.y + box.height / 2.0f);
    }

    // Point source : Milieu bas de la boite (les pieds du joueur)
    // C'est crucial pour la précision tactique (la tête est projetée différemment des pieds)
    std::vector<cv::Point2f> srcPoints;
    srcPoints.push_back(cv::Point2f(box.x + box.width / 2.0f, box.y + box.height));

    std::vector<cv::Point2f> dstPoints;
    cv::perspectiveTransform(srcPoints, dstPoints, homographyMatrix_);

    return dstPoints[0];
}