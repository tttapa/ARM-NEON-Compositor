#pragma once

#if defined(__ARM_NEON)
#include <arm_neon.h>
#define ENABLE_NEON 1
#elif defined(__x86_64__)
#include <NEON_2_SSE.h>
#define ENABLE_NEON 1
#elif defined(__ARM_FEATURE_SIMD32)
#include <arm_acle.h>
#define ENABLE_NEON 0
#define ENABLE_SIMD32 1
#if defined(__GNUC__) && __GNUC__ < 10
#include "ARM-SIMD32.h"
#endif
#else
#define ENABLE_NEON 0
#define ENABLE_SIMD32 0
#endif
