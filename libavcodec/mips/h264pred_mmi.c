/*
 * Loongson SIMD optimized h264pred
 *
 * Copyright (c) 2015 Loongson Technology Corporation Limited
 * Copyright (c) 2015 Zhou Xiaoyong <zhouxiaoyong@loongson.cn>
 *                    Zhang Shuangshuang <zhangshuangshuang@ict.ac.cn>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "h264pred_mips.h"
#include "constants.h"

void ff_pred16x16_vertical_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "dsubu $2, %0, %1                   \r\n"
        "daddu $3, %0, $0                   \r\n"
        "ldl $4, 7($2)                      \r\n"
        "ldr $4, 0($2)                      \r\n"
        "ldl $5, 15($2)                     \r\n"
        "ldr $5, 8($2)                      \r\n"
        "dli $6, 0x10                       \r\n"
        "1:                                 \r\n"
        "sdl $4, 7($3)                      \r\n"
        "sdr $4, 0($3)                      \r\n"
        "sdl $5, 15($3)                     \r\n"
        "sdr $5, 8($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "daddiu $6, -1                      \r\n"
        "bnez $6, 1b                        \r\n"
        ::"r"(src),"r"(stride)
        : "$2","$3","$4","$5","$6","memory"
    );
}

void ff_pred16x16_horizontal_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "daddiu $2, %0, -1                  \r\n"
        "daddu $3, %0, $0                   \r\n"
        "dli $6, 0x10                       \r\n"
        "1:                                 \r\n"
        "lbu $4, 0($2)                      \r\n"
        "dmul $5, $4, %2                    \r\n"
        "sdl $5, 7($3)                      \r\n"
        "sdr $5, 0($3)                      \r\n"
        "sdl $5, 15($3)                     \r\n"
        "sdr $5, 8($3)                      \r\n"
        "daddu $2, %1                       \r\n"
        "daddu $3, %1                       \r\n"
        "daddiu $6, -1                      \r\n"
        "bnez $6, 1b                        \r\n"
        ::"r"(src),"r"(stride),"r"(ff_pb_1)
        : "$2","$3","$4","$5","$6","memory"
    );
}

void ff_pred16x16_dc_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "daddiu $2, %0, -1                  \r\n"
        "dli $6, 0x10                       \r\n"
        "xor $8, $8, $8                     \r\n"
        "1:                                 \r\n"
        "lbu $4, 0($2)                      \r\n"
        "daddu $8, $8, $4                   \r\n"
        "daddu $2, $2, %1                   \r\n"
        "daddiu $6, $6, -1                  \r\n"
        "bnez $6, 1b                        \r\n"
        "dli $6, 0x10                       \r\n"
        "negu $3, %1                        \r\n"
        "daddu $2, %0, $3                   \r\n"
        "2:                                 \r\n"
        "lbu $4, 0($2)                      \r\n"
        "daddu $8, $8, $4                   \r\n"
        "daddiu $2, $2, 1                   \r\n"
        "daddiu $6, $6, -1                  \r\n"
        "bnez $6, 2b                        \r\n"
        "daddiu $8, $8, 0x10                \r\n"
        "dsra $8, 5                         \r\n"
        "dmul $5, $8, %2                    \r\n"
        "daddu $2, %0, $0                   \r\n"
        "dli $6, 0x10                       \r\n"
        "3:                                 \r\n"
        "sdl $5, 7($2)                      \r\n"
        "sdr $5, 0($2)                      \r\n"
        "sdl $5, 15($2)                     \r\n"
        "sdr $5, 8($2)                      \r\n"
        "daddu $2, $2, %1                   \r\n"
        "daddiu $6, $6, -1                  \r\n"
        "bnez $6, 3b                        \r\n"
        ::"r"(src),"r"(stride),"r"(ff_pb_1)
        : "$2","$3","$4","$5","$6","$8","memory"
    );
}

void ff_pred8x8l_top_dc_8_mmi(uint8_t *src, int has_topleft,
        int has_topright, ptrdiff_t stride)
{
    int y;
    uint32_t dc;

    __asm__ volatile (
        "ldl $8, 7(%1)                      \r\n"
        "ldr $8, 0(%1)                      \r\n"
        "ldl $9, 7(%2)                      \r\n"
        "ldr $9, 0(%2)                      \r\n"
        "ldl $10, 7(%3)                     \r\n"
        "ldr $10, 0(%3)                     \r\n"
        "dmtc1 $8, $f2                      \r\n"
        "dmtc1 $9, $f4                      \r\n"
        "dmtc1 $10, $f6                     \r\n"
        "dmtc1 $0, $f0                      \r\n"
        "punpcklbh $f8, $f2, $f0            \r\n"
        "punpckhbh $f10, $f2, $f0           \r\n"
        "punpcklbh $f12, $f4, $f0           \r\n"
        "punpckhbh $f14, $f4, $f0           \r\n"
        "punpcklbh $f16, $f6, $f0           \r\n"
        "punpckhbh $f18, $f6, $f0           \r\n"
        "bnez %4, 1f                        \r\n"
        "pinsrh_0 $f8, $f8, $f12            \r\n"
        "1:                                 \r\n"
        "bnez %5, 2f                        \r\n"
        "pinsrh_3 $f18, $f18, $f14          \r\n"
        "2:                                 \r\n"
        "daddiu $8, $0, 2                   \r\n"
        "dmtc1 $8, $f20                     \r\n"
        "pshufh $f22, $f20, $f0             \r\n"
        "pmullh $f12, $f12, $f22            \r\n"
        "pmullh $f14, $f14, $f22            \r\n"
        "paddh $f8, $f8, $f12               \r\n"
        "paddh $f10, $f10, $f14             \r\n"
        "paddh $f8, $f8, $f16               \r\n"
        "paddh $f10, $f10, $f18             \r\n"
        "paddh $f8, $f8, $f22               \r\n"
        "paddh $f10, $f10, $f22             \r\n"
        "psrah $f8, $f8, $f20               \r\n"
        "psrah $f10, $f10, $f20             \r\n"
        "packushb $f4, $f8, $f10            \r\n"
        "biadd $f2, $f4                     \r\n"
        "mfc1 $9, $f2                       \r\n"
        "addiu $9, $9, 4                    \r\n"
        "dsrl $9, $9, 3                     \r\n"
        "li $8, 0x01010101                  \r\n"
        "mul %0, $9, $8                     \r\n"
        : "=r"(dc)
        : "r"(src-stride-1),"r"(src-stride),"r"(src-stride+1),
          "r"(has_topleft),"r"(has_topright)
        : "$8","$9","$10"
    );

    for (y=0; y<8; y++) {
        AV_WN4PA(((uint32_t*)src)+0, dc);
        AV_WN4PA(((uint32_t*)src)+1, dc);
        src += stride;
    }
}

void ff_pred8x8l_dc_8_mmi(uint8_t *src, int has_topleft,
        int has_topright, ptrdiff_t stride)
{
    int y;
    uint32_t dc, dc1, dc2;

    const int l0 = ((has_topleft ? src[-1+-1*stride] : src[-1+0*stride]) + 2*src[-1+0*stride] + src[-1+1*stride] + 2) >> 2;
    const int l1 = (src[-1+0*stride] + 2*src[-1+1*stride] + src[-1+2*stride] + 2) >> 2;
    const int l2 = (src[-1+1*stride] + 2*src[-1+2*stride] + src[-1+3*stride] + 2) >> 2;
    const int l3 = (src[-1+2*stride] + 2*src[-1+3*stride] + src[-1+4*stride] + 2) >> 2;
    const int l4 = (src[-1+3*stride] + 2*src[-1+4*stride] + src[-1+5*stride] + 2) >> 2;
    const int l5 = (src[-1+4*stride] + 2*src[-1+5*stride] + src[-1+6*stride] + 2) >> 2;
    const int l6 = (src[-1+5*stride] + 2*src[-1+6*stride] + src[-1+7*stride] + 2) >> 2;
    const int l7 = (src[-1+6*stride] + 2*src[-1+7*stride] + src[-1+7*stride] + 2) >> 2;

    __asm__ volatile (
        "ldl $8, 7(%1)                      \r\n"
        "ldr $8, 0(%1)                      \r\n"
        "ldl $9, 7(%2)                      \r\n"
        "ldr $9, 0(%2)                      \r\n"
        "ldl $10, 7(%3)                     \r\n"
        "ldr $10, 0(%3)                     \r\n"
        "dmtc1 $8, $f2                      \r\n"
        "dmtc1 $9, $f4                      \r\n"
        "dmtc1 $10, $f6                     \r\n"
        "dmtc1 $0, $f0                      \r\n"
        "punpcklbh $f8, $f2, $f0            \r\n"
        "punpckhbh $f10, $f2, $f0           \r\n"
        "punpcklbh $f12, $f4, $f0           \r\n"
        "punpckhbh $f14, $f4, $f0           \r\n"
        "punpcklbh $f16, $f6, $f0           \r\n"
        "punpckhbh $f18, $f6, $f0           \r\n"
        "daddiu $8, $0, 3                   \r\n"
        "dmtc1 $8, $f20                     \r\n"
        "pshufh $f28, $f10, $f20            \r\n"
        "pshufh $f30, $f18, $f20            \r\n"
        "pinsrh_3 $f10, $f10, $f30          \r\n"
        "pinsrh_3 $f18, $f18, $f28          \r\n"
        "bnez %4, 1f                        \r\n"
        "pinsrh_0 $f8, $f8, $f12            \r\n"
        "1:                                 \r\n"
        "bnez %5, 2f                        \r\n"
        "pshufh $f30, $f14, $f20            \r\n"
        "pinsrh_3 $f10, $f10, $f30          \r\n"
        "2:                                 \r\n"
        "daddiu $8, $0, 2                   \r\n"
        "dmtc1 $8, $f20                     \r\n"
        "pshufh $f22, $f20, $f0             \r\n"
        "pmullh $f12, $f12, $f22            \r\n"
        "pmullh $f14, $f14, $f22            \r\n"
        "paddh $f8, $f8, $f12               \r\n"
        "paddh $f10, $f10, $f14             \r\n"
        "paddh $f8, $f8, $f16               \r\n"
        "paddh $f10, $f10, $f18             \r\n"
        "paddh $f8, $f8, $f22               \r\n"
        "paddh $f10, $f10, $f22             \r\n"
        "psrah $f8, $f8, $f20               \r\n"
        "psrah $f10, $f10, $f20             \r\n"
        "packushb $f4, $f8, $f10            \r\n"
        "biadd $f2, $f4                     \r\n"
        "mfc1 %0, $f2                       \r\n"
        : "=r"(dc2)
        : "r"(src-stride-1),"r"(src-stride),"r"(src-stride+1),
          "r"(has_topleft),"r"(has_topright)
        : "$8","$9","$10"
    );

    dc1 = l0+l1+l2+l3+l4+l5+l6+l7;
    dc = PIXEL_SPLAT_X4((dc1+dc2+8)>>4);

    for (y=0; y<8; y++) {
        AV_WN4PA(((uint32_t*)src)+0, dc);
        AV_WN4PA(((uint32_t*)src)+1, dc);
        src += stride;
    }
}

void ff_pred8x8l_horizontal_8_mmi(uint8_t *src, int has_topleft,
        int has_topright, ptrdiff_t stride)
{
    const int l0 = ((has_topleft ? src[-1+-1*stride] : src[-1+0*stride]) + 2*src[-1+0*stride] + src[-1+1*stride] + 2) >> 2;
    const int l1 = (src[-1+0*stride] + 2*src[-1+1*stride] + src[-1+2*stride] + 2) >> 2;
    const int l2 = (src[-1+1*stride] + 2*src[-1+2*stride] + src[-1+3*stride] + 2) >> 2;
    const int l3 = (src[-1+2*stride] + 2*src[-1+3*stride] + src[-1+4*stride] + 2) >> 2;
    const int l4 = (src[-1+3*stride] + 2*src[-1+4*stride] + src[-1+5*stride] + 2) >> 2;
    const int l5 = (src[-1+4*stride] + 2*src[-1+5*stride] + src[-1+6*stride] + 2) >> 2;
    const int l6 = (src[-1+5*stride] + 2*src[-1+6*stride] + src[-1+7*stride] + 2) >> 2;
    const int l7 = (src[-1+6*stride] + 2*src[-1+7*stride] + src[-1+7*stride] + 2) >> 2;

    AV_WN4PA(src+0*stride, PIXEL_SPLAT_X4(l0));
    AV_WN4PA(src+0*stride+4, PIXEL_SPLAT_X4(l0));
    AV_WN4PA(src+1*stride, PIXEL_SPLAT_X4(l1));
    AV_WN4PA(src+1*stride+4, PIXEL_SPLAT_X4(l1));
    AV_WN4PA(src+2*stride, PIXEL_SPLAT_X4(l2));
    AV_WN4PA(src+2*stride+4, PIXEL_SPLAT_X4(l2));
    AV_WN4PA(src+3*stride, PIXEL_SPLAT_X4(l3));
    AV_WN4PA(src+3*stride+4, PIXEL_SPLAT_X4(l3));
    AV_WN4PA(src+4*stride, PIXEL_SPLAT_X4(l4));
    AV_WN4PA(src+4*stride+4, PIXEL_SPLAT_X4(l4));
    AV_WN4PA(src+5*stride, PIXEL_SPLAT_X4(l5));
    AV_WN4PA(src+5*stride+4, PIXEL_SPLAT_X4(l5));
    AV_WN4PA(src+6*stride, PIXEL_SPLAT_X4(l6));
    AV_WN4PA(src+6*stride+4, PIXEL_SPLAT_X4(l6));
    AV_WN4PA(src+7*stride, PIXEL_SPLAT_X4(l7));
    AV_WN4PA(src+7*stride+4, PIXEL_SPLAT_X4(l7));
}

void ff_pred8x8l_vertical_8_mmi(uint8_t *src, int has_topleft,
        int has_topright, ptrdiff_t stride)
{
    int y;
    uint32_t a, b;

    __asm__ volatile (
        "ldl $8, 7(%1)                      \r\n"
        "ldr $8, 0(%1)                      \r\n"
        "ldl $9, 7(%2)                      \r\n"
        "ldr $9, 0(%2)                      \r\n"
        "ldl $10, 7(%3)                     \r\n"
        "ldr $10, 0(%3)                     \r\n"
        "dmtc1 $8, $f2                      \r\n"
        "dmtc1 $9, $f4                      \r\n"
        "dmtc1 $10, $f6                     \r\n"
        "dmtc1 $0, $f0                      \r\n"
        "punpcklbh $f8, $f2, $f0            \r\n"
        "punpckhbh $f10, $f2, $f0           \r\n"
        "punpcklbh $f12, $f4, $f0           \r\n"
        "punpckhbh $f14, $f4, $f0           \r\n"
        "punpcklbh $f16, $f6, $f0           \r\n"
        "punpckhbh $f18, $f6, $f0           \r\n"
        "bnez %4, 1f                        \r\n"
        "pinsrh_0 $f8, $f8, $f12            \r\n"
        "1:                                 \r\n"
        "bnez %5, 2f                        \r\n"
        "pinsrh_3 $f18, $f18, $f14          \r\n"
        "2:                                 \r\n"
        "daddiu $8, $0, 2                   \r\n"
        "dmtc1 $8, $f20                     \r\n"
        "pshufh $f22, $f20, $f0             \r\n"
        "pmullh $f12, $f12, $f22            \r\n"
        "pmullh $f14, $f14, $f22            \r\n"
        "paddh $f8, $f8, $f12               \r\n"
        "paddh $f10, $f10, $f14             \r\n"
        "paddh $f8, $f8, $f16               \r\n"
        "paddh $f10, $f10, $f18             \r\n"
        "paddh $f8, $f8, $f22               \r\n"
        "paddh $f10, $f10, $f22             \r\n"
        "psrah $f8, $f8, $f20               \r\n"
        "psrah $f10, $f10, $f20             \r\n"
        "packushb $f4, $f8, $f10            \r\n"
        "sdc1 $f4, %0                       \r\n"
        : "=m"(*src)
        : "r"(src-stride-1),"r"(src-stride),"r"(src-stride+1),
          "r"(has_topleft),"r"(has_topright)
        : "$8","$9","$10"
    );

    a = AV_RN4PA(((uint32_t*)src)+0);
    b = AV_RN4PA(((uint32_t*)src)+1);

    for (y=1; y<8; y++) {
        AV_WN4PA(((uint32_t*)(src+y*stride))+0, a);
        AV_WN4PA(((uint32_t*)(src+y*stride))+1, b);
    }
}

void ff_pred4x4_dc_8_mmi(uint8_t *src, const uint8_t *topright,
        ptrdiff_t stride)
{
    const int dc = (src[-stride] + src[1-stride] + src[2-stride]
                 + src[3-stride] + src[-1+0*stride] + src[-1+1*stride]
                 + src[-1+2*stride] + src[-1+3*stride] + 4) >>3;

    __asm__ volatile (
        "daddu $2, %2, $0                   \r\n"
        "dmul $3, $2, %3                    \r\n"
        "xor $4, $4, $4                     \r\n"
        "gsswx $3, 0(%0,$4)                 \r\n"
        "daddu $4, %1                       \r\n"
        "gsswx $3, 0(%0,$4)                 \r\n"
        "daddu $4, %1                       \r\n"
        "gsswx $3, 0(%0,$4)                 \r\n"
        "daddu $4, %1                       \r\n"
        "gsswx $3, 0(%0,$4)                 \r\n"
        ::"r"(src),"r"(stride),"r"(dc),"r"(ff_pb_1)
        : "$2","$3","$4","memory"
    );
}

void ff_pred8x8_vertical_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "dsubu $2, %0, %1                   \r\n"
        "daddu $3, %0, $0                   \r\n"
        "ldl $4, 7($2)                      \r\n"
        "ldr $4, 0($2)                      \r\n"
        "dli $5, 0x8                        \r\n"
        "1:                                 \r\n"
        "sdl $4, 7($3)                      \r\n"
        "sdr $4, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "daddiu $5, -1                      \r\n"
        "bnez $5, 1b                        \r\n"
        ::"r"(src),"r"(stride)
        : "$2","$3","$4","$5","memory"
    );
}

void ff_pred8x8_horizontal_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "daddiu $2, %0, -1                  \r\n"
        "daddu $3, %0, $0                   \r\n"
        "dli $6, 0x8                        \r\n"
        "1:                                 \r\n"
        "lbu $4, 0($2)                      \r\n"
        "dmul $5, $4, %2                    \r\n"
        "sdl $5, 7($3)                      \r\n"
        "sdr $5, 0($3)                      \r\n"
        "daddu $2, %1                       \r\n"
        "daddu $3, %1                       \r\n"
        "daddiu $6, -1                      \r\n"
        "bnez $6, 1b                        \r\n"
        ::"r"(src),"r"(stride),"r"(ff_pb_1)
        : "$2","$3","$4","$5","$6","memory"
    );
}

static void ff_pred16x16_plane_compat_8_mmi(uint8_t *src, ptrdiff_t stride,
        const int svq3, const int rv40)
{
    __asm__ volatile (
        "negu $2, %1                        \r\n"
        "daddu $3, %0, $2                   \r\n"
        "xor $f8, $f8, $f8                  \r\n"
        "gslwlc1 $f0, 2($3)                 \r\n"
        "gslwrc1 $f0, -1($3)                \r\n"
        "gslwlc1 $f2, 6($3)                 \r\n"
        "gslwrc1 $f2, 3($3)                 \r\n"
        "gslwlc1 $f4, 11($3)                \r\n"
        "gslwrc1 $f4, 8($3)                 \r\n"
        "gslwlc1 $f6, 15($3)                \r\n"
        "gslwrc1 $f6, 12($3)                \r\n"
        "punpcklbh $f0, $f0, $f8            \r\n"
        "punpcklbh $f2, $f2, $f8            \r\n"
        "punpcklbh $f4, $f4, $f8            \r\n"
        "punpcklbh $f6, $f6, $f8            \r\n"
        "dmtc1 %4, $f20                     \r\n"
        "dmtc1 %5, $f22                     \r\n"
        "dmtc1 %6, $f24                     \r\n"
        "dmtc1 %7, $f26                     \r\n"
        "pmullh $f0, $f0, $f20              \r\n"
        "pmullh $f2, $f2, $f22              \r\n"
        "pmullh $f4, $f4, $f24              \r\n"
        "pmullh $f6, $f6, $f26              \r\n"
        "paddsh $f0, $f0, $f4               \r\n"
        "paddsh $f2, $f2, $f6               \r\n"
        "paddsh $f0, $f0, $f2               \r\n"
        "dli $4, 0xE                        \r\n"
        "dmtc1 $4, $f28                     \r\n"
        "pshufh $f2, $f0, $f28              \r\n"
        "paddsh $f0, $f0, $f2               \r\n"
        "dli $4, 0x1                        \r\n"
        "dmtc1 $4, $f30                     \r\n"
        "pshufh $f2, $f0, $f30              \r\n"
        "paddsh $f10, $f0, $f2              \r\n"
        "daddiu $3, %0, -1                  \r\n"
        "daddu $3, $2                       \r\n"
        "lbu $4, 0($3)                      \r\n"
        "lbu $8, 16($3)                     \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $5, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $6, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $7, 0($3)                      \r\n"
        "dsll $5, 16                        \r\n"
        "dsll $6, 32                        \r\n"
        "dsll $7, 48                        \r\n"
        "or $6, $7                          \r\n"
        "or $4, $5                          \r\n"
        "or $4, $6                          \r\n"
        "dmtc1 $4, $f0                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $4, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $5, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $6, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $7, 0($3)                      \r\n"
        "dsll $5, 16                        \r\n"
        "dsll $6, 32                        \r\n"
        "dsll $7, 48                        \r\n"
        "or $6, $7                          \r\n"
        "or $4, $5                          \r\n"
        "or $4, $6                          \r\n"
        "dmtc1 $4, $f2                      \r\n"
        "daddu $3, %1                       \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $4, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $5, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $6, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $7, 0($3)                      \r\n"
        "dsll $5, 16                        \r\n"
        "dsll $6, 32                        \r\n"
        "dsll $7, 48                        \r\n"
        "or $6, $7                          \r\n"
        "or $4, $5                          \r\n"
        "or $4, $6                          \r\n"
        "dmtc1 $4, $f4                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $4, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $5, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $6, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "lbu $7, 0($3)                      \r\n"
        "daddu $8, $7                       \r\n"
        "daddiu $8, 1                       \r\n"
        "dsll $8, 4                         \r\n"
        "dsll $5, 16                        \r\n"
        "dsll $6, 32                        \r\n"
        "dsll $7, 48                        \r\n"
        "or $6, $7                          \r\n"
        "or $4, $5                          \r\n"
        "or $4, $6                          \r\n"
        "dmtc1 $4, $f6                      \r\n"
        "pmullh $f0, $f0, $f20              \r\n"
        "pmullh $f2, $f2, $f22              \r\n"
        "pmullh $f4, $f4, $f24              \r\n"
        "pmullh $f6, $f6, $f26              \r\n"
        "paddsh $f0, $f0, $f4               \r\n"
        "paddsh $f2, $f2, $f6               \r\n"
        "paddsh $f0, $f0, $f2               \r\n"
        "pshufh $f2, $f0, $f28              \r\n"
        "paddsh $f0, $f0, $f2               \r\n"
        "pshufh $f2, $f0, $f30              \r\n"
        "paddsh $f12, $f0, $f2              \r\n"
        "dmfc1 $2, $f10                     \r\n"
        "dsll $2, 48                        \r\n"
        "dsra $2, 48                        \r\n"
        "dmfc1 $3, $f12                     \r\n"
        "dsll $3, 48                        \r\n"
        "dsra $3, 48                        \r\n"
        "beqz %2, 1f                        \r\n"
        "dli $4, 4                          \r\n"
        "ddiv $2, $4                        \r\n"
        "ddiv $3, $4                        \r\n"
        "dli $4, 5                          \r\n"
        "dmul $2, $4                        \r\n"
        "dmul $3, $4                        \r\n"
        "dli $4, 16                         \r\n"
        "ddiv $2, $4                        \r\n"
        "ddiv $3, $4                        \r\n"
        "daddu $4, $2, $0                   \r\n"
        "daddu $2, $3, $0                   \r\n"
        "daddu $3, $4, $0                   \r\n"
        "b 2f                               \r\n"
        "1:                                 \r\n"
        "beqz %3, 1f                        \r\n"
        "dsra $4, $2, 2                     \r\n"
        "daddu $2, $4                       \r\n"
        "dsra $4, $3, 2                     \r\n"
        "daddu $3, $4                       \r\n"
        "dsra $2, 4                         \r\n"
        "dsra $3, 4                         \r\n"
        "b 2f                               \r\n"
        "1:                                 \r\n"
        "dli $4, 5                          \r\n"
        "dmul $2, $4                        \r\n"
        "dmul $3, $4                        \r\n"
        "daddiu $2, 32                      \r\n"
        "daddiu $3, 32                      \r\n"
        "dsra $2, 6                         \r\n"
        "dsra $3, 6                         \r\n"
        "2:                                 \r\n"
        "daddu $5, $2, $3                   \r\n"
        "dli $4, 7                          \r\n"
        "dmul $5, $4                        \r\n"
        "dsubu $8, $5                       \r\n"
        "dmtc1 $0, $f8                      \r\n"
        "dmtc1 $2, $f0                      \r\n"
        "pshufh $f0, $f0, $f8               \r\n"
        "dmtc1 $3, $f10                     \r\n"
        "pshufh $f10, $f10, $f8             \r\n"
        "dmtc1 $8, $f12                     \r\n"
        "pshufh $f12, $f12, $f8             \r\n"
        "dli $4, 5                          \r\n"
        "dmtc1 $4, $f14                     \r\n"
        "dmtc1 %8, $f2                      \r\n"
        "pmullh $f2, $f2, $f0               \r\n"
        "dmtc1 %9, $f4                      \r\n"
        "pmullh $f4, $f4, $f0               \r\n"
        "dmtc1 %10, $f6                      \r\n"
        "pmullh $f6, $f6, $f0               \r\n"
        "dmtc1 %11, $f8                      \r\n"
        "pmullh $f8, $f8, $f0               \r\n"
        "daddu $3, %0, $0                   \r\n"
        "dli $2, 16                         \r\n"
        "1:                                 \r\n"
        "paddsh $f16, $f2, $f12             \r\n"
        "psrah $f16, $f16, $f14             \r\n"
        "paddsh $f18, $f4, $f12             \r\n"
        "psrah $f18, $f18, $f14             \r\n"
        "packushb $f20, $f16, $f18          \r\n"
        "gssdlc1 $f20, 7($3)                \r\n"
        "gssdrc1 $f20, 0($3)                \r\n"
        "paddsh $f16, $f6, $f12             \r\n"
        "psrah $f16, $f16, $f14             \r\n"
        "paddsh $f18, $f8, $f12             \r\n"
        "psrah $f18, $f18, $f14             \r\n"
        "packushb $f20, $f16, $f18          \r\n"
        "gssdlc1 $f20, 15($3)               \r\n"
        "gssdrc1 $f20, 8($3)                \r\n"
        "paddsh $f12, $f12, $f10            \r\n"
        "daddu $3, %1                       \r\n"
        "daddiu $2, -1                      \r\n"
        "bnez $2, 1b                        \r\n"
        ::"r"(src),"r"(stride),"r"(svq3),"r"(rv40),
          "r"(ff_pw_m8tom5),"r"(ff_pw_m4tom1),"r"(ff_pw_1to4),"r"(ff_pw_5to8),
          "r"(ff_pw_0to3),"r"(ff_pw_4to7),"r"(ff_pw_8tob),"r"(ff_pw_ctof)
        : "$2","$3","$4","$5","$6","$7","$8","memory"
    );
}

void ff_pred16x16_plane_svq3_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    ff_pred16x16_plane_compat_8_mmi(src, stride, 1, 0);
}

void ff_pred16x16_plane_rv40_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    ff_pred16x16_plane_compat_8_mmi(src, stride, 0, 1);
}

void ff_pred16x16_plane_h264_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    ff_pred16x16_plane_compat_8_mmi(src, stride, 0, 0);
}

void ff_pred8x8_top_dc_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "dli $2, 2                          \r\n"
        "xor $f0, $f0, $f0                  \r\n"
        "xor $f2, $f2, $f2                  \r\n"
        "xor $f30, $f30, $f30               \r\n"
        "negu $3, %1                        \r\n"
        "daddu $3, $3, %0                   \r\n"
        "gsldlc1 $f4, 7($3)                 \r\n"
        "gsldrc1 $f4, 0($3)                 \r\n"
        "punpcklbh $f0, $f4, $f30           \r\n"
        "punpckhbh $f2, $f4, $f30           \r\n"
        "biadd $f0, $f0                     \r\n"
        "biadd $f2, $f2                     \r\n"
        "pshufh $f0, $f0, $f30              \r\n"
        "pshufh $f2, $f2, $f30              \r\n"
        "dmtc1 $2, $f4                      \r\n"
        "pshufh $f4, $f4, $f30              \r\n"
        "paddush $f0, $f0, $f4              \r\n"
        "paddush $f2, $f2, $f4              \r\n"
        "dmtc1 $2, $f4                      \r\n"
        "psrlh $f0, $f0, $f4                \r\n"
        "psrlh $f2, $f2, $f4                \r\n"
        "packushb $f4, $f0, $f2             \r\n"
        "dli $2, 8                          \r\n"
        "1:                                 \r\n"
        "gssdlc1 $f4, 7(%0)                 \r\n"
        "gssdrc1 $f4, 0(%0)                 \r\n"
        "daddu %0, %0, %1                   \r\n"
        "daddiu $2, $2, -1                  \r\n"
        "bnez $2, 1b                        \r\n"
        ::"r"(src),"r"(stride)
        : "$2","$3","memory"
    );
}

void ff_pred8x8_dc_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "negu $2, %1                        \r\n"
        "daddu $2, $2, %0                   \r\n"
        "daddiu $5, $2, 4                   \r\n"
        "lbu $6, 0($2)                      \r\n"
        "daddu $3, $0, $6                   \r\n"
        "daddiu $2, 1                       \r\n"
        "lbu $6, 0($5)                      \r\n"
        "daddu $4, $0, $6                   \r\n"
        "daddiu $5, 1                       \r\n"
        "lbu $6, 0($2)                      \r\n"
        "daddu $3, $3, $6                   \r\n"
        "daddiu $2, 1                       \r\n"
        "lbu $6, 0($5)                      \r\n"
        "daddu $4, $4, $6                   \r\n"
        "daddiu $5, 1                       \r\n"
        "lbu $6, 0($2)                      \r\n"
        "daddu $3, $3, $6                   \r\n"
        "daddiu $2, 1                       \r\n"
        "lbu $6, 0($5)                      \r\n"
        "daddu $4, $4, $6                   \r\n"
        "daddiu $5, 1                       \r\n"
        "lbu $6, 0($2)                      \r\n"
        "daddu $3, $3, $6                   \r\n"
        "daddiu $2, 1                       \r\n"
        "lbu $6, 0($5)                      \r\n"
        "daddu $4, $4, $6                   \r\n"
        "daddiu $5, 1                       \r\n"
        "dli $6, -1                         \r\n"
        "daddu $6, $6, %0                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $7, $0, $5                   \r\n"
        "daddu $6, $6, %1                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $7, $7, $5                   \r\n"
        "daddu $6, $6, %1                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $7, $7, $5                   \r\n"
        "daddu $6, $6, %1                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $7, $7, $5                   \r\n"
        "daddu $6, $6, %1                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $8, $0, $5                   \r\n"
        "daddu $6, $6, %1                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $8, $8, $5                   \r\n"
        "daddu $6, $6, %1                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $8, $8, $5                   \r\n"
        "daddu $6, $6, %1                   \r\n"
        "lbu $5, 0($6)                      \r\n"
        "daddu $8, $8, $5                   \r\n"
        "daddu $3, $3, $7                   \r\n"
        "daddiu $3, $3, 4                   \r\n"
        "daddiu $4, $4, 2                   \r\n"
        "daddiu $5, $8, 2                   \r\n"
        "daddu $6, $4, $5                   \r\n"
        "dsrl $3, 3                         \r\n"
        "dsrl $4, 2                         \r\n"
        "dsrl $5, 2                         \r\n"
        "dsrl $6, 3                         \r\n"
        "xor $f30, $f30, $f30               \r\n"
        "dmtc1 $3, $f0                      \r\n"
        "pshufh $f0, $f0, $f30              \r\n"
        "dmtc1 $4, $f2                      \r\n"
        "pshufh $f2, $f2, $f30              \r\n"
        "dmtc1 $5, $f4                      \r\n"
        "pshufh $f4, $f4, $f30              \r\n"
        "dmtc1 $6, $f6                      \r\n"
        "pshufh $f6, $f6, $f30              \r\n"
        "packushb $f0, $f0, $f2             \r\n"
        "packushb $f2, $f4, $f6             \r\n"
        "daddu $2, $0, %0                   \r\n"
        "sdc1 $f0, 0($2)                    \r\n"
        "daddu $2, $2, %1                   \r\n"
        "sdc1 $f0, 0($2)                    \r\n"
        "daddu $2, $2, %1                   \r\n"
        "sdc1 $f0, 0($2)                    \r\n"
        "daddu $2, $2, %1                   \r\n"
        "sdc1 $f0, 0($2)                    \r\n"
        "daddu $2, $2, %1                   \r\n"
        "sdc1 $f2, 0($2)                    \r\n"
        "daddu $2, $2, %1                   \r\n"
        "sdc1 $f2, 0($2)                    \r\n"
        "daddu $2, $2, %1                   \r\n"
        "sdc1 $f2, 0($2)                    \r\n"
        "daddu $2, $2, %1                   \r\n"
        "sdc1 $f2, 0($2)                    \r\n"
        ::"r"(src),"r"(stride)
        :"$2","$3","$4","$5","$6","$7","$8","memory"
    );
}

void ff_pred8x16_vertical_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "dsubu $2, %0, %1                   \r\n"
        "daddu $3, %0, $0                   \r\n"
        "ldl $4, 7($2)                      \r\n"
        "ldr $4, 0($2)                      \r\n"
        "dli $5, 0x10                       \r\n"
        "1:                                 \r\n"
        "sdl $4, 7($3)                      \r\n"
        "sdr $4, 0($3)                      \r\n"
        "daddu $3, %1                       \r\n"
        "daddiu $5, -1                      \r\n"
        "bnez $5, 1b                        \r\n"
        ::"r"(src),"r"(stride)
        : "$2","$3","$4","$5","memory"
    );
}

void ff_pred8x16_horizontal_8_mmi(uint8_t *src, ptrdiff_t stride)
{
    __asm__ volatile (
        "daddiu $2, %0, -1                  \r\n"
        "daddu $3, %0, $0                   \r\n"
        "dli $6, 0x10                       \r\n"
        "1:                                 \r\n"
        "lbu $4, 0($2)                      \r\n"
        "dmul $5, $4, %2                    \r\n"
        "sdl $5, 7($3)                      \r\n"
        "sdr $5, 0($3)                      \r\n"
        "daddu $2, %1                       \r\n"
        "daddu $3, %1                       \r\n"
        "daddiu $6, -1                      \r\n"
        "bnez $6, 1b                        \r\n"
        ::"r"(src),"r"(stride),"r"(ff_pb_1)
        : "$2","$3","$4","$5","$6","memory"
    );
}
