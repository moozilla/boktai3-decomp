#include "global.h"

#define SceneInventory_FindSlotByValue sub_080B0E04

s32 sub_0822CCC8(s32 i);
s32 sub_0822CDA8(u32);

s32 sub_080B0E04(s16 *p0, s16 *p1, s16 *p2, s32 p3)
{
    s16 i;
    for (i = 0; i <= 15; i++) {
        s32 v = sub_0822CDA8(i);
        *p1 = v;
        if ((s16)v < 0)
            continue;
        if (*p1 != p3)
            continue;
        *p0 = i;
        {
            s32 w = sub_0822CCC8(*p0);
            *p2 = w;
        }
        return 1;
    }
    return 0;
}
