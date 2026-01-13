#include "PipelineManager.hpp"
#include <iostream>

PipelineManager::PipelineManager(const std::string& videoPath, const std::string& modelPath)
    : video_(videoPath), detector_(modelPath) 
{
    // Initialize a temporary identity matrix to avoid crashes before calibration
    transformer_.setHomography(cv::Mat::eye(3, 3, CV_64F));
}

PipelineManager::~PipelineManager() {
    stop();
}

void PipelineManager::stop() {
    running_ = false;
    queue_.request_stop(); // Wake up the consumer so it can exit the wait
    if (producerThread_.joinable()) {
        producerThread_.join();
    }
}

void PipelineManager::run(const std::shared_ptr<grpc::ServerWriter<tactician::FrameData>>& writer) {
    running_ = true;

    // Start capture thread
    producerThread_ = std::thread(&PipelineManager::producerLoop, this);

    // The main thread becomes the consumer (avoids spawning an extra thread)
    consumerLoop(writer);
}

void PipelineManager::producerLoop() {
    cv::Mat frame;
    while (running_) {
        if (video_.getFrame(frame)) {
            // Push a copy (clone is required because OpenCV reuses the internal buffer)
            // Then move the clone into the queue
            queue_.push(frame.clone());
            
            // Real-time simulation when reading from a file (otherwise it runs too fast)
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        } else {
            // End of video: loop or stop
            std::cout << "[Pipeline] End of video stream." << std::endl;
            running_ = false; 
            queue_.request_stop();
        }
    }
}

void PipelineManager::consumerLoop(std::shared_ptr<grpc::ServerWriter<tactician::FrameData>> writer) {
    while (running_) {
        // 1. Retrieve frame (blocking)
        auto frameOpt = queue_.pop();
        if (!frameOpt.has_value()) {
            if (!running_) break; // Empty queue + stop requested
            continue;
        }

        cv::Mat frame = std::move(*frameOpt);
        tactician::FrameData frameData;
        frameData.set_timestamp(time(nullptr));

        // 2. AI inference
        auto detections = detector_.detect(frame);

        // 3. Business logic
        for (const auto& det : detections) {
            // Check valid ROI
            cv::Rect roiBox = det.box & cv::Rect(0, 0, frame.cols, frame.rows);
            if (roiBox.area() <= 0) continue;

            cv::Mat roi = frame(roiBox);
            EntityType type = classifier_.predict(roi, det.class_id);
            
            // 3D -> 2D transform
            cv::Point2f mapPos = transformer_.transform(det.box);

            if (type == EntityType::Ball) {
                auto* ball = frameData.mutable_ball();
                ball->set_x(mapPos.x);
                ball->set_y(mapPos.y);
            } 
            else if (type == EntityType::TeamA || type == EntityType::TeamB) {
                auto* player = frameData.add_players();
                player->set_id(0); // TODO: Add Tracking ID (SORT)
                player->set_x(mapPos.x);
                player->set_y(mapPos.y);
                if (type == EntityType::TeamA) {
                    player->set_team(tactician::Player::TEAM_A);
                } else {
                    player->set_team(tactician::Player::TEAM_B);
                }
            }
        }

        // 4. gRPC send
        if (writer) {
            if (!writer->Write(frameData)) {
                std::cerr << "[Pipeline] Client disconnected." << std::endl;
                running_ = false; // Clean shutdown if the client disconnects
            }
        }
    }
}