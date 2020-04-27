cmake_minimum_required(VERSION 2.8.12)

project(OpenCV-NEON-Compositor CXX)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

find_package(OpenCV)
find_package(OpenMP)

if (NOT OpenCV_FOUND)
    message(WARNING "OpenCV not found. Examples will not be built.")
endif()

# On x86, use the NEON_2_SSE library to convert NEON intrinsics to their Intel
# counterparts.
# Install it from https://github.com/intel/ARM_NEON_2_x86_SSE
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86.*|x64.*|amd64|AMD64")
    find_package(NEON_2_SSE REQUIRED)
    add_library(SSE4 INTERFACE IMPORTED)
    target_compile_options(SSE4 INTERFACE -msse4 -DUSE_SSE4=1)
endif()

add_subdirectory(src)