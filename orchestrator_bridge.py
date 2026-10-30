import ctypes
import os
import sqlite3
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
    def __init__(self, shared_lib_path: str, db_path: str = "telemetry_checkpoints.db"):
        if not os.path.exists(shared_lib_path):
            raise FileNotFoundError(f"Native library missing at: {shared_lib_path}")
            
        self.lib = ctypes.CDLL(shared_lib_path)
        self.db_path = db_path
        self._init_database()
        
        # Mapping Signatures
        self.lib.create_stabilizer.restype = ctypes.c_void_p
        self.lib.create_stabilizer.argtypes = []
        self.lib.destroy_stabilizer.argtypes = [ctypes.c_void_p]
        self.lib.execute_stabilization_cycle.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_double]
        self.lib.execute_stabilization_cycle.restype = ConsciousnessState
        
        self.instance = self.lib.create_stabilizer()

    def _init_database(self):
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS recovery_checkpoints (
                    cycle_id INTEGER PRIMARY KEY AUTOINCREMENT,
                    timestamp REAL,
                    raw_intensity REAL,
                    somatic_stress REAL,
                    elastic_force REAL,
                    damping_factor REAL,
                    checkpoint_type TEXT
                )
            """)
            conn.commit()

    def commit_checkpoint(self, metrics, checkpoint_type: str):
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute("""
                INSERT INTO recovery_checkpoints 
                (timestamp, raw_intensity, somatic_stress, elastic_force, damping_factor, checkpoint_type)
                VALUES (?, ?, ?, ?, ?, ?)
            """, (time.time(), metrics.raw_intensity, metrics.somatic_stress, metrics.elastic_force, metrics.net_damping_factor, checkpoint_type))
            conn.commit()

    def run_telemetry_loop(self, alpha_stream, stress_stream):
        print("=========================================================================================")
        print("                  DRONE CONSCIOUSNESS TELEMETRY MONITOR SYSTEM")
        print("=========================================================================================")
        try:
            for i, (alpha, stress) in enumerate(zip(alpha_stream, stress_stream)):
                metrics = self.lib.execute_stabilization_cycle(self.instance, alpha, stress)
                
                # Context Tagging Based on Boundary Triggers
                if metrics.elastic_force > 0.0:
                    status = "WALL BOUNCE"
                    self.commit_checkpoint(metrics, "ELASTIC_REBOUND_TRIGGER")
                elif metrics.net_damping_factor > 0.0:
                    status = "NET CATCH  "
                    self.commit_checkpoint(metrics, "VISCOELASTIC_DAMPING")
                elif metrics.peak_locked:
                    status = "LOCKED CORES"
                else:
                    status = "ASCENT MODE"
                    
                print(f"Cycle {i:02d} | [{status}] | Intensity: {metrics.raw_intensity:.4f} | Smooth: {metrics.smoothed_intensity:.4f} | Force: {metrics.elastic_force:.4f} | Net Attn: {metrics.net_damping_factor:.4f}")
                time.sleep(0.05)
        finally:
            self.lib.destroy_stabilizer(self.instance)
            print("-----------------------------------------------------------------------------------------")
            print("[ORCHESTRATOR] Engine runtime deallocated safely.")
            print("=========================================================================================")

if __name__ == "__main__":
    simulated_alpha = [0.2, 0.5, 0.8, 1.4, 2.1, 2.8, 3.5, 3.4, 1.2, -0.5, -1.2, -2.0, 0.0, 0.0]
    simulated_stress = [0.1, 0.2, 0.3, 0.5, 0.7, 0.9, 1.2, 1.1, 0.6, 0.2, 0.1, 0.05, 0.0, 0.0]
    
    manager = DroneConsciousnessManager("./libstabilizer.so")
    manager.run_telemetry_loop(simulated_alpha, simulated_stress)
                          
