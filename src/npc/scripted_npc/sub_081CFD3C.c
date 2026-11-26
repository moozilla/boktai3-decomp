#include "global.h"
extern const s16 gUnk_086149C4[];
void sub_081CFD3C(s32 *out, s32 angle, s32 radius)
{
    out[0] = gUnk_086149C4[(angle + 0x40) & 0xff] * radius;
    out[1] = gUnk_086149C4[angle & 0xff] * radius;
}
