#pragma once

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#endif

typedef int32_t int16x2_t;

inline static int32_t __attribute__((__always_inline__))
__smlad(int16x2_t __a, int16x2_t __b, int32_t __c) {
    uint32_t result;
    __asm__ volatile("smlad %0, %1, %2, %3"
                     : "=r"(result)
                     : "r"(__a), "r"(__b), "r"(__c));
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif