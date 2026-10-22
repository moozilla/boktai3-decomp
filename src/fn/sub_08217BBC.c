#include "global.h"

#ifdef NONMATCHING
// fuzzy: 56.2%
// ROM keeps the channel masks inside the inner loop and hoists 0x3AA; this draft hoists the masks and spills i.
extern u8 *Palette_GetBackgroundBuffer(void);
extern u8 *gUnk_030051E0;
extern s32 gUnk_030051CC;
extern u16 *gUnk_030042E4;
extern u16 gUnk_030051F0[];
extern u16 gUnk_03004DC0[][16];

// Rewrites every object palette bank as greyscale (banks tagged 0x3AA are skipped).
void sub_08217BBC(void)
{
    s32 i;
    s32 j;
    s32 next;
    u16 *src;
    u16 *dst;

    gUnk_030051E0 = Palette_GetBackgroundBuffer();
    for (i = 0; i <= 15; i = next) {
        if (i < gUnk_030051CC)
            src = (u16 *)((u8 *)gUnk_030042E4 + (gUnk_030051F0[i] << 5));
        else
            src = gUnk_03004DC0[i];
        dst = gUnk_03004DC0[i];
        next = i + 1;
        if (gUnk_030051F0[i] != 0x3AA) {
            j = 15;
            do {
                s32 color = *src;
                s32 v = (color & 0x1F) + ((color & 0x3E0) >> 5) + ((color & 0x7C00) >> 10);
                v = (v >> 2) + (v >> 4) + (v >> 6);
                *dst = v | (v << 5) | (v << 10);
                src++;
                dst++;
                j--;
            } while (j >= 0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08217BBC);
#endif
