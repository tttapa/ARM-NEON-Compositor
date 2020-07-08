#!/usr/bin/env python3.8

##
# @example examples/perf_test/perf_test.py
# 
# # Performance tests
# 
# This example tests the performance of the `overlay_alpha*` functions with
# different sizes of random images.  
# Ctypes is used to dynamically load the `alpha-lib` library in Python.
# 
# ## SIMD intrinsics
# 
# The following two graphs show the results of four experiments comparing the
# performance of overlaying one image onto another, using GCC's `-O3` 
# optimization level on the one hand, and using hand-crafted NEON intrinsics on 
# the other hand.
# Especially for small images, the NEON version is much faster.
# For larger images, memory throughput and caching effects start to become more
# important factors than raw processing power, but the NEON version is still 
# significantly faster than the version without intrinsics.
# 
# <center>
# ![Performance SIMD intrinsics small images](perf-small.svg)
# ![Performance SIMD intrinsics large images](perf-large.svg)
# </center>
# 
# ## Rounding methods
# 
# The difference between the different scaling and rounding methods is 
# negligible. As expected, an exact rounding division by 255 is slowest. An 
# approximation is slightly faster, because it eliminates a vector load 
# instruction to load the rounding constant. An exact flooring division by 255
# is a tiny bit faster still.
# 
# The fastest option is to divide by 256 instead of 255, as both the rounding 
# and flooring divisions by powers of two can be implemented using a single bit 
# shift instruction.  
# This does result in a small error in the output image. Most notably, combining
# two white pixels with color values `0xFF` will result in a slightly less white 
# pixel, with color value `0xFE`.
# 
# <center>
# ![Performance rescaling methods small images](perf-rescale-small.svg)
# </center>
# 
# This graph also clearly shows the slightly better performance when the image 
# size is a multiple of eight. The reason is the size of the NEON registers, 
# which is four words, or eight 16-bit integers. When the number of columns of 
# the foreground image is not a multiple of eight, extra code is needed to 
# process the last pixels of each row, resulting in lower performance.
#
import os.path as path
import cv2
import numpy as np
import ctypes
import time
import platform
import argparse

dir = path.dirname(path.realpath(__file__))

parser = argparse.ArgumentParser(description='Benchmark for overlay_alpha')
parser.add_argument('--no-simd', dest='SIMD', action='store_false',
                    help='Disable SIMD intrinsics')
parser.add_argument('--N', dest='N', type=int, default=25,
                    help='The number of different sizes to test')
parser.add_argument('--min', dest='min_size', type=int, default=10,
                    help='The size in pixels of the smallest image in the test')
parser.add_argument('--max', dest='max_size', type=int, default=2000,
                    help='The size in pixels of the largest image in the test')
parser.add_argument('--it', dest='max_iterations', type=int, default=10,
                    help='The number of test iterations for the largest images')
parser.add_argument('--rescale', dest='rescale', choices=['div255_round', 
                    'div255_round_approx', 'div255_floor', 'div256_round',
                    'div256_floor'], default='div255_round',
                    help='The number of test iterations for the largest images')
args = parser.parse_args()
print(args)

# C types
uint8_t_p = ctypes.POINTER(ctypes.c_uint8)
size_t = ctypes.c_size_t
void = None
# Load the function and declare its signature
so = "libalpha-lib-"
so += platform.machine()
if not args.SIMD: so += "-no-simd"
so += ".so"
dll = ctypes.cdll.LoadLibrary(path.join(dir, so))
overlay_alpha = dll['overlay_alpha_stride_' + args.rescale]
overlay_alpha.argtypes = [
    uint8_t_p,  # bg_img
    uint8_t_p,  # fg_img
    uint8_t_p,  # out_img
    size_t,  # bg_full_cols
    size_t,  # fg_rows
    size_t,  # fg_cols
    size_t,  # fg_full_cols
]
overlay_alpha.restype = void

# The different image sizes
sizes = np.linspace(args.min_size, args.max_size, args.N, dtype=np.int)
times = np.zeros((args.N, ))

# Run the function for all sizes
for i, size in enumerate(sizes):
    print(i + 1, '/', args.N, ':', size)

    # Generate some random images of the given size
    bg_img = np.random.randint(255, size=(size, size, 4), dtype=np.uint8)
    fg_img = np.random.randint(255, size=(size, size, 4), dtype=np.uint8)
    out_img = np.zeros((size, size, 4), dtype=np.uint8)
    bg_img_p = bg_img.ctypes.data_as(uint8_t_p)
    fg_img_p = fg_img.ctypes.data_as(uint8_t_p)
    out_img_p = out_img.ctypes.data_as(uint8_t_p)

    # Overlay the random images, do it multiple times for accurate timing
    iterations = int(round(args.max_size * args.max_iterations / size))
    start_time = time.perf_counter()
    for _ in range(iterations):
        overlay_alpha(bg_img_p, fg_img_p, out_img_p, size, size, size, size)
    end_time = time.perf_counter()
    times[i] = (end_time - start_time) / iterations

# Save the results as a CSV file
results = np.column_stack((sizes, times))
simd = (' simd ' if args.SIMD else ' no-simd ')
name = str(time.asctime()) + simd + args.rescale + ' ' + platform.machine()
np.savetxt(name + '.csv', results, delimiter=',')

# Plot the results
import matplotlib.pyplot as plt
plt.plot(sizes, times, '.-')
plt.xlabel('Image size [pixels]')
plt.ylabel('Time [s]')
plt.savefig(name + '.svg')
plt.show()
