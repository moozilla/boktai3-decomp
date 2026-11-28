#include "global.h"
#include "engine/video/render.h"

u32 Palette_CacheReservedObjectBank(u32 key, void *src)
{
    s32 i;
    for (i = 0; i < gPaletteBanksInUse; i++) {
        if (gObjPaletteKeys[i] == key)
            return i;
    }
    if (gPaletteBanksInUse > 1)
        return 0;
    gObjPaletteKeys[gPaletteBanksInUse] = key;
    CpuFastCopy(src, gObjPaletteBuffer[gPaletteBanksInUse], 32);
    return gPaletteBanksInUse++;
}
