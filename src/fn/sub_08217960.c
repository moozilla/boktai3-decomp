#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 90.7%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: loop setup, mask hoisting, backdrop temporaries and register allocation.
extern s32 gBgFadeLevel, gFadeMultiplier, gBgFadeApplied;
extern u16 *gBgPaletteSource;
extern u16 gBgPaletteBuffer[], gBgFadeBankMask, gBgFadeColor;
extern s16 gBackdropOverride, gBackdropColor;
u16 *Palette_GetBackgroundBuffer(void);
void Palette_ApplyBackgroundFade(void)
{
    u16 *buffer = Palette_GetBackgroundBuffer();
    s32 product = gBgFadeLevel * gFadeMultiplier;
    s32 value = product >> 6;
    s32 i;
    s32 j;
    u16 *src;
    u16 *dst;
    register s32 level asm("r12");
    s32 other;
    u32 rb;
    register u32 g asm("r10");
    if (value == 64) {
        gBgPaletteSource = gBgPaletteBuffer;
        return;
    }
    if (value == 0) {
        dst = buffer;
        for (i = 0; i < 16; i++) {
            if ((gBgFadeBankMask >> i) & 1) {
                for (j = 0; j < 16; j++) {
                    *(dst++) = gBgFadeColor;
                }

            } else {
                src = gBgPaletteBuffer + (i * 16);
                for (j = 0; j < 16; j++) {
                    *(dst++) = *(src++);
                }
            }
        }

        gBgPaletteSource = buffer;
        return;
    }
    level = product >> 7;
    other = 32 - level;
    gBgPaletteSource = buffer;
    if (!gBgFadeColor) {
        src = gBgPaletteBuffer;
        dst = buffer;
        for (i = 0; i < 16; i++) {
            if ((gBgFadeBankMask >> i) & 1) {
                for (j = 0; j < 16; j++) {
                    u32 c = *src;
                    *dst = (((level * (c & 0x7c1f)) & 0xf83e0) |
                            ((level * ((u16)(c & 0x3e0))) & 0x7c00)) >>
                           5;
                    src++;
                    dst++;
                }

            } else {
                for (j = 0; j < 16; j++) {
                    *(dst++) = *(src++);
                }
            }
        }

    } else {
        rb = gBgFadeColor & 0x7c1f;
        {
            u8 k1 = 0x3e0;
            g = gBgFadeColor & k1;
        }
        src = gBgPaletteBuffer;
        dst = buffer;
        for (i = 0; i < 16; i++) {
            if ((gBgFadeBankMask >> i) & 1) {
                register u32 green asm("r8") = g * other;
                register u32 mask asm("r9") = 0x7c1f;
                register u8 redblue asm("r7") = other * rb;
                for (j = 0; j < 16; j++) {
                    u32 c = *src;
                    register u32 redTerm asm("r2") = (level * (c & mask)) + redblue;
                    register u32 greenTerm asm("r0");
                    redTerm &= 0xf83e0;
                    greenTerm = ((level * (c & 0x3e0)) + green) & 0x7c00;
                    *dst = (redTerm | greenTerm) >> 5;
                    src++;
                    dst++;
                }

            } else {
                for (j = 0; j < 16; j++) {
                    *(dst++) = *(src++);
                }
            }
        }
    }
    if (gBackdropOverride) {
        u32 rb = gBgFadeColor & 0x7c1f;
        u32 g = gBgFadeColor & 0x3e0;
        s32 old = gBackdropColor;
        gBackdropColor = ((((level * (old & 0x7c1f)) + (rb * other)) & 0xf83e0) |
                          (((level * (old & 0x3e0)) + (g * other)) & 0x7c00)) >>
                         5;
    }
    gBgFadeApplied = value;
}

#else
INCLUDE_ASM("asm/nonmatching", Palette_ApplyBackgroundFade);
#endif
