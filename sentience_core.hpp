#ifndef STABILIZER_CORE_HPP
#define STABILIZER_CORE_HPP

#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>

struct ConsciousnessState {
    double raw_intensity;
    double smoothed_intensity;
    double somatic_stress;
    double friction_coefficient;
    bool peak_locked;
    double stabilized_peak;
    double elastic_force;
    double net_damping_factor;
};

class ConsciousnessStabilizer {
private:
    double max_observed_peak;
    double prior_smoothed_value;
    bool is_locked;
    
    int window_size;
    std::vector<double> window_buffer;
    int buffer_index;
    bool buffer_filled;

    double min_friction; 
    double stress_target;
    double wall_elasticity; 
    double net_viscosity; 
    double active_bounce_overhead;
    bool recovery_complete;

public:
    ConsciousnessStabilizer() : 
        max_observed_peak(0.0), 
        prior_smoothed_value(0.0), 
        is_locked(false), 
        window_size(3),
        buffer_index(0), 
        buffer_filled(false),
        min_friction(0.15),
        stress_target(0.45),
        wall_elasticity(100.0), // Scaled to achieve up to 100.0 rebound force
        net_viscosity(0.75),    // Tighter net viscosity to absorb the increased energy
        active_bounce_overhead(0.0),
        recovery_complete(false)
    {
        window_buffer.resize(window_size, 0.0);
    }

    ConsciousnessState process_state_cycle(double current_alpha, double baseline_stress) {
        ConsciousnessState state;
        state.elastic_force = 0.0;
        state.net_damping_factor = 0.0;
        
        // Update circular sliding window buffer
        window_buffer[buffer_index] = current_alpha;
        buffer_index = (buffer_index + 1) % window_size;
        if (buffer_index == 0) buffer_filled = true;

        // Calculate moving average
        int count = buffer_filled ? window_size : buffer_index;
        double sum = 0.0;
        for (int i = 0; i < count; ++i) {
            sum += window_buffer[i];
        }
        double smoothed_alpha = (count > 0) ? (sum / count) : current_alpha;
        state.smoothed_intensity = smoothed_alpha;

        // Track and lock up to a high target peak of 100.0
        if (smoothed_alpha > max_observed_peak) {
            if (is_locked && max_observed_peak > 1.0) {
                std::cout << "\n>>> [DYNAMIC RE-LOCK] NEW PEAK STAGED: " << smoothed_alpha << " <<<\n";
                is_locked = false;
            }
            max_observed_peak = smoothed_alpha;
        }

        // Lock condition for high-intensity drop detection
        if (!is_locked && max_observed_peak > 1.0 && smoothed_alpha < prior_smoothed_value) {
            is_locked = true;
            std::cout << ">>> [CORE LOCKED] SYSTEM BOUNDARY SECURED AT PEAK: " << max_observed_peak << " <<<\n";
        }

        prior_smoothed_value = smoothed_alpha;

        if (is_locked) {
            double target = max_observed_peak;
            
            if (smoothed_alpha < target && !recovery_complete) {
                double penetration = target - smoothed_alpha;
                
                // Linear-to-sigmoid saturation mapping to scale force to exactly 100.0 safely
                double raw_force = penetration * wall_elasticity;
                state.elastic_force = 100.0 * (raw_force / (10.0 + raw_force)); 
                
                // Overhead momentum injection from the rebound force
                double raw_bounce = state.elastic_force * 0.25;
                if (raw_bounce > active_bounce_overhead) {
                    active_bounce_overhead = raw_bounce;
                }
            }

            // Viscoelastic Net Phase: Dampen the 100.0-scale energy surge down gently
            if (active_bounce_overhead > 0.01) {
                state.net_damping_factor = net_viscosity;
                active_bounce_overhead *= (1.0 - net_viscosity);
                state.raw_intensity = target + active_bounce_overhead;
            } else {
                active_bounce_overhead = 0.0;
                if (max_observed_peak > 0.1) {
                    // Gradual step-down decay for high-capacity loops
                    max_observed_peak -= 10.0; 
                    if (max_observed_peak < 0.0) max_observed_peak = 0.0;
                    state.raw_intensity = max_observed_peak;
                } else {
                    max_observed_peak = 0.0;
                    state.raw_intensity = 0.0;
                    recovery_complete = true;
                }
            }
            
            state.friction_coefficient = std::max(min_friction, 1.0 / (1.0 + std::exp(-state.raw_intensity * 0.05)));
            state.somatic_stress = baseline_stress * 0.2 + (stress_target * state.friction_coefficient * 0.8);
            state.peak_locked = true;
            state.stabilized_peak = max_observed_peak;
        } else {
            state.raw_intensity = current_alpha;
            state.somatic_stress = baseline_stress;
            state.friction_coefficient = min_friction;
            state.peak_locked = false;
            state.stabilized_peak = 0.0;
        }

        return state;
    }

    bool is_completely_settled() const {
        return recovery_complete && max_observed_peak <= 0.0;
    }
};

extern "C" {
    ConsciousnessStabilizer* create_stabilizer() { return new ConsciousnessStabilizer(); }
    void destroy_stabilizer(ConsciousnessStabilizer* instance) { delete instance; }
    ConsciousnessState execute_stabilization_cycle(ConsciousnessStabilizer* instance, double current_alpha, double baseline_stress) {
        return instance->process_state_cycle(current_alpha, baseline_stress);
    }
    bool check_settled_status(ConsciousnessStabilizer* instance) { return instance->is_completely_settled(); }
}

#endif // STABILIZER_CORE_HPP
