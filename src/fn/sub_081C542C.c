#include "global.h"
#define GameState_GetPackedTripletValue sub_081C542C
#define gGameStateBytes gUnk_02000710
extern u8 *gUnk_02000710;
s32 sub_081C542C(s32 a, s32 b)
{
    s32 i = a * 3 + b;
    s32 h = i / 2;
    s32 odd = i & 1;
    u8 *q = gUnk_02000710 + 0x658 + h;
    if (odd) {
        s32 value = *q;
        if (value >= 0) return value >> 4;
        return -((-value) >> 4);
    }
    return *q & 15;
}
