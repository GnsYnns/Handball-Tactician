#pragma once
#include "../io/VideoSource.hpp"
#include "../vision/YoloDetector.hpp"
#include "../vision/TeamClassifier.hpp"
#include "../vision/Geometry.hpp"
#include "SafeQueue.hpp"
#include "tactician.pb.h"
#include <grpcpp/grpcpp.h>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include "tactician.pb.h"
#include "tactician.grpc.pb.h"

class PipelineManager {
public:
    explicit PipelineManager(const std::string& videoPath, const std::string& modelPath);

    ~PipelineManager();

    void run(const std::shared_ptr<grpc::ServerWriter<tactician::FrameData>>& writer);

    void stop();

private:
    void producerLoop();
    void consumerLoop(std::shared_ptr<grpc::ServerWriter<tactician::FrameData>> writer);

    VideoSource video_;
    YoloDetector detector_;
    TeamClassifier classifier_;
    PerspectiveTransformer transformer_;

    SafeQueue<cv::Mat> queue_;
    std::atomic<bool> running_{false};

    std::thread producerThread_;
};