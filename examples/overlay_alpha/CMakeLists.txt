if (OpenCV_FOUND)
    add_executable(overlay_alpha overlay_alpha.cpp)
    target_link_libraries(overlay_alpha PRIVATE opencv_imgcodecs alpha_lib)
endif()