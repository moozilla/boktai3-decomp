// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"

extern s16 gUnk_03004C10;
extern s16 gUnk_03004BE8;
extern const s16 gUnk_086149C4[];

s32 __divsi3(s32, s32);

static inline s16 SignedScaleQuotient(s32 dividend, s16 scale)
{
    return __divsi3(dividend, scale);
}

/* The 8.8 reciprocal is narrowed before its matrix multiplication. */
static inline s16 FixedDivide(s32 numerator, s16 scale)
{
    register s32 dividend asm("r0") = numerator << 8;
    return SignedScaleQuotient(dividend, scale);
}
/* Division by 256 rounds negative products toward zero. */
static inline s16 FixedMultiply(s32 a, s32 b)
{
    s32 product = a * b;
    product /= 256;
    return product;
}

void Bg_SetAffineTransform(s32 x, s32 y, s32 angle, u16 scaleX, s32 scaleY)
{
    gUnk_03004C10 = x;
    gUnk_03004BE8 = y;
    gBgAffineAngle = angle;
    gBgAffineScaleX = scaleX;
    gBgAffineScaleY = scaleY;
    /* The table uses 12 fractional bits; the affine matrix uses eight. */
    gBgAffineMatrix[0] = FixedMultiply((gUnk_086149C4[(gBgAffineAngle + 64) & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleX));
    gBgAffineMatrix[1] = FixedMultiply((gUnk_086149C4[gBgAffineAngle & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleX));
    gBgAffineMatrix[2] = FixedMultiply((-gUnk_086149C4[gBgAffineAngle & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleY));
    gBgAffineMatrix[3] = FixedMultiply((gUnk_086149C4[(gBgAffineAngle + 64) & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleY));
    gBgAffineRefX =
        ((120 - gUnk_03004C10) << 8) - 120 * gBgAffineMatrix[0] - 80 * gBgAffineMatrix[1];
    gBgAffineRefY =
        ((120 - gUnk_03004BE8) << 8) - 120 * gBgAffineMatrix[2] - 80 * gBgAffineMatrix[3];
}
