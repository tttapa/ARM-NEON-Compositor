add_subdirectory(alpha-lib)

if (OpenCV_FOUND)
    add_executable(overlay_alpha main.cpp)
    target_link_libraries(overlay_alpha PRIVATE opencv_imgcodecs alpha_lib)
endif()