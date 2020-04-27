#include <algorithm> // std::min,max
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <overlay_alpha.hpp>

using std::cout;
using std::endl;

void print_usage(const char *);

int main(int argc, char **argv) {

    // Parse the command line arguments
    if (argc <= 2) {
        print_usage(argv[0]);
        return argc == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    std::string bg_fname = argv[1];
    std::string fg_fname = argv[2];
    int x                = argc > 3 ? std::stoi(argv[3]) : 0;
    int y                = argc > 4 ? std::stoi(argv[4]) : 0;
    int repeat           = argc > 5 ? std::stoi(argv[5]) : 1;

    // Load the images
    cv::Mat bg_img = imread(bg_fname, cv::IMREAD_UNCHANGED);
    cv::Mat fg_img = imread(fg_fname, cv::IMREAD_UNCHANGED);
    if (bg_img.empty())
        throw std::runtime_error("unable to load " + bg_fname);
    if (fg_img.empty())
        throw std::runtime_error("unable to load " + fg_fname);

    // Check that the images are in the right format
    if (bg_img.channels() == 3)
        cv::cvtColor(bg_img, bg_img, cv::COLOR_BGR2BGRA);
    if (fg_img.channels() != 4)
        throw std::runtime_error("foreground image without alpha channel");

    // Check that the x and y coordinates are inside of the background image
    if (x >= bg_img.cols)
        throw std::runtime_error("x coordinate out of bounds");
    if (y >= bg_img.rows)
        throw std::runtime_error("y coordinate out of bounds");

    // Compute what parts of the background and foreground image to use
    int fg_x      = std::max(0, -x);
    int fg_y      = std::max(0, -y);
    int bg_x      = std::max(0, x);
    int bg_y      = std::max(0, y);
    int fg_width  = std::min(fg_img.cols - fg_x, bg_img.cols - x);
    int fg_height = std::min(fg_img.rows - fg_y, bg_img.rows - y);

    // Overlay the foreground image over the background image
    auto start = std::chrono::high_resolution_clock::now();
    for (int r = 0; r < repeat; ++r) {
        overlay_alpha_stride(bg_img.at<uint8_t[4]>(bg_y, bg_x),
                             fg_img.at<uint8_t[4]>(fg_y, fg_x),
                             bg_img.at<uint8_t[4]>(bg_y, bg_x), bg_img.cols,
                             fg_height, fg_width, fg_img.cols);
    }
    auto finish = std::chrono::high_resolution_clock::now();

    // Print how long it took
    using std::chrono::duration_cast;
    auto ms        = duration_cast<std::chrono::milliseconds>(finish - start);
    auto us        = duration_cast<std::chrono::microseconds>(finish - start);
    auto ns        = duration_cast<std::chrono::nanoseconds>(finish - start);
    long fg_pixels = 1L * fg_width * fg_height;
    cout << ms.count() << " ms = " << (us.count() / double(repeat))
         << " µs/it = " << (ns.count() / double(fg_pixels * repeat)) << " ns/px"
         << endl;

    // Write the result to a file
    cv::imwrite("output.png", bg_img);
}

void print_usage(const char *cmd) {
    cout << "Usage: " << cmd << " background overlay x y" << endl;
}