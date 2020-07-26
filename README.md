[![Build Status](https://github.com/tttapa/ARM-NEON-Compositor/workflows/CI%20Tests/badge.svg)](https://github.com/tttapa/ARM-NEON-Compositor/actions)
[![GitHub](https://img.shields.io/github/stars/tttapa/ARM-NEON-Compositor?label=GitHub&logo=github)](https://github.com/tttapa/ARM-NEON-Compositor)

# ARM/Raspberry Pi NEON Compositor

Uses NEON SIMD instructions to overlay a foreground image with an alpha channel
(transparency) over a background image really quickly.

For small images, it is up to **3.5 times faster** than an implementation without 
NEON intrinsics, and for really large images, it is around 1.4 times faster.

## Documentation

[**Documentation**](https://tttapa.github.io/ARM-NEON-Compositor/Doxygen/index.html)

The [**modules**](https://tttapa.github.io/ARM-NEON-Compositor/Doxygen/modules.html)
page is the best place to start. 
The main function is [`overlay_alpha_stride`](https://tttapa.github.io/ARM-NEON-Compositor/Doxygen/d6/d96/group__overlay__alpha.html#ga1f0ae30bcf7b623123eff96cf2a40525).

You can find more in-depth explanations of the NEON intrinsics used by this 
library here: [**Raspberry-Pi/NEON**](https://tttapa.github.io/Pages/Raspberry-Pi/NEON/index.html).

## Examples

The [`overlay_alpha`](https://tttapa.github.io/ARM-NEON-Compositor/Doxygen/dc/d0e/examples_2overlay_alpha_2overlay_alpha_8cpp-example.html)
example overlays a foreground image with an alpha channel onto a background
image, for example:

| Background | Foreground | Result |
|:----------:|:----------:|:------:|
| ![](doxygen/images/Machu-Picchu.thumb.jpg) | ![](doxygen/images/Overlay-Machu-Picchu.thumb.png) | ![](doxygen/images/Overlay-Machu-Picchu-Output.thumb.jpg) |

## Performance

The following two graphs show the results of four experiments comparing the
performance of overlaying one image onto another, using GCC's `-O3` optimization
level on the one hand, and using hand-crafted NEON intrinsics on the other hand.
Especially for small images, the NEON version is much faster.
For larger images, memory throughput and caching effects start to become more
important factors than raw processing power, but the NEON version is still 
significantly faster than the version without intrinsics.

| Small images | Large images |
|:------------:|:------------:|
| ![](doxygen/images/perf-small.svg) | ![](doxygen/images/perf-large.svg) |

The experiments were carried out on a Raspberry Pi 3B+ running Ubuntu 20.04 
(64-bit).  
More performance tests can be found in the [`perf_test`](https://tttapa.github.io/ARM-NEON-Compositor/Doxygen/dc/d46/examples_2perf_test_2perf_test_8py-example.html) example.
