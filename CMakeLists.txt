cmake_minimum_required(VERSION 2.8.12)

project(RPi-NEON-Compositor CXX)

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

# Compiler flags
# ---------------------------------------------------------------------------- #

# Warnings
set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} \
    -Wmissing-include-dirs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -Wall -Wextra -Werror -pedantic -pedantic-errors -fdiagnostics-show-option \
    -Wno-unknown-pragmas")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -Wall -Wextra -Werror -pedantic -pedantic-errors -fdiagnostics-show-option")
    
# Build-type specific flags
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -D_GLIBCXX_DEBUG")
set(CMAKE_C_FLAGS_DEBUG   "-ggdb3 -O0 -D_GLIBCXX_DEBUG")
set(CMAKE_DEBUG_POSTFIX "-debug")

set(CMAKE_CXX_FLAGS_ASAN "-g3 -O0 -D_GLIBCXX_DEBUG \
    -fsanitize=address,leak,undefined,pointer-compare,pointer-subtract")
set(CMAKE_C_FLAGS_ASAN   "-g3 -O0 -D_GLIBCXX_DEBUG \
    -fsanitize=address,leak,undefined,pointer-compare,pointer-subtract")
set(CMAKE_ASAN_POSTFIX "-asan")

set(CMAKE_CXX_FLAGS_TSAN "-g3 -O0 -D_GLIBCXX_DEBUG \
    -fsanitize=thread,undefined")
set(CMAKE_C_FLAGS_TSAN   "-g3 -O0 -D_GLIBCXX_DEBUG \
    -fsanitize=thread,undefined")
set(CMAKE_TSAN_POSTFIX "-tsan")

# Coverage
set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_ASAN} --coverage -fno-inline")
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_ASAN} --coverage -fno-inline")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "--coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

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
