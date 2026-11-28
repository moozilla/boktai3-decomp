#include "global.h"
#include "engine/video/render.h"
#include "link/player/link_player_palette.h"

/* tentative: copies a palette bank, optionally brightening each channel capped at 31 */

/* tentative: copies a palette bank, optionally brightening each channel capped at 31 */
void PaletteFade_ApplySelectiveBrightness(struct LinkPlayerFade *p)
{
    u16 *src;
    s32 i;

    src = (u16 *)(gPaletteBankData + p->paletteBanks[p->modeArg] * 32);
    if (p->brightness == 0) {
        if (p->alternate == 0) {
            u16 *dst;

            for (i = 0, dst = p->basePalette; i < 16; i++) {
                if (!(i == 5 || i == 6 || i == 13))
                    *dst = *src;
                src++;
                dst++;
            }
        } else {
            u16 *dst = p->basePalette;

            for (i = 15; i >= 0; i--)
                *dst++ = *src++;
        }
    } else {
        if (p->alternate == 0) {
            u8 *add;
            u16 *dst;
            i = 0;
            add = &p->brightness;
            dst = p->basePalette;
            for (; i < 16; i++) {
                if (!(i == 5 || i == 6 || i == 13)) {
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
            u8 *add = &p->brightness;
            u16 *dst = p->basePalette;
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
