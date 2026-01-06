#!/bin/bash
set -e

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

cmake .. -G "Unix Makefiles"
make -j"$(nproc)"

echo "Build completed!"
echo ""

echo "Usage Instructions:"
echo   "To run the program, use the following command:"
echo   "./build/hardware_proto                    # Runs TestDevice with plain protocol"
echo   "./build/hardware_proto led                # Runs LedController with plain protocol"

echo   "./build/hardware_proto plain test         # Runs TestDevice with plain protocol"
echo   "./build/hardware_proto plain led          # Runs LedController with plain protocol"
echo   "./build/hardware_proto shift              # Runs TestDevice with shift protocol (default 0x69)"
echo   "./build/hardware_proto shift led          # Runs LedController with shift protocol (default 0x69)"

echo   "./build/hardware_proto shift 0x21         # Runs TestDevice with shift protocol (custom value)"
echo   "./build/hardware_proto shift 0x31 led     # Runs LedController with shift protocol (custom value)"