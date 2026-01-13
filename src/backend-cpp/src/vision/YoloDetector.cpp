#include "YoloDetector.hpp"

// Constantes pour YOLOv8 (COCO dataset)
constexpr float SCORE_THRESHOLD = 0.45f;
constexpr float NMS_THRESHOLD = 0.50f;

std::vector<Detection> YoloDetector::parseOutput(const std::vector<cv::Mat>& outputs, cv::Size frameSize) const {
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    // YOLOv8 output: [1, 4 + classes, 8400]
    // La matrice est souvent transposée par rapport aux anciennes versions
    // Data layout: cx, cy, w, h, score_class0, score_class1...
    
    // Accès au pointeur brut pour la vitesse (pointer arithmetic)
    float* data = (float*)outputs[0].data;
    
    // Dimensions du tenseur de sortie
    const int dimensions = 4 + 80; // 4 coords + 80 classes COCO
    const int rows = 8400;         // Nombre d'ancres

    // Attention : OpenCV DNN avec ONNX peut inverser rows/cols selon l'export.
    // Si la sortie est [1, 84, 8400], on itère sur les 8400 colonnes.
    
    // Facteurs de mise à l'échelle pour remettre les bbox à la taille de l'image d'origine
    float x_factor = (float)frameSize.width / input_width;
    float y_factor = (float)frameSize.height / input_height;

    for (int i = 0; i < rows; ++i) {
        // Dans YOLOv8 exporté standard, les classes sont après les 4 coords
        // Mais il faut trouver le score max parmi les classes
        float* classes_scores = data + 4;
        
        cv::Mat scores(1, 80, CV_32FC1, classes_scores);
        cv::Point class_id_point;
        double max_class_score;
        cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id_point);

        if (max_class_score > SCORE_THRESHOLD) {
            float cx = data[0];
            float cy = data[1];
            float w = data[2];
            float h = data[3];

            int left = int((cx - 0.5 * w) * x_factor);
            int top = int((cy - 0.5 * h) * y_factor);
            int width = int(w * x_factor);
            int height = int(h * y_factor);

            boxes.push_back(cv::Rect(left, top, width, height));
            confidences.push_back((float)max_class_score);
            class_ids.push_back(class_id_point.x);
        }
        // Sauter au prochain vecteur de données (stride)
        data += dimensions; 
    }

    // NMS (Non-Maximum Suppression) pour supprimer les boites qui se chevauchent
    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);

    std::vector<Detection> detections;
    for (int idx : nms_result) {
        detections.push_back({ class_ids[idx], confidences[idx], boxes[idx] });
    }

    return detections;
}