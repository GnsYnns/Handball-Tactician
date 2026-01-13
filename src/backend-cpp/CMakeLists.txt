cmake_minimum_required(VERSION 3.20)
project(HandballTactician CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. Dépendances ---
find_package(OpenCV REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# --- 2. Génération Automatique gRPC/Protobuf ---
set(PROTO_SRC_DIR "${CMAKE_SOURCE_DIR}/../../proto")
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")

# On s'assure que le dossier de destination existe
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Recherche propre du plugin gRPC
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

if(NOT GRPC_CPP_PLUGIN)
    message(FATAL_ERROR "grpc_cpp_plugin introuvable ! As-tu bien installé 'protobuf-compiler-grpc' ?")
else()
    message(STATUS "Plugin gRPC trouvé : ${GRPC_CPP_PLUGIN}")
endif()

# COMMANDE DE GÉNÉRATION (Une seule fois !)
add_custom_command(
        OUTPUT "${GENERATED_DIR}/tactician.pb.cc"
        "${GENERATED_DIR}/tactician.pb.h"
        "${GENERATED_DIR}/tactician.grpc.pb.cc"
        "${GENERATED_DIR}/tactician.grpc.pb.h"
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS --grpc_out=${GENERATED_DIR}
        --cpp_out=${GENERATED_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        -I ${PROTO_SRC_DIR}
        ${PROTO_SRC_DIR}/tactician.proto
        DEPENDS ${PROTO_SRC_DIR}/tactician.proto
        COMMENT "Génération du code gRPC/Protobuf..."
)

# --- 3. Déclaration de l'exécutable ---
add_executable(handball_engine
        main_debug.cpp
        ${GENERATED_DIR}/tactician.pb.cc
        ${GENERATED_DIR}/tactician.grpc.pb.cc
        src/io/VideoSource.cpp
        src/io/VideoSource.hpp
        src/pipeline/SafeQueue.cpp
        src/pipeline/SafeQueue.hpp
        src/vision/YoloDetector.cpp
        src/vision/YoloDetector.hpp
        src/core/DomainTypes.hpp
        src/vision/TeamClassifier.cpp
        src/vision/TeamClassifier.hpp
        src/vision/Geometry.cpp
        src/vision/Geometry.hpp
        src/pipeline/PipelineManager.cpp
        src/pipeline/PipelineManager.hpp
        main_debug.cpp
)

# --- 4. Includes et Linking ---
target_include_directories(handball_engine PRIVATE
        src
        ${GENERATED_DIR}
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(handball_engine PRIVATE
        ${OpenCV_LIBS}
        gRPC::grpc++
        protobuf::libprotobuf
)