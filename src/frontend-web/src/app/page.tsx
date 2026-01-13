"use client";
import { useTacticianSocket } from '../hooks/useTacticianSocket';

export default function Home() {
    const { ballPos, connected } = useTacticianSocket();

    // Couleurs basées sur ton image
    const courtColor = "bg-[#348ac7]"; // Bleu du terrain
    const zoneColor = "bg-[#d99046]";  // Orange des zones
    const lineColor = "border-white";  // Couleur des lignes

    return (
        <div className="flex flex-col items-center justify-center min-h-screen bg-gray-900 text-white p-10">
            <h1 className="text-2xl font-bold mb-8">
                Handball Tactician {connected ? "🟢 Online" : "🔴 Offline"}
            </h1>

            {/* WRAPPER DU TERRAIN (Pour que les buts puissent dépasser sur les côtés) */}
            <div className="relative">

                {/* But Gauche (Extérieur) */}
                <div className="absolute top-1/2 -left-6 -translate-y-1/2 w-6 h-[80px] border-2 border-white bg-white/10 z-0 flex flex-col justify-around">
                    <div className="w-full h-px bg-white/30"></div>
                    <div className="w-full h-px bg-white/30"></div>
                    <div className="w-full h-px bg-white/30"></div>
                </div>

                {/* But Droit (Extérieur) */}
                <div className="absolute top-1/2 -right-6 -translate-y-1/2 w-6 h-[80px] border-2 border-white bg-white/10 z-0 flex flex-col justify-around">
                    <div className="w-full h-px bg-white/30"></div>
                    <div className="w-full h-px bg-white/30"></div>
                    <div className="w-full h-px bg-white/30"></div>
                </div>

                {/* SURFACE DE JEU (800x400) */}
                <div className={`relative w-[800px] h-[400px] ${courtColor} border-2 ${lineColor} shadow-2xl overflow-hidden z-10`}>

                    {/* --- LIGNE MÉDIANE --- */}
                    <div className={`absolute left-1/2 top-0 bottom-0 w-0.5 ${courtColor} border-l-2 ${lineColor}`}></div>
                    <div className="absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2 w-2 h-2 rounded-full bg-white"></div>

                    {/* --- CÔTÉ GAUCHE --- */}

                    {/* Ligne des 9m (Pointillés) - Rayon approx 9m */}
                    <div className={`absolute top-1/2 left-0 -translate-y-1/2 w-[22.5%] h-[90%] border-r-2 border-dashed ${lineColor} rounded-r-full pointer-events-none`}></div>

                    {/* Zone des 6m (Orange) - Rayon approx 6m */}
                    <div className={`absolute top-1/2 left-0 -translate-y-1/2 w-[15%] h-[60%] ${zoneColor} border-r-2 ${lineColor} rounded-r-full z-0`}></div>

                    {/* Marque des 7m (Penalty) - Verticale : w-0.5 h-4 */}
                    <div className="absolute top-1/2 left-[17.5%] -translate-y-1/2 w-0.5 h-4 bg-white"></div>

                    {/* Marque des 4m (Gardien) - Verticale : w-0.5 h-4 */}
                    <div className="absolute top-1/2 left-[10%] -translate-y-1/2 w-0.5 h-4 bg-white"></div>


                    {/* --- CÔTÉ DROIT --- */}

                    {/* Ligne des 9m (Pointillés) */}
                    <div className={`absolute top-1/2 right-0 -translate-y-1/2 w-[22.5%] h-[90%] border-l-2 border-dashed ${lineColor} rounded-l-full pointer-events-none`}></div>

                    {/* Zone des 6m (Orange) */}
                    <div className={`absolute top-1/2 right-0 -translate-y-1/2 w-[15%] h-[60%] ${zoneColor} border-l-2 ${lineColor} rounded-l-full z-0`}></div>

                    {/* Marque des 7m (Penalty) */}
                    <div className="absolute top-1/2 right-[17.5%] -translate-y-1/2 w-0.5 h-4 bg-white"></div>

                    {/* Marque des 4m (Gardien) */}
                    <div className="absolute top-1/2 right-[10%] -translate-y-1/2 w-0.5 h-4 bg-white"></div>


                    {/* --- LE BALLON --- */}
                    <div
                        className="absolute w-5 h-5 bg-yellow-400 rounded-full shadow-[0_0_10px_rgba(0,0,0,0.5)] border border-black z-50 transition-all duration-75 ease-linear"
                        style={{
                            left: `${ballPos.x * 100}%`,
                            top: `${ballPos.y * 100}%`,
                            transform: 'translate(-50%, -50%)'
                        }}
                    />
                </div>
            </div>

            <div className="mt-6 font-mono bg-gray-800 p-2 rounded text-sm text-gray-300">
                DATA: X={ballPos.x.toFixed(3)} Y={ballPos.y.toFixed(3)}
            </div>
        </div>
    );
}