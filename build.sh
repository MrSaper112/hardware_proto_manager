#!/bin/bash

if [ -d "build" ]; then
    echo "Build directory exists, rebuilding..."
    cd build
    echo "Building project..."
    cmake --build .
else
    echo "Creating build directory..."
    mkdir build
    cd build
    
    echo "Current directory: $(pwd)"

    echo "Configuring project with CMake..."
    cmake ../CMakeLists.txt

    echo "Building project..."
    cmake --build .

    ./hardware_proto_manager
fi

echo ""
echo "================================"
echo "Build completed successfully!"
echo "================================"
echo "  ./build/hardware_proto_manager    (Linux/WSL)"

