#define gResourceSlotCount gPaletteBanksInUse
#define gResourceSlotIds   gObjPaletteKeys
#include "engine/shared/global.h"
#include "engine/shared/render.h"
void ResourceSlots_Reset(void)
{
    s32 i;
    gPaletteBanksInUse = 0;
    for (i = 0; i < 2; i++)
        gObjPaletteKeys[i] |= 0xffff;
}
