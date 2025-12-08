cmake_minimum_required(VERSION 3.10)

project(hardware_proto_manager VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Source files
set(SOURCES
    src/main.cpp
    src/hardware_manager.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# If you have libraries in libs/, you can add them here
# add_subdirectory(libs)
