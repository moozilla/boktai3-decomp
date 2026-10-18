#include "global.h"
#define Math_GetVectorLength sub_0812DF40
#define gVectorSineTable gUnk_086149C4
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
s16 sub_0812DF40(s16 x, s16 y)
{
    u8 angle = Math_Atan2(x, y);
    s32 result;
    if (angle == 64 || angle == 192) {
        s32 vertical = y;
        if (vertical < 0) vertical = -vertical;
        return vertical;
    } else {
        result = Div(x << 12, gUnk_086149C4[(angle + 64) & 255]);
        if (result < 0) result = -result;
    }
    return result;
}
