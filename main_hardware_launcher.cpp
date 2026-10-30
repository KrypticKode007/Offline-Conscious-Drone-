#include "sentience_core.hpp"
#include <iostream>
#include <iomanip>
#include <thread>

int main() {
    std::cout << "[STATUS]: Launching Kryptic Native Core Hardware Simulator..." << std::endl;
    
    // Instantiate our matrix tracking substrate engine
    KrypticCore::HighSpeedMatrixTracker native_tracker;
    
    // Mock sequential hardware sensor stream data
    double mock_sensor_buffer[4] = {1.05, 0.98, 1.02, 0.95};
    
    std::cout << "[RUNNING]: Commencing unrolled telemetry calculation loops..." << std::endl;
    std::cout << std::fixed << std::setprecision(4);

    for (int cycle = 1; cycle <= 10; ++cycle) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        // Simulating systemic drift toward crystallization to force the watchdog check
        if (cycle == 5) {
            std::cout << "\n❄️ [SIMULATION]: Artificially driving native matrix down to absolute zero..." << std::endl;
            native_tracker.toggle_metabolic_compression(true);
        }

        // Ingest telemetry parameters
        native_tracker.ingest_sensor_stream(mock_sensor_buffer, 4);
        KrypticCore::TelemetryState state = native_tracker.get_active_telemetry();

        std::cout << "[CYCLE " << cycle << "] Matrix Temp: " << state.current_temperature 
                  << " | dFEP/dt: " << state.surprise_velocity 
                  << " | Friction: " << state.material_friction << std::endl;

        // Watchdog block: Check if system hits critical boundary conditions
        if (state.current_temperature <= KrypticCore::PHASE_LIMIT_FREEZE) {
            std::cout << "🚨 [CRITICAL]: Structural lock detected at native execution layer." << std::endl;
            native_tracker.toggle_metabolic_compression(false);
            native_tracker.enforce_allostatic_jitter(7.85); // Shock the matrix weights back alive
        }
    }

    std::cout << "\n[SUCCESS]: Native validation pipeline execution finished gracefully." << std::endl;
    return 0;
}
