#include "TeamClassifier.hpp"

// IDs COCO Dataset
constexpr int CLASS_PERSON = 0;
constexpr int CLASS_BALL = 32;

EntityType TeamClassifier::predict(const cv::Mat& roi, int class_id) const {
    // 1. Classification par type d'objet YOLO
    if (class_id == CLASS_BALL) {
        return EntityType::Ball;
    }
    if (class_id != CLASS_PERSON) {
        return EntityType::Unknown;
    }

    // 2. Analyse Couleur pour les joueurs
    if (roi.empty()) return EntityType::Unknown;

    // Convertir en HSV pour être robuste aux changements de luminosité
    cv::Mat hsv;
    cv::cvtColor(roi, hsv, cv::COLOR_BGR2HSV);

    // On calcule la couleur moyenne de la zone (optimisation: prendre juste le centre du maillot)
    // Ici on prend tout le ROI pour le MVP
    cv::Scalar mean_color = cv::mean(hsv);
    
    // Déstructuration pour lisibilité
    double h = mean_color[0];
    double s = mean_color[1];
    double v = mean_color[2];

    // Logique basée sur vos seuils (à tuner avec les logs)
    // Team A (Blancs) : Saturation faible, Valeur haute
    if (s < 50 && v > 180) {
        return EntityType::TeamA;
    }

    // Team B (Bleus) : Teinte bleue (environ 100-130), Saturation moyenne+
    if (h > 90 && h < 130 && s > 70) {
        return EntityType::TeamB;
    }

    // Si c'est un arbitre (souvent jaune ou noir distinct) ou incertain
    return EntityType::Unknown;
}