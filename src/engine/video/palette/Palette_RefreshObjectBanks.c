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
    s32 next;
    u16 *src;
    u16 *dst;

    gBgPaletteSource = Palette_GetBackgroundBuffer();
    for (i = 0; i < gPaletteBanksInUse; i = next) {
        src = (u16 *)((u8 *)gPaletteBankData + (gObjPaletteKeys[i] << 5));
        dst = gObjPaletteBuffer[i];
        next = i + 1;
        j = 15;
        do {
            *dst++ = *src++;
            j--;
        } while (j >= 0);
    }
}
