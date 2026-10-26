#include "global.h"
#include "render.h"

extern u16 gUnk_03004DC0[][16];
extern u16 gUnk_03004FC0[];

void sub_082171C0(void)
{
    s32 i, j;
    u16 *p;

    gObjPaletteBankCount = 0;
    gPaletteBanksInUse = 0;
    gObjPaletteReserved = 0;
    p = gUnk_03004DC0[0];
    for (i = 0; i <= 15; i++) {
        for (j = 0; j < 16; j++)
            *p++ = 0;
        gObjPaletteKeys[i] = 0;
    }
    gBgPaletteSource = gUnk_03004FC0;
    gBgFadeLevel = 0x40;
    gBgFadeApplied = 0x40;
    gFadeMultiplier = 0x40;
    gBgFadeColor = 0x1084;
    gBgFadeBankMask = 0;
    gObjFadeLevel = 0x40;
    gObjFadeApplied = 0x40;
    gObjFadeColor = 0x1084;
    gObjFadeFlags = 0;
    gObjPaletteEffect = 0;
}
