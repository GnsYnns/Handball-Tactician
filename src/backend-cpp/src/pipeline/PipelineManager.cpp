#include "PipelineManager.hpp"
#include <iostream>

PipelineManager::PipelineManager(const std::string& videoPath, const std::string& modelPath)
    : video_(videoPath), detector_(modelPath) 
{
    // Initialisation d'une matrice d'identité temporaire pour éviter les crashs avant calibration
    transformer_.setHomography(cv::Mat::eye(3, 3, CV_64F));
}

PipelineManager::~PipelineManager() {
    stop();
}

void PipelineManager::stop() {
    running_ = false;
    queue_.request_stop(); // Réveille le consommateur pour qu'il sorte du wait
    if (producerThread_.joinable()) {
        producerThread_.join();
    }
}

void PipelineManager::run(const std::shared_ptr<grpc::ServerWriter<tactician::FrameData>>& writer) {
    running_ = true;

    // Lancement du Thread Capture
    producerThread_ = std::thread(&PipelineManager::producerLoop, this);

    // Le Thread principal devient le Consommateur (évite de créer un thread de plus)
    consumerLoop(writer);
}

void PipelineManager::producerLoop() {
    cv::Mat frame;
    while (running_) {
        if (video_.getFrame(frame)) {
            // Push une copie (clone nécessaire car OpenCV réutilise le buffer interne)
            // Mais on move le clone dans la queue
            queue_.push(frame.clone());
            
            // Simulation temps réel si lecture fichier (sinon ça va trop vite)
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        } else {
            // Fin de vidéo : boucle ou arrêt
            std::cout << "[Pipeline] Fin du flux vidéo." << std::endl;
            running_ = false; 
            queue_.request_stop();
        }
    }
}

void PipelineManager::consumerLoop(std::shared_ptr<grpc::ServerWriter<tactician::FrameData>> writer) {
    while (running_) {
        // 1. Récupération Frame (Bloquant)
        auto frameOpt = queue_.pop();
        if (!frameOpt.has_value()) {
            if (!running_) break; // Queue vide + stop demandé
            continue;
        }

        cv::Mat frame = std::move(*frameOpt);
        tactician::FrameData frameData;
        frameData.set_timestamp(time(nullptr));

        // 2. Inférence IA
        auto detections = detector_.detect(frame);

        // 3. Logique Métier
        for (const auto& det : detections) {
            // Check ROI valide
            cv::Rect roiBox = det.box & cv::Rect(0, 0, frame.cols, frame.rows);
            if (roiBox.area() <= 0) continue;

            cv::Mat roi = frame(roiBox);
            EntityType type = classifier_.predict(roi, det.class_id);
            
            // Transformation 3D -> 2D
            cv::Point2f mapPos = transformer_.transform(det.box);

            if (type == EntityType::Ball) {
                auto* ball = frameData.mutable_ball();
                ball->set_x(mapPos.x);
                ball->set_y(mapPos.y);
            } 
            else if (type == EntityType::TeamA || type == EntityType::TeamB) {
                auto* player = frameData.add_players();
                player->set_id(0); // TODO: Ajouter Tracking ID (SORT)
                player->set_x(mapPos.x);
                player->set_y(mapPos.y);
                if (type == EntityType::TeamA) {
                    player->set_team(tactician::Player::TEAM_A);
                } else {
                    player->set_team(tactician::Player::TEAM_B);
                }
            }
        }

        // 4. Envoi gRPC
        if (writer) {
            if (!writer->Write(frameData)) {
                std::cerr << "[Pipeline] Client déconnecté." << std::endl;
                running_ = false; // Arrêt propre si le client part
            }
        }
    }
}