/// @example examples/overlay_alpha/overlay_alpha.cpp

#include <algorithm> // std::min, max
#include <cassert>   // assert
#include <chrono>    // std::chrono::high_resolution_clock
#include <iomanip>   // std::setprecision
#include <iostream>  // std::cout
#include <string>    // std::string, std::stoi

#include <opencv2/imgcodecs.hpp> // cv::imread, cv::imwrite
#include <opencv2/imgproc.hpp>   // cv::cvtColor

#include <alpha-lib/overlay_alpha.hpp> // overlay_alpha_stride

using std::cout;
using std::endl;

// Print the usage and command line options.
void print_usage(const char *);

// Print how long it took to overlay the image.
template <class T>
void print_duration(T duration, int repeat, long num_pixels);

int main(int argc, char *argv[]) {
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
    if (bg_img.channels() != 4)
        throw std::runtime_error("incorrect background image color format");
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
        overlay_alpha_stride(bg_img.at<uint8_t[4]>(bg_y, bg_x), // background
                             fg_img.at<uint8_t[4]>(fg_y, fg_x), // foreground
                             bg_img.at<uint8_t[4]>(bg_y, bg_x), // output
                             bg_img.cols,         // background stride
                             fg_height, fg_width, // foreground dimensions
                             fg_img.cols);        // foreground stride
    }
    auto finish = std::chrono::high_resolution_clock::now();

    // Print how long it took
    long fg_pixels = 1L * fg_width * fg_height;
    print_duration(finish - start, repeat, fg_pixels);

    // Write the result to a file
    cv::imwrite("output.png", bg_img);
}

void print_usage(const char *cmd) {
    cout << "Usage: " << cmd << " <background> <overlay> <x> <y>" << endl;
}

using float_seconds = std::chrono::duration<float>;
std::ostream &operator<<(std::ostream &, float_seconds duration) {
    using namespace std::chrono_literals;
    std::cout << std::fixed << std::setprecision(3);
    if (duration < 1000ns) {
        return std::cout << duration.count() * 1e9 << " ns";
    } else if (duration < 1000us) {
        return std::cout << duration.count() * 1e6 << " µs";
    } else if (duration < 1000ms) {
        return std::cout << duration.count() * 1e3 << " ms";
    } else {
        return std::cout << duration.count() * 1e0 << " s";
    }
}

template <class T>
void print_duration(T duration, int repeat, long num_pixels) {
    auto duration_s = std::chrono::duration_cast<float_seconds>(duration);

    cout << duration_s << " = " << duration_s / double(repeat)
         << "/it = " << duration_s / double(num_pixels * repeat) << "/px"
         << endl;
}