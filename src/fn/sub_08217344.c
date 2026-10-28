#include "global.h"
#include "render.h"
u32 Palette_CacheObjectBank(u32 key, void *src)
{
    s32 i = 0;
    u32 result;
    for (; i < (*(s32 *)&gObjPaletteBankCount); i++)
        if (gObjPaletteKeys[i] == key)
            goto found;
    if ((*(s32 *)&gObjPaletteBankCount) > 15)
        goto overflow;
    {
        s32 n = (*(s32 *)&gObjPaletteBankCount);
        gObjPaletteKeys[n] = key;
        CpuFastSet(src, ((u8 *)gObjPaletteBuffer) + n * 32, 8);
        return (*(s32 *)&gObjPaletteBankCount)++;
    }
found:
    return i;
overflow:
    return 0;
}
