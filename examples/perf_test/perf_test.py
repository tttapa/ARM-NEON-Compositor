#!/usr/bin/env python3.8

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
overlay_alpha = dll.overlay_alpha_stride_div255_round
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

###

sizes = np.linspace(args.min_size, args.max_size, args.N, dtype=np.int)
times = np.zeros((args.N, ))

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

results = np.column_stack((sizes, times))
name = str(time.asctime()) + (' simd' if args.SIMD else ' no-simd')
np.savetxt(name + '.csv', results, delimiter=',')

import matplotlib.pyplot as plt
plt.plot(sizes, times, '.-')
plt.xlabel('Image size [pixels]')
plt.ylabel('Time [s]')
plt.savefig(name + '.svg')
plt.show()
