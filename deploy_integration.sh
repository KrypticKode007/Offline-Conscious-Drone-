#!/usr/bin/env bash
# =========================================================================================
#             KRYPTIC SWARM MATRIX FRAMEWORK - BARE-METAL EDGE DEPLOYMENT
# =========================================================================================
set -euo pipefail

# Operational Color Codes for Terminal Readouts
export LOG_INFO="\e[34m[INFO]\e[0m"
export LOG_SUCCESS="\e[32m[SUCCESS]\e[0m"
export LOG_ERROR="\e[31m[ERROR]\e[0m"

echo -e "========================================================================================="
echo -e "                 INITIALIZING AUTOMATED BARE-METAL COMPILATION CHAIN"
echo -e "========================================================================================="

# 1. Environment and Prerequisite Toolchain Audit
echo -e "${LOG_INFO} Performing host architecture verification..."
if ! command -v g++ &> /dev/null; then
    echo -e "${LOG_ERROR} Native GNU C++ Compiler (g++) is missing. Run: sudo apt install build-essential"
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo -e "${LOG_ERROR} Python3 run-time engine could not be resolved."
    exit 1
fi

# 2. Workspace File Layout Structural Validation
echo -e "${LOG_INFO} Verifying integrity of deployment repository workspace..."
REQUIRED_FILES=("stabilizer_core.hpp" "orchestrator_bridge.py" "config.json")
for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo -e "${LOG_ERROR} Critical deployment dependency asset target missing: $file"
        exit 1
    fi
done

# 3. Native Dynamic Library Compilation Optimization Sequence
echo -e "${LOG_INFO} Executing bare-metal g++ optimization build layer..."
LIB_OUT="./libstabilizer.so"
if [ "$(uname)" == "Darwin" ]; then
    LIB_OUT="./libstabilizer.dylib"
fi

g++ -O3 -shared -fPIC -std=c++11 \
    -ffast-math \
    -funroll-loops \
    -march=native \
    stabilizer_core.hpp -o "$LIB_OUT"

echo -e "${LOG_SUCCESS} Shared binary compiled successfully: $LIB_OUT"

# 4. Shared Binary Security Mapping and Permissions Setup
echo -e "${LOG_INFO} Setting environment execution context permissions..."
chmod 755 "$LIB_OUT"

# 5. Local Runtime Verification and Smoke Test Pipeline
echo -e "${LOG_INFO} Spawning python interface mapping smoke-test suite..."
if python3 orchestrator_bridge.py; then
    echo -e "${LOG_SUCCESS} Bare-metal pipeline verification checks passed flawlessly."
else
    echo -e "${LOG_ERROR} Post-deployment run loop simulation failed script validation."
    exit 1
fi

# 6. Housekeeping and Artifact Pruning Phase
echo -e "${LOG_INFO} Clearing residual environment workspace artifacts..."
rm -rf __pycache__ .pytest_cache

echo -e "-----------------------------------------------------------------------------------------"
echo -e "${LOG_SUCCESS} EDGE ARCHITECTURE SYSTEM STACK DEPLOYMENT COMPLETE."
echo -e "========================================================================================="
