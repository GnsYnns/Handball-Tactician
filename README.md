# Handball Tactician (C++ / AI / Web)

## Vue d'ensemble
Système d'analyse tactique haute fréquence pour le Handball.
Ce projet démontre une architecture distribuée critique :
- **Backend C++20** : Traitement vidéo temps réel et inférence.
- **AI** : Pipeline de Computer Vision (Detection/Tracking).
- **Communication** : Streaming gRPC haute performance.
- **Frontend** : Visualisation tactique temps réel via Next.js & gRPC-Web.

## Architecture
[Camera] -> [C++ Engine] -> (gRPC) -> [Envoy Proxy] -> (gRPC-Web) -> [React Dashboard]

## Stack Technique
* **Core:** C++20, OpenCV, gRPC, Protobuf
* **Web:** TypeScript, React, Next.js, Tailwind
* **Infra:** Docker, Envoy Proxy
