# Compilation
# ---------------------------------------------------------------------------- #

add_library(alpha-lib 
    src/overlay_alpha.cpp)

target_include_directories(alpha-lib 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Options and dependencies
# ---------------------------------------------------------------------------- #

if (TARGET OpenMP::OpenMP_CXX)
    target_link_libraries(alpha-lib PRIVATE OpenMP::OpenMP_CXX)
endif()
if (TARGET NEON_2_SSE::NEON_2_SSE)
    target_link_libraries(alpha-lib PRIVATE NEON_2_SSE::NEON_2_SSE)
    target_compile_options(alpha-lib PRIVATE -Wno-deprecated-declarations)
endif()
if (TARGET SSE4)
    target_link_libraries(alpha-lib PRIVATE SSE4)
endif()

# Tests
# ---------------------------------------------------------------------------- #

add_subdirectory(test)

# Installation
# ---------------------------------------------------------------------------- #

# TODO