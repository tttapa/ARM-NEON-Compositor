#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int i, j, alpha;
    long err = 0;
    long tot = 0;
    for (i = 0; i < 256; i++) {
        for (j = 0; j < 256; j++) {
            for (alpha = 0; alpha < 256; alpha++) {
                uint32_t value = (i * alpha + j * (255 - alpha));
#if 0   // sloppy, just dividing by 256: 8546790/16777216 = 50.942838% incorrect
                int a = value >> 8;
                int b = value / 255;
#elif 0 // better: 446670/16777216 = 2.662361%
                int a = (value * 0x101) >> 16;
                int b = value / 255;
#elif 0 // better: 446670/16777216 = 2.662361%
                int a = (value * 0x4040) >> 22;
                int b = value / 255;
#elif 0 // rounding, /256: 8323072/16777216 = 49.609375%
                int a = (value + (1 << 7)) >> 8;
                int b = round(value / 255.);
#elif 0 // rounding: 16224/16777216 = 0.096703%
                int a = (value * 0x101 + (1 << 15)) >> 16;
                int b = round(value / 255.);
#elif 0 // rounding: 15948/16777216 = 0.095057%
                int a = (value * 0x8081 + (1 << 22)) >> 23;
                int b = round(value / 255.);
#elif 0 // exact rounding: 0/16777216 = 0.000000%
                int a = ((value + (1 << 7)) * 0x101) >> 16;
                int b = round(value / 255.);
#elif 0 // exact: 0/16777216 = 0.000000%
                int a = (value * 0x8081) >> 23;
                int b = value / 255;
#else   // exact: 0/16777216 = 0.000000%
                int a = (value * 257 + i + j) >> 16;
                int b = value / 255;
#endif
                if (a != b) {
                    // printf("%d, %d, %d\n", i, j, alpha);
                    assert(abs(a - b) <= 1);
                    err++;
                }
                tot++;
            }
        }
    }

    printf("%ld/%ld = %f%%\n", err, tot, 100. * err / tot);
    return 0;
}