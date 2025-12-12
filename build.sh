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

./hardware_proto