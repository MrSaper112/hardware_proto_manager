cmake_minimum_required(VERSION 3.21)

project(hardware_proto
    VERSION 1.0.0
    DESCRIPTION "CMake Project"
    LANGUAGES CXX
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please use a separate build directory.")
endif()

add_subdirectory(external/nlohmann_json)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "" CACHE STRING "" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")


set(MAIN_FILE "${PROJECT_SOURCE_DIR}/src/main.cpp")

file(GLOB_RECURSE OTHER_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
list(REMOVE_ITEM OTHER_FILES "${MAIN_FILE}")

set(SRC_FILES ${MAIN_FILE} ${OTHER_FILES})

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_features(${PROJECT_NAME}
    PRIVATE
        cxx_std_20
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        nlohmann_json::nlohmann_json
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
            -Wall -Wextra
            -O3
    )
endif()
