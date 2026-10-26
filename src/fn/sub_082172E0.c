#include "global.h"
extern s32 gPaletteBanksInUse;
extern u16 gObjPaletteKeys[];
extern u8 gObjPaletteBuffer[];
u32 Palette_CacheReservedObjectBank(u32 key, void *src)
{
    s32 i = 0;
    u32 result;
    for (; i < gPaletteBanksInUse; i++)
        if (gObjPaletteKeys[i] == key)
            goto found;
    if (gPaletteBanksInUse > 1)
        goto overflow;
    {
        s32 n = gPaletteBanksInUse;
        gObjPaletteKeys[n] = key;
        CpuFastSet(src, gObjPaletteBuffer + n * 32, 8);
        return gPaletteBanksInUse++;
    }
found:
    return i;
overflow:
    return 0;
}
