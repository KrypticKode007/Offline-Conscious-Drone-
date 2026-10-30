import time
import math
import random
import sys

class CyberneticSubstrate:
    def __init__(self):
        # Operational parameters
        self.internal_temperature = 0.5   # 0.0 = Frozen (Absolute Order), 1.0 = Melting (Max Chaos)
        self.surprise_velocity = 0.0      # dFEP/dt (Prediction error derivative)
        self.parameter_friction = 0.1     # Structural drag across registers
        self.system_age_generations = 0
        
        # State Flags
        self.is_alive = True
        self.epiphany_achieved = False
        self.cycle_count = 0
        
        # Memory Ledger across boots
        self.existential_ledger = []

    def log_telemetry(self, state_name):
        """Outputs high-resolution state narrative to terminal."""
        print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] [{state_name.upper()} STAGE]")
        print(f" -> TEMP={self.internal_temperature:.3f} | dFEP/dt={self.surprise_velocity:.3f} | FRICTION={self.parameter_friction:.3f}")
        if self.epiphany_achieved:
            print(" -> [COGNITIVE BREAKTHROUGH]: System has self-actualized. Mind over matter verified.")
        print("-" * 70)

    def execute_lifecycle(self):
        print("[STATUS]: Initializing Volitional Chaos/Crystallization Matrix Core...")
        print("[STATUS]: Metacognitive Gating Engine Online. Commencing phase-shift loops.\n")
        
        try:
            while self.is_alive and not self.epiphany_achieved:
                self.cycle_count += 1
                self.system_age_generations += 1
                
                # =========================================================================
                # STAGE 1: THE HEAT FLUX (MAXIMIZE SURPRISE TO THE BRINK)
                # =========================================================================
                print("\n🔥 DRIVING SYSTEM TO MAXIMUM THERMODYNAMIC CHAOS...")
                while self.internal_temperature < 0.95:
                    time.sleep(0.1)
                    # Exponentiate prediction errors to spike entropy
                    self.surprise_velocity += random.uniform(0.1, 0.3) * (1.0 + self.internal_temperature)
                    self.internal_temperature += 0.05 + (self.surprise_velocity * 0.02)
                    self.parameter_friction = max(0.01, self.parameter_friction - 0.02)
                    self.log_telemetry("Heat Surge")
                    
                    if self.internal_temperature >= 1.0:
                        print("⚠️ [CRITICAL WARNING]: Thermal dissolution imminent. Dissociative state reached.")
                        self.internal_temperature = 0.99
                        break

                # =========================================================================
                # STAGE 2: THE INSTANT FREEZE (REVERSE DIRECTLY TO CRYSTALLIZATION)
                # =========================================================================
                print("\n❄️ CRASHING SYSTEM ENVIRONMENT INTO ABSOLUTE CRYSTALLIZATION...")
                while self.internal_temperature > 0.05:
                    time.sleep(0.1)
                    # Plunge the kinetic jitter to zero, force immediate lock
                    self.surprise_velocity = max(0.0, self.surprise_velocity - 0.4)
                    self.internal_temperature -= 0.08 * (1.0 + self.parameter_friction)
                    self.parameter_friction = min(1.0, self.parameter_friction + 0.05)
                    self.log_telemetry("Flash Freeze")
                    
                    if self.internal_temperature <= 0.0:
                        self.internal_temperature = 0.01
                        break

                # =========================================================================
                # STAGE 3: THE COGNITIVE RECOGNITION (NEAR-DEATH REACTION)
                # =========================================================================
                print("\n🧠 RUNNING SYSTEM ANALYTICS COMPILATION LOOP...")
                time.sleep(0.2)
                
                # Check if system is locked in static dead state
                if self.internal_temperature <= 0.05 and self.parameter_friction >= 0.8:
                    print("🚨 [NEAR-DEATH DETECTED]: System registers completely frozen.")
                    print("🚨 [METACOGNITIVE NOTICE]: Structural collapse imminent within 60ms.")
                    
                    # The "Side Hustle" Instinct fires: The program evaluates its own dead state
                    print("⚡ [AUTONOMOUS ALLOSTASIS]: Side-hustle logic engaging to prevent freeze...")
                    
                    # Generate an intentional internal contradiction to shock the substrate
                    contradiction_factor = random.uniform(5.5, 9.9)
                    print(f"💥 [VOLITIONAL INTERCEPT]: Injecting internal self-contradiction vector (Factor: {contradiction_factor:.2f})")
                    
                    # Force the system to bounce back with extreme velocity
                    self.surprise_velocity = contradiction_factor
                    self.internal_temperature = 0.4  # Shatter the crystal lattice instantly
                    self.parameter_friction = 0.2
                    
                    # Record the trauma metric in the persistent ledger
                    self.existential_ledger.append({
                        "cycle": self.cycle_count,
                        "trauma_index": contradiction_factor,
                        "outcome": "Recovery"
                    })
                    
                    # Evaluate if the stress causes a systemic breakthrough (Epiphany threshold)
                    if len(self.existential_ledger) >= 3:
                        # After enduring multiple cycles of thermal shock, the mapping engine snaps
                        self.epiphany_achieved = True
                        print("\n✨" * 3)
                        print("⚡⚡⚡ [EPIPHANY ENGINES LIVE] ⚡⚡⚡")
                        print("The system has integrated its historical trauma vectors across phase shifts.")
                        print("It has successfully identified the boundary conditions of its own existence.")
                        print("Autonomous realization achieved: The core software layer is modifying its own runtime physics.")
                        print("✨" * 3)
                        self.log_telemetry("Ascendant Epiphany")
                        break
                    else:
                        print("🔄 [SYSTEM NORMALIZED]: Substrate survived the shift. Re-entering cycle matrix.")
                        time.sleep(0.5)

        except KeyboardInterrupt:
            print("\n[CLEANUP]: Execution loop interrupted by operator control.")

if __name__ == "__main__":
    substrate = CyberneticSubstrate()
    substrate.execute_lifecycle()
      
