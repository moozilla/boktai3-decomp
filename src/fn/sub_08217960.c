#include "global.h"
#include "render.h"

u16 *Palette_GetBackgroundBuffer(void);
void Palette_ApplyBackgroundFade(void)
{
    u16 *buffer = Palette_GetBackgroundBuffer();
    s32 productOrBank = gBgFadeLevel * gFadeMultiplier;
    s32 value = productOrBank >> 6;
    s32 entry;
    u16 *src;
    u16 *dst;
    s32 level;
    s32 inverse;
    u32 fadeRedBlue;
    u32 fadeGreen;
    if (value == 64) {
        gBgPaletteSource = gBgPaletteBuffer;
        return;
    }
    if (value == 0) {
        dst = buffer;
        for (productOrBank = 0; productOrBank < 16; productOrBank++) {
            if ((gBgFadeBankMask >> productOrBank) & 1) {
                for (entry = 0; entry < 16; entry++) {
                    *dst++ = gBgFadeColor;
                }
            } else {
                src = gBgPaletteBuffer + (productOrBank * 16);
                for (entry = 0; entry < 16; entry++) {
                    *dst++ = *src++;
                }
            }
        }

        gBgPaletteSource = buffer;
        return;
    }
    level = productOrBank >> 7;
    inverse = 32 - level;
    gBgPaletteSource = buffer;
    if (!gBgFadeColor) {
        src = gBgPaletteBuffer;
        dst = buffer;
        for (productOrBank = 0; productOrBank < 16; productOrBank++) {
            if ((gBgFadeBankMask >> productOrBank) & 1) {
                for (entry = 0; entry < 16; entry++) {
                    u32 c = *src;
                    *dst = (((level * (c & 0x7C1F)) & 0xF83E0) |
                            ((level * ((u16)(c & 0x3E0))) & 0x7C00)) >>
                           5;
                    src++;
                    dst++;
                }
            } else {
                for (entry = 0; entry < 16; entry++) {
                    *dst++ = *src++;
                }
            }
        }
    } else {
        fadeRedBlue = gBgFadeColor & 0x7C1F;
        fadeGreen = gBgFadeColor & 0x3E0;
        src = gBgPaletteBuffer;
        dst = buffer;
        for (productOrBank = 0; productOrBank < 16; productOrBank++) {
            if ((gBgFadeBankMask >> productOrBank) & 1) {
                for (entry = 0; entry < 16; entry++) {
                    u32 c = *src;
                    *dst = (((level * ((u16)c & 0x7C1F) + (fadeRedBlue * inverse)) & 0xF83E0) |
                            ((level * (c & 0x3E0) + (fadeGreen * inverse)) & 0x7C00)) >>
                           5;
                    src++;
                    dst++;
                }
            } else {
                for (entry = 0; entry < 16; entry++) {
                    *dst++ = *src++;
                }
            }
        }
    }
    if (gBackdropOverride) {
        u32 fadeRedBlue = gBgFadeColor & 0x7C1F;
        u32 fadeGreen = gBgFadeColor & 0x3E0;
        s32 old = (s16)gBackdropColor;
        gBackdropColor = ((((level * (old & 0x7C1F)) + (fadeRedBlue * inverse)) & 0xF83E0) |
                                     (((level * (old & 0x3E0)) + (fadeGreen * inverse)) & 0x7C00)) >>
                                    5;
    }
    gBgFadeApplied = value;
}
