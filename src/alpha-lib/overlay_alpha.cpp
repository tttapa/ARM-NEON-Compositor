#include "overlay_alpha.hpp"
#include "SIMD.h"
#include "rescale.hpp"

#include <cassert>
#include <cstddef>
#include <cstring>

#if ENABLE_NEON

void overlay_alpha_8(const uint8_t *bg_img, const uint8_t *fg_img,
                               uint8_t *out_img) {
    uint8x8x4_t fg = vld4_u8(fg_img);
    uint8x8x4_t bg = vld4_u8(bg_img);

    uint8x8_t alpha   = fg.val[3];
    uint8x8_t alpha_c = vmvn_u8(alpha); // 255 - alpha

    // r = bg.r * (255 - alpha) + fg.r * alpha + round_const
    uint16x8_t r = vaddq_u16(vmull_u8(bg.val[2], alpha_c), //
                             vmull_u8(fg.val[2], alpha));

    uint16x8_t g = vaddq_u16(vmull_u8(bg.val[1], alpha_c), //
                             vmull_u8(fg.val[1], alpha));

    uint16x8_t b = vaddq_u16(vmull_u8(bg.val[0], alpha_c), //
                             vmull_u8(fg.val[0], alpha));

    bg.val[2] = rescale(r);
    bg.val[1] = rescale(g);
    bg.val[0] = rescale(b);

    vst4_u8(out_img, bg);
}

#elif ENABLE_SIMD32

void overlay_alpha_8(const uint8_t *bg_img, const uint8_t *fg_img,
                     uint8_t *out_img) {
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t alpha      = fg_img[4 * i + 3];
        uint8_t alpha_c    = ~alpha;
        uint16_t round     = 0; // TODO 1 << 7;
        int16x2_t op1      = alpha << 16 | alpha_c;
        int16x2_t op2_r    = fg_img[4 * i + 2] << 16 | bg_img[4 * i + 2];
        out_img[4 * i + 2] = rescale(__smlad(op1, op2_r, round));
        int16x2_t op2_g    = fg_img[4 * i + 1] << 16 | bg_img[4 * i + 1];
        out_img[4 * i + 1] = rescale(__smlad(op1, op2_g, round));
        int16x2_t op2_b    = fg_img[4 * i + 0] << 16 | bg_img[4 * i + 0];
        out_img[4 * i + 0] = rescale(__smlad(op1, op2_b, round));
        out_img[4 * i + 3] = bg_img[4 * i + 3];
    }
}

#else

void overlay_alpha_8(const uint8_t *bg_img, const uint8_t *fg_img,
                     uint8_t *out_img) {

    for (uint8_t i = 0; i < 8; ++i) {
        uint16_t alpha   = fg_img[4 * i + 3];
        uint16_t alpha_c = ~alpha;
        uint16_t r = fg_img[4 * i + 2] * alpha + bg_img[4 * i + 2] * alpha_c;
        out_img[4 * i + 2] = rescale(r);
        uint16_t g = fg_img[4 * i + 1] * alpha + bg_img[4 * i + 1] * alpha_c;
        out_img[4 * i + 1] = rescale(g);
        uint16_t b = fg_img[4 * i + 0] * alpha + bg_img[4 * i + 0] * alpha_c;
        out_img[4 * i + 0] = rescale(b);
        out_img[4 * i + 3] = bg_img[4 * i + 3];
    }
}

#endif

void overlay_alpha(const uint8_t *bg_img, const uint8_t *fg_img,
                   uint8_t *out_img, size_t n) {
    assert(n % 16 == 0);
#pragma omp parallel for
    for (size_t i = 0; i < n * 4; i += 4 * 8)
        overlay_alpha_8(&bg_img[i], &fg_img[i], &out_img[i]);
}

void overlay_alpha_stride(const uint8_t *bg_img, const uint8_t *fg_img,
                          uint8_t *out_img, size_t bg_full_cols, size_t fg_rows,
                          size_t fg_cols, size_t fg_full_cols) {

    const size_t fg_rem_cols = fg_cols % 8;

#pragma omp parallel for
    for (size_t r = 0; r < fg_rows; ++r) {
        for (size_t c = 0; c < fg_cols - 7; c += 8) {
            size_t bg_offset = 4 * (r * bg_full_cols + c);
            size_t fg_offset = 4 * (r * fg_full_cols + c);
            overlay_alpha_8(&bg_img[bg_offset], &fg_img[fg_offset],
                            &out_img[bg_offset]);
        }
        if (fg_rem_cols != 0) {
            size_t bg_offset = 4 * (r * bg_full_cols + fg_cols - fg_rem_cols);
            size_t fg_offset = 4 * (r * fg_full_cols + fg_cols - fg_rem_cols);
            uint8_t tmp_bg[4 * 8];
            uint8_t tmp_fg[4 * 8];
            uint8_t tmp_out[4 * 8];
            memcpy(tmp_bg, &bg_img[bg_offset], 4 * fg_rem_cols);
            memcpy(tmp_fg, &fg_img[fg_offset], 4 * fg_rem_cols);
            overlay_alpha_8(tmp_bg, tmp_fg, tmp_out);
            memcpy(&out_img[bg_offset], tmp_out, 4 * fg_rem_cols);
        }
    }
}
