#include "global.h"
#include "engine/video/render.h"

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
            for (i = 0; i < 16; i++) {
                if (p->mode == 2 || !(i == 5 || i == 6 || i == 13))
                    p->palette[i] = *src;
                src++;
            }
        } else {
            for (i = 0; i < 16; i++)
                p->palette[i] = *src++;
        }
    } else {
        if (p->copyAll == 0) {
            for (i = 0; i < 16; i++) {
                if (!(i == 5 || i == 6 || i == 13)) {
                    s32 color = *src;
                    s32 r = (*src & 0x1F) + p->brighten;
                    s32 g, b;
                    if (r > 0x1F)
                        r = 0x1F;
                    g = (((u16)color >> 5) & 0x1F) + p->brighten;
                    if (g > 0x1F)
                        g = 0x1F;
                    b = (((u16)color >> 10) & 0x1F) + p->brighten;
                    if (b > 0x1F)
                        b = 0x1F;
                    p->palette[i] = (b << 10) | (g << 5) | r;
                }
                src++;
            }
        } else {
            for (i = 0; i < 16; i++) {
                s32 color = *src;
                s32 r = (*src & 0x1F) + p->brighten;
                s32 g, b;
                if (r > 0x1F)
                    r = 0x1F;
                g = (((u16)color >> 5) & 0x1F) + p->brighten;
                if (g > 0x1F)
                    g = 0x1F;
                b = (((u16)color >> 10) & 0x1F) + p->brighten;
                if (b > 0x1F)
                    b = 0x1F;
                p->palette[i] = (b << 10) | (g << 5) | r;
                src++;
            }
        }
    }
}
