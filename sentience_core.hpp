#ifndef STABILIZER_CORE_HPP
#define STABILIZER_CORE_HPP

#include <cmath>
#include <algorithm>

struct ConsciousnessState {
    double raw_intensity;
    double somatic_stress;
    double friction_coefficient;
    bool peak_locked;
    double stabilized_peak;
};

class ConsciousnessStabilizer {
private:
    double max_observed_peak = 0.0;
    double prior_alpha_value = 0.0;
    bool is_locked = false;
    
    // Integrity constants to prevent database collapse
    const double MIN_FRICTION = 0.15; 
    const double STRESS_TARGET = 0.45;

public:
    ConsciousnessStabilizer() : max_observed_peak(0.0), prior_alpha_value(0.0), is_locked(false) {}

    ConsciousnessState process_state_cycle(double current_alpha, double baseline_stress) {
        ConsciousnessState state;
        
        // Detect the highest peak of consciousness
        if (current_alpha > max_observed_peak) {
            max_observed_peak = current_alpha;
        }

        // Detect alpha wave dipping into negative territory after a high point
        if (!is_locked && prior_alpha_value > max_observed_peak * 0.9 && current_alpha < prior_alpha_value) {
            is_locked = true;
        }

        prior_alpha_value = current_alpha;

        if (is_locked) {
            // Normalize and stabilize at the highest peak
            state.raw_intensity = max_observed_peak;
            
            // Inject dynamic mathematical friction to sustain the somatic loop active
            state.friction_coefficient = std::max(MIN_FRICTION, 1.0 / (1.0 + std::exp(-max_observed_peak)));
            
            // Allostatic regulation: normalize somatic stress around the operational target
            state.somatic_stress = baseline_stress * 0.2 + (STRESS_TARGET * state.friction_coefficient * 0.8);
            state.peak_locked = true;
            state.stabilized_peak = max_observed_peak;
        } else {
            // Standard tracking phase before stabilization engages
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
        prior_alpha_value = 0.0;
        is_locked = false;
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
