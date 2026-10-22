#include "global.h"

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
    u16 tag;

    gUnk_030051E0 = Palette_GetBackgroundBuffer();
    for (i = 0; i <= 15; i = next) {
        if (i < gUnk_030051CC)
            src = (u16 *)((u8 *)gUnk_030042E4 + (gUnk_030051F0[i] << 5));
        else
            src = gUnk_03004DC0[i];
        dst = gUnk_03004DC0[i];
        tag = gUnk_030051F0[i];
        next = i + 1;
        if (tag != 0x3AA) {
            j = 15;
            do {
                s32 color = *src;
                s32 r = color & 0x1F, g = (color & 0x3E0) >> 5, b = (color & 0x7C00) >> 10;
                s32 v = r + g + b;
                v = (v >> 2) + (v >> 4) + (v >> 6);
                {
                    u16 o = v | (v << 5) | (v << 10);
                    *dst = o;
                }
                src++;
                dst++;
                j--;
            } while (j >= 0);
        }
    }
}
