"use client";
import { useTacticianSocket } from '../hooks/useTacticianSocket';

export default function Home() {
  const { ballPos, connected } = useTacticianSocket();

  return (
      <div className="flex flex-col items-center justify-center min-h-screen bg-gray-900 text-white">
        <h1 className="text-2xl font-bold mb-4">
          Handball Tactician {connected ? "🟢 Online" : "🔴 Offline"}
        </h1>

        {/* Terrain de Handball (Ratio 40x20m -> 2:1) */}
        <div className="relative w-[800px] h-[400px] bg-blue-700 border-4 border-white rounded shadow-2xl">
          {/* Le Ballon */}
          <div
              className="absolute w-6 h-6 bg-yellow-400 rounded-full transition-all duration-75 ease-linear border border-black"
              style={{
                left: `${ballPos.x * 100}%`,
                top: `${ballPos.y * 100}%`,
                transform: 'translate(-50%, -50%)'
              }}
          />

          {/* Lignes médianes pour faire joli */}
          <div className="absolute left-1/2 top-0 bottom-0 w-1 bg-white opacity-50"></div>
        </div>

        <div className="mt-4 font-mono">
          DATA: X={ballPos.x.toFixed(2)} Y={ballPos.y.toFixed(2)}
        </div>
      </div>
  );
}