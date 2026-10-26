#include "global.h"
extern s32 gObjPaletteBankCount;
extern u16 gObjPaletteKeys[];
extern u8 gUnk_03004DC0[];
u32 Palette_CacheObjectBank(u32 key, void *src)
{
    s32 i = 0;
    u32 result;
    for (; i < gObjPaletteBankCount; i++)
        if (gObjPaletteKeys[i] == key)
            goto found;
    if (gObjPaletteBankCount > 15)
        goto overflow;
    {
        s32 n = gObjPaletteBankCount;
        gObjPaletteKeys[n] = key;
        CpuFastSet(src, gUnk_03004DC0 + n * 32, 8);
        return gObjPaletteBankCount++;
    }
found:
    return i;
overflow:
    return 0;
}
