#!/usr/bin/env python3.8

from os.path import dirname, realpath, join
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rc

rc('font',**{'family':'serif', 'size': 12})
rc('text', usetex=True)

dir = dirname(realpath(__file__))

def compare(name: str, filename_no_simd: str, filename_simd: str):
    small_no_simd = np.genfromtxt(join(dir, filename_no_simd), delimiter=',')
    small_simd    = np.genfromtxt(join(dir, filename_simd), delimiter=',')

    plt.figure(figsize=(6,6*3/4))
    plt.plot(small_no_simd[:,0], small_no_simd[:,1], '-',
            label='Without SIMD intrinsics')
    plt.plot(small_simd[:,0], small_simd[:,1], '-',
            label='With SIMD intrinsics')
    plt.xlim((0, small_simd[-1, 0]*1.025))
    plt.gca().set_ylim(bottom=0)

    plt.title("Comparison of \\verb|alpha_overlay_stride|\n" +
            "with and without SIMD intrinsics")
    plt.xlabel('Image size [pixels]')
    plt.ylabel('Time [s]')
    plt.legend()
    plt.tight_layout()
    plt.savefig(name + '.svg')

compare('small', 
        'Wed Jul  8 15:12:30 2020 no-simd.csv',
        'Wed Jul  8 15:13:39 2020 simd.csv')
compare('large', 
        'Wed Jul  8 15:45:17 2020 no-simd.csv',
        'Wed Jul  8 15:50:07 2020 simd.csv')
plt.show()