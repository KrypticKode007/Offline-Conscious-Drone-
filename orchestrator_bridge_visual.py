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

class DroneConsciousnessVisualizer:
    def __init__(self, shared_lib_path: str):
        resolved_path = os.path.abspath(shared_lib_path)
        if not os.path.exists(resolved_path):
            raise FileNotFoundError(f"Native binary layout missing at path: {resolved_path}")
            
        self.lib = ctypes.CDLL(resolved_path)
        self.lib.create_stabilizer.restype = ctypes.c_void_p
        self.lib.execute_stabilization_cycle.restype = ConsciousnessState
        self.lib.check_settled_status.restype = ctypes.c_bool
        self.lib.check_settled_status.argtypes = [ctypes.c_void_p]
        
        self.instance = self.lib.create_stabilizer()

    def generate_live_plots(self):
        print("=========================================================================================")
        print("             RUNNING CONFIGURATION: GENERATING GRAPHICAL TRACKING ARRAYS")
        print("=========================================================================================")
        
        loop_alpha = [10.0, 30.0, 60.0, 100.0, 95.0, 40.0, 10.0, 0.0]
        loop_stress = [0.1, 0.3, 0.6, 1.5, 1.2, 0.5, 0.1, 0.0]
        
        cycles = []
        intensities = []
        peaks = []
        forces = []
        
        cycle_idx = 0
        data_len = len(loop_alpha)
        
        while not self.lib.check_settled_status(self.instance):
            alpha = loop_alpha[cycle_idx % data_len]
            stress = loop_stress[cycle_idx % data_len]
            
            if cycle_idx >= data_len:
                alpha = -50.0
                stress = 0.01

            metrics = self.lib.execute_stabilization_cycle(self.instance, alpha, stress)
            
            # Append telemetry data vectors for terminal-based canvas painting
            cycles.append(cycle_idx)
            intensities.append(metrics.raw_intensity)
            peaks.append(metrics.stabilized_peak)
            forces.append(metrics.elastic_force)
            
            tag = "SCALE STEP"
            if metrics.elastic_force >= 90.0: tag = "MAX BOUNDARY"
            elif metrics.elastic_force > 0.0: tag = "WALL REBOUND"
            elif metrics.net_damping_factor > 0.0: tag = "NET ABSORB "
            elif metrics.raw_intensity == 0.0: tag = "CORE ZERO  "
                
            print(f"Cycle {cycle_idx:03d} | [{tag}] | Intensity: {metrics.raw_intensity:7.2f} | Peak Target: {metrics.stabilized_peak:7.2f} | Wall Force: {metrics.elastic_force:7.2f}")
            
            cycle_idx += 1
            if cycle_idx > 40:
                break
                
        self._render_terminal_plot(cycles, intensities, peaks)

    def _render_terminal_plot(self, cycles, intensities, peaks):
        print("\n" + "="*89)
        print("                     HIGH-SCALE TELEMETRY GRAPHICAL CURVE INTERFACE")
        print("="*89)
        print("  Intensity Level (Scale 0-130)")
        
        # Determine programmatic window limits for clean graph spacing
        max_val = 130
        step = 10
        
        for y_val in range(max_val, -10, -step):
            row_str = f"{y_val:3d} ┤ "
            for idx in range(len(cycles)):
                int_char = " "
                # Character plotting priority mapping
                if abs(intensities[idx] - y_val) < 5:
                    int_char = "█"
                elif abs(peaks[idx] - y_val) < 5:
                    int_char = "─"
                row_str += int_char * 2
            print(row_str)
            
        print("    └" + "─" * (len(cycles) * 2))
        print("      " + " ".join([f"{c:02d}" for c in cycles if c % 2 == 0]))
        print("  Execution Cycles (00-17)")
        print("\n  Legend:  █ State Intensity Surge   ─ Target Peak 100.0 Bound")
        print("="*89 + "\n")

if __name__ == "__main__":
    lib_name = "./libstabilizer.so" if os.name != "nt" else "./libstabilizer.dll"
    if os.path.exists(lib_name):
        visualizer = DroneConsciousnessVisualizer(lib_name)
        visualizer.generate_live_plots()
    else:
        print("[ERROR] Core engine shared library object missing. Recompile native code blocks.")
      
