#include "global.h"

extern u8 *Palette_GetBackgroundBuffer(void);
extern u8 *gBgPaletteSource;
extern s32 gPaletteBanksInUse;
extern u16 *gPaletteBankData;
extern u16 gObjPaletteKeys[];
extern u16 gObjPaletteBuffer[][16];

// Rewrites every object palette bank as greyscale (banks tagged 0x3AA are skipped).
void sub_08217BBC(void)
{
    s32 i;
    s32 j;
    u16 *src;
    u16 *dst;

    gBgPaletteSource = Palette_GetBackgroundBuffer();
    for (i = 0; i < 16; i++) {
        if (i < gPaletteBanksInUse)
            src = gPaletteBankData + gObjPaletteKeys[i] * 16;
        else
            src = gObjPaletteBuffer[i];
        dst = gObjPaletteBuffer[i];
        if (gObjPaletteKeys[i] != 0x3AA) {
            for (j = 0; j < 16; j++) {
                s32 color = *src;
                s32 r = color & 0x1F;
                s32 g = (color & 0x3E0) >> 5;
                s32 b = (color & 0x7C00) >> 10;
                s32 gray = r + g + b;
                u16 result;
                gray = (gray >> 2) + (gray >> 4) + (gray >> 6);
                result = gray | (gray << 5) | (gray << 10);
                *dst = result;
                src++;
                dst++;
            }
        }
    }
}
