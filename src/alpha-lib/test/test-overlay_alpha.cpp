#include <gtest/gtest.h>
#include <overlay_alpha.hpp>
#include <rescale.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#if ENABLE_NEON

TEST(overlay_alpha, rescale_div256_floor) {
    std::vector<uint16_t> in(1ul << 16);
    std::iota(std::begin(in), std::end(in),
              std::numeric_limits<uint16_t>::min());
    std::vector<uint8_t> expected(in.size());
    std::vector<uint8_t> result(in.size());

    for (size_t i = 0; i < in.size(); i += 8) {
        vst1_u8(&result[i], div256_floor(vld1q_u16(&in[i])));
        for (size_t j = 0; j < 8; ++j) {
            expected[i + j] = in[i + j] / 256;
        }
    }
    EXPECT_EQ(result, expected);
}

TEST(overlay_alpha, rescale_div256_round) {
    std::vector<uint16_t> in(1ul << 16);
    std::iota(std::begin(in), std::end(in),
              std::numeric_limits<uint16_t>::min());
    std::vector<uint8_t> expected(in.size());
    std::vector<uint8_t> result(in.size());

    for (size_t i = 0; i < in.size(); i += 8) {
        vst1_u8(&result[i], div256_round(vld1q_u16(&in[i])));
        for (size_t j = 0; j < 8; ++j) {
            expected[i + j] = std::round(in[i + j] / 256.);
        }
    }
    EXPECT_EQ(result, expected);
}

TEST(overlay_alpha, rescale_div255_floor) {
    std::vector<uint16_t> in(1ul << 16);
    std::iota(std::begin(in), std::end(in),
              std::numeric_limits<uint16_t>::min());
    std::vector<uint8_t> expected(in.size());
    std::vector<uint8_t> result(in.size());

    for (size_t i = 0; i < in.size(); i += 8) {
        vst1_u8(&result[i], div255_floor(vld1q_u16(&in[i])));
        for (size_t j = 0; j < 8; ++j) {
            expected[i + j] = in[i + j] / 255;
        }
    }
    EXPECT_EQ(result, expected);
}

TEST(overlay_alpha, rescale_div255_round) {
    std::vector<uint16_t> in(255 * 255 + 8);
    std::iota(std::begin(in), std::end(in),
              std::numeric_limits<uint16_t>::min());
    std::vector<uint8_t> expected(in.size());
    std::vector<uint8_t> result(in.size());

    for (size_t i = 0; i < in.size(); i += 8) {
        vst1_u8(&result[i], div255_round(vld1q_u16(&in[i])));
        for (size_t j = 0; j < 8; ++j) {
            expected[i + j] = std::round(in[i + j] / 255.);
        }
    }
    EXPECT_EQ(result, expected);
}

TEST(overlay_alpha, rescale_div255_round_approx) {
    std::vector<uint16_t> in(1ul << 16);
    std::iota(std::begin(in), std::end(in),
              std::numeric_limits<uint16_t>::min());
    std::vector<uint8_t> expected(in.size());
    std::vector<uint8_t> result(in.size());

    for (size_t i = 0; i < in.size(); i += 8) {
        vst1_u8(&result[i], div255_round_approx(vld1q_u16(&in[i])));
        for (size_t j = 0; j < 8; ++j) {
            expected[i + j] = std::round(in[i + j] / 255.);
        }
    }
    std::vector<int8_t> differences(in.size());
    std::transform(std::begin(expected), std::end(expected), std::begin(result),
                   std::begin(differences), std::minus<>());
    auto errors = std::count_if(std::begin(differences), std::end(differences),
                                [](int8_t d) { return d != 0; });
    EXPECT_LE(errors, 127);
    EXPECT_LE(*std::max(std::begin(differences), std::end(differences)), 1);
    EXPECT_GE(*std::min(std::begin(differences), std::end(differences)), 0);
}

#endif


#if 0
            if (expected[i + j] != result[i + j])
                std::cout << (i + j) << ", " << +expected[i + j] << ", "
                          << +result[i + j] << std::endl;
#endif