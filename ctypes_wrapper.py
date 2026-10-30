import ctypes
import os
import sys

class TelemetryState(ctypes.Structure):
    """Maps memory alignment fields matching structural design rules in C++."""
    _fields_ = [
        ("current_temperature", ctypes.c_double),
        ("surprise_velocity", ctypes.c_double),
        ("material_friction", ctypes.c_double),
        ("metabolic_compression", ctypes.c_bool)
    ]

class KrypticCoreBridge:
    def __init__(self, lib_name="./libsentience_core.so"):
        self.lib_path = os.path.abspath(lib_name)
        print(f"[STATUS]: Binding native runtime pipeline source: {self.lib_path}")
        
        try:
            # Bind shared compiled library module configurations
            self.lib = ctypes.CDLL(self.lib_path)
            self._setup_bindings()
        except Exception as e:
            print(f"[CRITICAL FAILURE]: Could not map native object dependencies: {e}")
            print("💡 Reminder: Compile your C++ shared object library before executing this script!")
            sys.exit(1)

    def _setup_bindings(self):
        """Sets arguments and return types for the native shared library functions."""
        # Constructor / Destructor bindings
        self.lib.create_tracker.argtypes = []
        self.lib.create_tracker.restype = ctypes.c_void_p
        
        self.lib.destroy_tracker.argtypes = [ctypes.c_void_p]
        self.lib.destroy_tracker.restype = None

        # Data ingestion pipeline bindings
        self.lib.ingest_sensor_stream.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t]
        self.lib.ingest_sensor_stream.restype = None

        # State evaluation and manipulation bindings
        self.lib.get_active_telemetry.argtypes = [ctypes.c_void_p]
        self.lib.get_active_telemetry.restype = TelemetryState

        self.lib.enforce_allostatic_jitter.argtypes = [ctypes.c_void_p, ctypes.c_double]
        self.lib.enforce_allostatic_jitter.restype = None

def compile_native_binary():
    """Helper method to run a system shell build call for compiling the shared library wrapper."""
    print("[BUILD]: Invoking local g++ compilation pipeline...")
    cpp_source = "main_hardware_launcher.cpp" # Alternately link code to an export wrapper file
    output_lib = "libsentience_core.so"
    
    # Check for platform adjustments (Linux/macOS vs Windows)
    compile_cmd = f"g++ -O3 -shared -fPIC -std=c++17 {cpp_source} -o {output_lib}"
    
    if os.system(compile_cmd) == 0:
        print(f"[SUCCESS]: Binary compiled cleanly -> {output_lib}")
    else:
        print("[ERROR]: Compilation sequence dropped hooks. Verify your local g++ configurations.")

if __name__ == "__main__":
    # Test script will attempt to compile itself locally if executed directly
    compile_native_binary()
      
