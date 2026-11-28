#include "global.h"
#include "engine/video/render.h"

u32 Palette_CacheObjectBank(u32 key, void *src)
{
    s32 i;
    for (i = 0; i < gObjPaletteBankCount; i++) {
        if (gObjPaletteKeys[i] == key)
            return i;
    }
    if (gObjPaletteBankCount > 15)
        return 0;
    gObjPaletteKeys[gObjPaletteBankCount] = key;
    CpuFastCopy(src, gObjPaletteBuffer[gObjPaletteBankCount], 32);
    return gObjPaletteBankCount++;
}
