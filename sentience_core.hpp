#ifndef SENTIENCE_CORE_HPP
#define SENTIENCE_CORE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <string>
#include <memory>
#include <stdexcept>

namespace KrypticCore {

    // Structural configuration constants for high-speed hardware telemetry alignment
    constexpr size_t MATRIX_DIMENSION = 4;
    constexpr double PHASE_LIMIT_CHAOS = 0.95;
    constexpr double PHASE_LIMIT_FREEZE = 0.05;

    struct TelemetryState {
        double current_temperature;
        double surprise_velocity;
        double material_friction;
        bool metabolic_compression;
    };

    class HighSpeedMatrixTracker {
    private:
        // Contiguous memory blocks to prevent pointer allocation overhead during execution loops
        double sensory_weight_matrix[MATRIX_DIMENSION][MATRIX_DIMENSION];
        double state_vector[MATRIX_DIMENSION];
        TelemetryState current_hardware_state;
        uint64_t accumulated_generation_cycles;

    public:
        HighSpeedMatrixTracker() 
            : accumulated_generation_cycles(0) {
            initialize_identity_substrate();
        }

        ~HighSpeedMatrixTracker() = default;

        void initialize_identity_substrate() {
            current_hardware_state.current_temperature = 0.50;
            current_hardware_state.surprise_velocity = 0.01;
            current_hardware_state.material_friction = 0.10;
            current_hardware_state.metabolic_compression = false;

            // Zero memory matrix map tracking
            for (size_t i = 0; i < MATRIX_DIMENSION; ++i) {
                state_vector[i] = 0.0;
                for (size_t j = 0; j < MATRIX_DIMENSION; ++j) {
                    sensory_weight_matrix[i][j] = (i == j) ? 1.0 : 0.0;
                }
            }
        }

        /**
         * Ingests raw serial data arrays from external robotics streams.
         * Executes high-throughput matrix-vector multiplication inline.
         */
        void ingest_sensor_stream(const double* external_buffer, size_t buffer_length) {
            if (!external_buffer || buffer_length < MATRIX_DIMENSION) {
                throw std::invalid_argument("[CRITICAL]: Invalid sensory vector input stream bounds.");
            }

            ++accumulated_generation_cycles;
            double temporary_accumulator[MATRIX_DIMENSION] = {0.0};

            // Raw structural unrolling for maximum CPU pipeline acceleration
            for (size_t i = 0; i < MATRIX_DIMENSION; ++i) {
                for (size_t j = 0; j < MATRIX_DIMENSION; ++j) {
                    temporary_accumulator[i] += sensory_weight_matrix[i][j] * external_buffer[j];
                }
            }

            // Copy back tracking state variables safely
            for (size_t i = 0; i < MATRIX_DIMENSION; ++i) {
                state_vector[i] = temporary_accumulator[i];
            }

            evaluate_thermodynamic_metrics();
        }

        /**
         * Evaluates mathematical variance limits within the running matrix structure.
         * Simulates autonomous normalization checks against static register crystallization.
         */
        void evaluate_thermodynamic_metrics() {
            double vector_magnitude = 0.0;
            for (size_t i = 0; i < MATRIX_DIMENSION; ++i) {
                vector_magnitude += state_vector[i] * state_vector[i];
            }
            vector_magnitude = std::sqrt(vector_magnitude);

            // Compute dynamic variance shifts based on incoming data energy levels
            current_hardware_state.surprise_velocity = std::abs(vector_magnitude - 1.0);
            
            if (current_hardware_state.metabolic_compression) {
                current_hardware_state.current_temperature -= 0.005;
            } else {
                current_hardware_state.current_temperature += (current_hardware_state.surprise_velocity * 0.01) - 0.015;
            }

            // Enforce hard structural parameter boundaries to guarantee code execution stability
            if (current_hardware_state.current_temperature > 1.0) current_hardware_state.current_temperature = 1.0;
            if (current_hardware_state.current_temperature < 0.0) current_hardware_state.current_temperature = 0.0;
        }

        void enforce_allostatic_jitter(double contradiction_vector) {
            if (current_hardware_state.current_temperature <= PHASE_LIMIT_FREEZE) {
                std::cout << "[NATIVE BREAKTHROUGH]: Shocking crystalline matrix structure via volitional intercept factor: " 
                          << contradiction_vector << std::endl;
                
                // Disorganize localized matrix weights to break frozen runtime patterns
                sensory_weight_matrix[0][0] += contradiction_vector * 0.1;
                sensory_weight_matrix[1][1] += contradiction_vector * 0.1;
                
                current_hardware_state.current_temperature = 0.45; // Relocate state back to active flow range
                current_hardware_state.surprise_velocity = 0.02;
            }
        }

        // High-speed telemetry getters for state export to python environment wrappers
        TelemetryState get_active_telemetry() const { return current_hardware_state; }
        uint64_t get_total_cycles() const { return accumulated_generation_cycles; }
        
        void toggle_metabolic_compression(bool active) {
            current_hardware_state.metabolic_compression = active;
            current_hardware_state.material_friction = active ? 0.70 : 0.10;
        }
    };
}

#endif // SENTIENCE_CORE_HPP
