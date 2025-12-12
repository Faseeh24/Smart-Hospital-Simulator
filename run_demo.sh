#!/bin/bash

# Smart Hospital Simulator Demo Script
# This script demonstrates the complete simulation with cleanup

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║       Smart Hospital Simulator - Demo Scenario                ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Cleanup any existing IPC resources
echo "Step 1: Cleaning up existing IPC resources..."
make clean-ipc 2>/dev/null
echo ""

# Clean build artifacts
echo "Step 2: Cleaning build artifacts..."
make clean 2>/dev/null
echo ""

# Build the project
echo "Step 3: Building the simulator..."
if make; then
    echo "✓ Build successful!"
    echo ""
else
    echo "✗ Build failed!"
    exit 1
fi

# Run the simulation
echo "Step 4: Running the simulation..."
echo "════════════════════════════════════════════════════════════════"
echo ""

./bin/hospital_simulator

EXIT_CODE=$?

echo ""
echo "════════════════════════════════════════════════════════════════"
echo ""

# Display log file
if [ -f "hospital_simulation.log" ]; then
    echo "Step 5: Displaying simulation log..."
    echo "════════════════════════════════════════════════════════════════"
    cat hospital_simulation.log
    echo "════════════════════════════════════════════════════════════════"
    echo ""
fi

# Cleanup IPC resources
echo "Step 6: Final cleanup of IPC resources..."
make clean-ipc 2>/dev/null
echo ""

if [ $EXIT_CODE -eq 0 ]; then
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║            Demo Completed Successfully!                        ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
else
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║            Demo Completed with Errors                          ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
fi

echo ""
echo "Log file: hospital_simulation.log"
echo "Executable: bin/hospital_simulator"
echo ""
