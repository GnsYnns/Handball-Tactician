#include <iostream>
#include <thread>
#include <cmath>
#include <grpcpp/grpcpp.h>
#include "tactician.pb.h"
#include "tactician.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using tactician::StreamPositions;
using tactician::SessionRequest;
using tactician::FrameData;

// Logique du Service
class TacticianImpl final : public StreamPositions::Service {
    Status Subscribe(ServerContext* context, const SessionRequest* request,
                     grpc::ServerWriter<FrameData>* writer) override {
        std::cout << "Client connecté : " << request->client_id() << std::endl;
        
        FrameData frame;
        float t = 0.0f;

        // Boucle infinie de simulation (s'arrête si le client coupe)
        while (!context->IsCancelled()) {
            frame.set_timestamp(time(NULL));
            
            // Simuler une balle qui fait des cercles
            auto* ball = frame.mutable_ball();
            ball->set_x(0.5f + 0.3f * std::cos(t));
            ball->set_y(0.5f + 0.3f * std::sin(t));
            ball->set_speed(15.0f);

            // Envoyer la frame
            writer->Write(frame);
            
            // Pause 33ms (~30 FPS)
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
            t += 0.1f;
        }
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    TacticianImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}