cmake_minimum_required(VERSION 3.15)
project(KrypticEdgeCore LANGUAGES CXX)

# Force standard C++17 optimization configurations
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Inject enterprise-grade production compiler optimization and security flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -O3             # Maximum performance loop-unrolling optimizations
        -Wall           # Enable all basic warnings
        -Wextra          # Enable extra structural checks
        -Werror         # Treat all compiler warnings as fatal compilation blocks
        -fPIC           # Position Independent Code generation for shared libraries
    )
endif()

# Specify target tracking directories for header inclusions
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# =========================================================================
# TARGET 1: Compile Native Standalone Hardware Simulator Binary
# =========================================================================
add_executable(kryptic_core_simulator main_hardware_launcher.cpp)

# =========================================================================
# TARGET 2: Compile Python-Ready C-Binding Shared Library Binary (.so)
# =========================================================================
add_library(sentience_core SHARED main_hardware_launcher.cpp)

# Set custom output configurations to name output artifact as 'libsentience_core.so'
set_target_properties(sentience_core PROPERTIES
    OUTPUT_NAME "sentience_core"
    PREFIX "lib"
) 

cmake_minimum_required(VERSION 3.10)
project(KrypticDroneStabilizer CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flags for performance-critical execution
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -fPIC")

# Build the shared object library for ctypes interface mapping
add_library(stabilizer SHARED stabilizer_core.hpp)

# Install location structure
install(TARGETS stabilizer DESTINATION ${CMAKE_BINARY_DIR}/lib)


# Output runtime update metrics upon successful build completion
message(STATUS "--------------------------------------------------------")
message(STATUS "Kryptic Core Build Engine Context Configured.")
message(STATUS "Target Execution Binaries: 'kryptic_core_simulator'")
message(STATUS "Target Python Interface Libraries: 'libsentience_core.so'")
message(STATUS "--------------------------------------------------------")
