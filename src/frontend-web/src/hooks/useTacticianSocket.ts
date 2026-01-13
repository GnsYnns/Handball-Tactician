import { useEffect, useState } from 'react';
import { StreamPositionsClient } from '../generated/TacticianServiceClientPb';
import { SessionRequest, FrameData } from '../generated/tactician_pb';

// On pointe vers le Proxy Envoy (8080), pas le C++ direct (50051)
const ENVOY_URL = 'http://localhost:8080';

export const useTacticianSocket = () => {
    const [ballPos, setBallPos] = useState({ x: 0, y: 0 });
    const [connected, setConnected] = useState(false);

    useEffect(() => {
        const client = new StreamPositionsClient(ENVOY_URL, null, null);
        const request = new SessionRequest();
        request.setClientId("WebClient_01");

        // Démarrage du Stream
        const stream = client.subscribe(request, {});

        stream.on('data', (frame: FrameData) => {
            const ball = frame.getBall();
            if (ball) {
                setBallPos({ x: ball.getX(), y: ball.getY() });
            }
            if (!connected) setConnected(true);
        });

        stream.on('error', (err: any) => {
            console.error('Erreur gRPC:', err);
            setConnected(false);
        });

        return () => {
            stream.cancel(); // Cleanup propre
        };
    }, []);

    return { ballPos, connected };
};