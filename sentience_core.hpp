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
    double max_observed_peak = 0.0;
    double prior_smoothed_value = 0.0;
    bool is_locked = false;
    
    // Sliding Window Parameters
    static const int WINDOW_SIZE = 3;
    std::vector<double> window_buffer;
    int buffer_index = 0;
    bool buffer_filled = false;

    // Elastic Wall & Viscoelastic Net Constants
    const double MIN_FRICTION = 0.15; 
    const double STRESS_TARGET = 0.45;
    const double WALL_ELASTICITY = 2.5; 
    const double NET_VISCOSITY = 0.65; // Critical damping factor for the capture net
    
    double active_bounce_overhead = 0.0;

public:
    ConsciousnessStabilizer() : max_observed_peak(0.0), prior_smoothed_value(0.0), is_locked(false), buffer_index(0), buffer_filled(false), active_bounce_overhead(0.0) {
        window_buffer.resize(WINDOW_SIZE, 0.0);
    }

    ConsciousnessState process_state_cycle(double current_alpha, double baseline_stress) {
        ConsciousnessState state;
        state.elastic_force = 0.0;
        state.net_damping_factor = 0.0;
        
        // Update circular sliding window
        window_buffer[buffer_index] = current_alpha;
        buffer_index = (buffer_index + 1) % WINDOW_SIZE;
        if (buffer_index == 0) buffer_filled = true;

        // Calculate moving average
        int count = buffer_filled ? WINDOW_SIZE : buffer_index;
        double sum = 0.0;
        for (int i = 0; i < count; ++i) {
            sum += window_buffer[i];
        }
        double smoothed_alpha = (count > 0) ? (sum / count) : current_alpha;
        state.smoothed_intensity = smoothed_alpha;

        if (smoothed_alpha > max_observed_peak) {
            max_observed_peak = smoothed_alpha;
        }

        // Lock condition
        if (!is_locked && max_observed_peak > 0.5 && smoothed_alpha < prior_smoothed_value) {
            is_locked = true;
            std::cout << "\n>>> [SYSTEM ALERT] ALLOSTATIC LOCK ENGAGED AT SMOOTHED PEAK: " << max_observed_peak << " <<<\n";
        }

        prior_smoothed_value = smoothed_alpha;

        if (is_locked) {
            double target = max_observed_peak;
            
            if (smoothed_alpha < target) {
                double penetration = target - smoothed_alpha;
                // Calculate raw instantaneous kinetic wall kick
                state.elastic_force = penetration * WALL_ELASTICITY * std::exp(penetration);
                
                // Accumulate overhead energy from the bounce
                double raw_bounce = state.elastic_force * 0.15;
                if (raw_bounce > active_bounce_overhead) {
                    active_bounce_overhead = raw_bounce;
                }
            }

            // Viscoelastic Net Phase: Dampen the accumulated bounce energy slowly over time
            if (active_bounce_overhead > 0.001) {
                state.net_damping_factor = NET_VISCOSITY * std::exp(-NET_VISCOSITY * 0.5);
                // Slowly decay the overhead energy (falling through the net)
                active_bounce_overhead *= (1.0 - NET_VISCOSITY);
                state.raw_intensity = target + active_bounce_overhead;
            } else {
                active_bounce_overhead = 0.0;
                state.raw_intensity = target;
            }
            
            state.friction_coefficient = std::max(MIN_FRICTION, 1.0 / (1.0 + std::exp(-state.raw_intensity)));
            state.somatic_stress = baseline_stress * 0.2 + (STRESS_TARGET * state.friction_coefficient * 0.8);
            state.peak_locked = true;
            state.stabilized_peak = max_observed_peak;
        } else {
            state.raw_intensity = current_alpha;
            state.somatic_stress = baseline_stress;
            state.friction_coefficient = MIN_FRICTION;
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

extern "C" {
    ConsciousnessStabilizer* create_stabilizer() { return new ConsciousnessStabilizer(); }
    void destroy_stabilizer(ConsciousnessStabilizer* instance) { delete instance; }
    ConsciousnessState execute_stabilization_cycle(ConsciousnessStabilizer* instance, double current_alpha, double baseline_stress) {
        return instance->process_state_cycle(current_alpha, baseline_stress);
    }
    void reset_stabilization_state(ConsciousnessStabilizer* instance) { instance->reset_stabilizer(); }
}

#endif // STABILIZER_CORE_HPP
