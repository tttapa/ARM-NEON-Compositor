cmake_minimum_required(VERSION 2.8.12)

project(OpenCV-NEON-Compositor CXX)

# Unit tests
# ---------------------------------------------------------------------------- #

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

# External libraries
# ---------------------------------------------------------------------------- #

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

# Options
# ---------------------------------------------------------------------------- #

option(DISABLE_NEON "Disable NEON intrinsics" OFF)
if (DISABLE_NEON)
    add_compile_definitions(DISABLE_NEON)
    message(STATUS "NEON disabled")
endif()

# Source code
# ---------------------------------------------------------------------------- #

add_subdirectory(src)

# Examples
# ---------------------------------------------------------------------------- #

add_subdirectory(examples)

# Documentation
# ---------------------------------------------------------------------------- #

find_package(Doxygen)

if (DOXYGEN_FOUND)
    add_custom_target(documentation 
        Doxygen::doxygen
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/doxygen")
endif()
