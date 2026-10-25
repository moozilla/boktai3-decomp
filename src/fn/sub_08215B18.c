#include "global.h"

extern s16 gUnk_03004C10;
extern s16 gUnk_03004BE8;
extern u16 gUnk_03004BB0;
extern u16 gUnk_03004C14;
extern u16 gUnk_03004BFC;
extern s16 gUnk_03004BD0[4];
extern s32 gUnk_03004BF8;
extern s32 gUnk_03004C00;
extern const s16 gUnk_086149C4[];

s32 __divsi3(s32, s32);

/* Signed 8.8 division; the reciprocal is narrowed before multiplication. */
static inline s16 FixedDivide(s32 numerator, s16 denominator)
{
    return __divsi3(numerator << 8, denominator);
}

/* Division by 256 rounds negative products toward zero. */
static inline s16 FixedMultiply(s32 a, s32 b)
{
    s32 product = a * b;
    return product / 256;
}

#ifdef NONMATCHING
// fuzzy: 79.9%
// scope shape 22 (differences once low registers are set aside)
void Bg_SetAffineTransform(s32 x, s32 y, s32 angle, u16 scaleX, s32 scaleY)
{
    gUnk_03004C10 = x;
    gUnk_03004BE8 = y;
    gUnk_03004BB0 = angle;
    gUnk_03004C14 = scaleX;
    gUnk_03004BFC = scaleY;
    /* The table uses 12 fractional bits; the affine matrix uses eight. */
    gUnk_03004BD0[0] = FixedMultiply((gUnk_086149C4[(gUnk_03004BB0 + 64) & 255] >> 4),
                                     FixedDivide(256, gUnk_03004C14));
    gUnk_03004BD0[1] =
        FixedMultiply((gUnk_086149C4[gUnk_03004BB0 & 255] >> 4), FixedDivide(256, gUnk_03004C14));
    gUnk_03004BD0[2] =
        FixedMultiply((-gUnk_086149C4[gUnk_03004BB0 & 255] >> 4), FixedDivide(256, gUnk_03004BFC));
    gUnk_03004BD0[3] = FixedMultiply((gUnk_086149C4[(gUnk_03004BB0 + 64) & 255] >> 4),
                                     FixedDivide(256, gUnk_03004BFC));
    gUnk_03004BF8 = ((120 - gUnk_03004C10) << 8) - 120 * gUnk_03004BD0[0] - 80 * gUnk_03004BD0[1];
    gUnk_03004C00 = ((120 - gUnk_03004BE8) << 8) - 120 * gUnk_03004BD0[2] - 80 * gUnk_03004BD0[3];
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08215B18);
#endif
