#include "global.h"
#include "render.h"

/* tentative: copies a palette bank, optionally brightening each channel capped at 31 */

struct PalFadeObj {
    u8 pad0[0x360];
    u16 palette[16];
    u8 pad380[0x418 - 0x380];
    u8 mode;
    u8 copyAll;
    u8 pad41A[0xAB0 - 0x41A];
    u16 paletteBanks[10];
    s16 bankIndex;
    u8 brighten;
};

void PaletteFade_ApplyBrightenedPalette(struct PalFadeObj *p)
{
    u16 *src;
    s32 i;

    src = (u16 *)(gPaletteBankData + p->paletteBanks[p->bankIndex] * 32);
    if (p->brighten == 0) {
        if (p->copyAll == 0) {
            u8 *mode;
            u16 *dst;
            i = 0;
            mode = &p->mode;
            dst = p->palette;
            for (; i < 16; i++) {
                if (*mode == 2 || !((u32)(i - 5) <= 1 || i == 13))
                    *dst = *src;
                src++;
                dst++;
            }
        } else {
            u16 *dst = p->palette;
            for (i = 15; i >= 0; i--)
                *dst++ = *src++;
        }
    } else {
        if (p->copyAll == 0) {
            u8 *add;
            u16 *dst;
            i = 0;
            add = &p->brighten;
            dst = p->palette;
            for (; i < 16; i++) {
                if (!((u32)(i - 5) <= 1 || i == 13)) {
                    u16 c = *src;
                    s32 r, g, b;
                    r = (c & 0x1F) + *add;
                    if (r > 0x1F)
                        r = 0x1F;
                    g = (s16)(((u32)(c << 16) >> 21) & 0x1F) + *add;
                    if (g > 0x1F)
                        g = 0x1F;
                    b = (s16)(((u32)(c << 16) >> 26) & 0x1F) + *add;
                    if (b > 0x1F)
                        b = 0x1F;
                    *dst = (b << 10) | (g << 5) | r;
                }
                src++;
                dst++;
            }
        } else {
            s32 mask = 0x1F;
            u8 *add = &p->brighten;
            u16 *dst = p->palette;
            for (i = 15; i >= 0; i--) {
                u16 c = *src;
                s32 r, g, b;
                r = (c & 0x1F) + *add;
                if (r > 0x1F)
                    r = 0x1F;
                g = (((u32)(c << 16) >> 21) & mask) + *add;
                if (g > 0x1F)
                    g = 0x1F;
                b = (((u32)(c << 16) >> 26) & mask) + *add;
                if (b > 0x1F)
                    b = 0x1F;
                *dst = (b << 10) | (g << 5) | r;
                src++;
                dst++;
            }
        }
    }
}
