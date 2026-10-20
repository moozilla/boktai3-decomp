#include "global.h"
/* ObjShape_InitTables: Fills the sprite shape width/height tables (pixels, tiles, area) and the per-index attribute words from the shape size table */
#define ObjShape_InitTables sub_08217120

extern u8 gUnk_03004D90[], gUnk_03004D20[], gUnk_03004D40[], gUnk_03004D10[], gUnk_03004D30[];
extern u32 gUnk_03004D50[];
extern const u16 gUnk_086140EC[];

void sub_08217120(void)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        u32 e = gUnk_086140EC[i];
        s32 lo = *(const u8 *)&gUnk_086140EC[i];
        s32 hi = e >> 8;
        s32 tw = lo >> 3;
        s32 th = hi >> 3;
        gUnk_03004D90[i] = lo;
        gUnk_03004D20[i] = hi;
        gUnk_03004D40[i] = tw;
        gUnk_03004D10[i] = th;
        gUnk_03004D30[i] = tw * th;
        gUnk_03004D50[i] = ((i & 3) << 14) | ((i & 12) << 28);
    }
}
