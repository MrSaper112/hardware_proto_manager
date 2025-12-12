cmake_minimum_required(VERSION 3.21)

project(hardware_proto
    VERSION 1.0.0
    DESCRIPTION "CMake Project"
    LANGUAGES CXX
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please use a separate build directory.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "" CACHE STRING "" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        src/main.cpp
        "src/devices/IDevice.cpp" 
        "src/devices/LedControllerDevice.cpp" 
        "src/protocols/JsonProtocolAdapter.cpp" 
        "src/protocols/PlainProtocolAdapter.cpp"
        "src/transport/RealUartTransport.cpp" 
        "src/transport/SimulatedUartTransport.cpp")

target_compile_features(${PROJECT_NAME}
    PRIVATE
        cxx_std_20
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

if(MSVC)
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            /W4                     
            /permissive-            
            /Zc:__cplusplus       
            /Zc:preprocessor
            /O2
            /EHsc
    )
else()
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            -Wall -Wextra -Wpedantic
            -O3
    )
endif()

