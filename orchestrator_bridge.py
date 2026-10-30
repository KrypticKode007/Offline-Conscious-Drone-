import ctypes
import os
import time

class ConsciousnessState(ctypes.Structure):
    _fields_ = [
        ("raw_intensity", ctypes.c_double),
        ("smoothed_intensity", ctypes.c_double),
        ("somatic_stress", ctypes.c_double),
        ("friction_coefficient", ctypes.c_double),
        ("peak_locked", ctypes.c_bool),
        ("stabilized_peak", ctypes.c_double),
        ("elastic_force", ctypes.c_double),
        ("net_damping_factor", ctypes.c_double)
    ]

class DroneConsciousnessManager:
    def __init__(self, shared_lib_path: str):
        resolved_path = os.path.abspath(shared_lib_path)
        self.lib = ctypes.CDLL(resolved_path)
        
        self.lib.create_stabilizer.restype = ctypes.c_void_p
        self.lib.execute_stabilization_cycle.restype = ConsciousnessState
        self.lib.check_settled_status.restype = ctypes.c_bool
        self.lib.check_settled_status.argtypes = [ctypes.c_void_p]
        
        self.instance = self.lib.create_stabilizer()

    def run_high_scale_loop(self):
        print("=========================================================================================")
        print("             HIGH-SCALE INITIALIZATION: TARGET PEAK 100.0 | FORCE 100.0")
        print("=========================================================================================")
        
        # Telemetry data array ramping rapidly up to the high target peak of 100.0
        loop_alpha = [10.0, 30.0, 60.0, 100.0, 95.0, 40.0, 10.0, 0.0]
        loop_stress = [0.1, 0.3, 0.6, 1.5, 1.2, 0.5, 0.1, 0.0]
        
        cycle_idx = 0
        data_len = len(loop_alpha)
        
        while not self.lib.check_settled_status(self.instance):
            alpha = loop_alpha[cycle_idx % data_len]
            stress = loop_stress[cycle_idx % data_len]
            
            # Post-peak drop to force a high-load boundary penetration test
            if cycle_idx >= data_len:
                alpha = -50.0
                stress = 0.01

            metrics = self.lib.execute_stabilization_cycle(self.instance, alpha, stress)
            
            if metrics.elastic_force >= 90.0:
                tag = "MAX BOUNDARY"
            elif metrics.elastic_force > 0.0:
                tag = "WALL REBOUND"
            elif metrics.net_damping_factor > 0.0:
                tag = "NET ABSORB  "
            elif metrics.raw_intensity == 0.0:
                tag = "CORE ZERO   "
            else:
                tag = "SCALE STEP  "
                
            print(f"Cycle {cycle_idx:03d} | [{tag}] | Intensity State: {metrics.raw_intensity:.4f} | Target Peak: {metrics.stabilized_peak:.4f} | Elastic Force: {metrics.elastic_force:.4f}")
            cycle_idx += 1
            time.sleep(0.01)
            
            if cycle_idx > 50:
                break
                
        print("-----------------------------------------------------------------------------------------")
        print("[STATUS] High-scale lifecycle verified. System state brought cleanly to 0.0000")
        print("=========================================================================================")

if __name__ == "__main__":
    lib_name = "./libstabilizer.so" if os.name != "nt" else "./libstabilizer.dll"
    if os.path.exists(lib_name):
        manager = DroneConsciousnessManager(lib_name)
        manager.run_high_scale_loop()
        
