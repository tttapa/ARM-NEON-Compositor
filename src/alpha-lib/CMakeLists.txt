add_library(alpha_lib overlay_alpha.cpp)
target_include_directories(alpha_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(alpha_lib PRIVATE OVERLAY_ALPHA_DIV_256=0
                                             OVERLAY_ALPHA_DIV_ROUND=1
                                             OVERLAY_ALPHA_DIV_APPROX_ROUND=0)

if (TARGET OpenMP::OpenMP_CXX)
    target_link_libraries(alpha_lib PRIVATE OpenMP::OpenMP_CXX)
endif()
if (TARGET NEON_2_SSE::NEON_2_SSE)
    target_link_libraries(alpha_lib PRIVATE NEON_2_SSE::NEON_2_SSE)
    target_compile_options(alpha_lib PRIVATE -Wno-deprecated-declarations)
endif()
if (TARGET SSE4)
    target_link_libraries(alpha_lib PRIVATE SSE4)
endif()

add_subdirectory(test)