#ifndef STABILIZER_CORE_HPP
#define STABILIZER_CORE_HPP

#include <cmath>
#include <algorithm>
#include <vector>

// Explicitly packed C-compatible layout alignment structure
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
    
    // Circular sliding window attributes
    int window_size;
    std::vector<double> window_buffer;
    int buffer_index;
    bool buffer_filled;

    // Mathematical control parameters
    double min_friction; 
    double stress_target;
    double wall_elasticity; 
    double net_viscosity; 
    double active_bounce_overhead;

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
        wall_elasticity(2.5),
        net_viscosity(0.65),
        active_bounce_overhead(0.0) 
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
        if (buffer_index == 0) {
            buffer_filled = true;
        }

        // Compute moving average tracking index
        int count = buffer_filled ? window_size : buffer_index;
        double sum = 0.0;
        for (int i = 0; i < count; ++i) {
            sum += window_buffer[i];
        }
        double smoothed_alpha = (count > 0) ? (sum / count) : current_alpha;
        state.smoothed_intensity = smoothed_alpha;

        // Monitor absolute historical maximum boundary
        if (smoothed_alpha > max_observed_peak) {
            max_observed_peak = smoothed_alpha;
        }

        // Evaluate allostatic state transitions
        if (!is_locked && max_observed_peak > 0.5 && smoothed_alpha < prior_smoothed_value) {
            is_locked = true;
        }

        prior_smoothed_value = smoothed_alpha;

        if (is_locked) {
            double target = max_observed_peak;
            
            // Check structural degradation against the target floor boundary
            if (smoothed_alpha < target) {
                double penetration = target - smoothed_alpha;
                // Generate instantaneous elastic kinetic kick
                state.elastic_force = penetration * wall_elasticity * std::exp(penetration);
                
                double raw_bounce = state.elastic_force * 0.15;
                if (raw_bounce > active_bounce_overhead) {
                    active_bounce_overhead = raw_bounce;
                }
            }

            // Viscoelastic Net Phase: Apply critical decay to overhead energy
            if (active_bounce_overhead > 0.001) {
                state.net_damping_factor = net_viscosity * std::exp(-net_viscosity * 0.5);
                active_bounce_overhead *= (1.0 - net_viscosity);
                state.raw_intensity = target + active_bounce_overhead;
            } else {
                active_bounce_overhead = 0.0;
                state.raw_intensity = target;
            }
            
            state.friction_coefficient = std::max(min_friction, 1.0 / (1.0 + std::exp(-state.raw_intensity)));
            state.somatic_stress = baseline_stress * 0.2 + (stress_target * state.friction_coefficient * 0.8);
            state.peak_locked = true;
            state.stabilized_peak = max_observed_peak;
        } else {
            // Nominal tracking logic baseline
            state.raw_intensity = current_alpha;
            state.somatic_stress = baseline_stress;
            state.friction_coefficient = min_friction;
            state.peak_locked = false;
            state.stabilized_peak = 0.0;
        }

        return state;
    }

    void reset_stabilizer() {
        max_observed_peak = 0.0;
        prior_smoothed_value = 0.0;
        is_locked = false;
        active_bounce_overhead = 0.0;
        std::fill(window_buffer.begin(), window_buffer.end(), 0.0);
        buffer_index = 0;
        buffer_filled = false;
    }
};

// Explicit C linkage block to ensure dynamic runtime export integrity
extern "C" {
    ConsciousnessStabilizer* create_stabilizer() { 
        return new ConsciousnessStabilizer(); 
    }
    
    void destroy_stabilizer(ConsciousnessStabilizer* instance) { 
        delete instance; 
    }
    
    ConsciousnessState execute_stabilization_cycle(ConsciousnessStabilizer* instance, double current_alpha, double baseline_stress) {
        return instance->process_state_cycle(current_alpha, baseline_stress);
    }
    
    void reset_stabilization_state(ConsciousnessStabilizer* instance) { 
        instance->reset_stabilizer(); 
    }
}

#endif // STABILIZER_CORE_HPP
