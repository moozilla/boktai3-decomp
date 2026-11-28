#include "global.h"

extern u8 *Palette_GetBackgroundBuffer(void);
extern u8 *gBgPaletteSource;
extern s32 gPaletteBanksInUse;
extern u16 *gPaletteBankData;
extern u16 gObjPaletteKeys[];
extern u16 gObjPaletteBuffer[][16];

void Palette_RefreshObjectBanks(void)
{
    s32 i;
    s32 j;
    u16 *src;
    u16 *dst;

    gBgPaletteSource = Palette_GetBackgroundBuffer();
    for (i = 0; i < gPaletteBanksInUse; i++) {
        src = gPaletteBankData + gObjPaletteKeys[i] * 16;
        dst = gObjPaletteBuffer[i];
        for (j = 0; j < 16; j++)
            *dst++ = *src++;
    }
}
