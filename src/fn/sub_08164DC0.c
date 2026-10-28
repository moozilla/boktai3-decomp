// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"

// Remaining differences: loop-counter init is emitted before the hoisted 0xFFFFD000 literal in two loops.
// confidence=low: transient gauges use two independent countdowns.
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Div(s32, s32);
void StatDisplay_DrawTwoDigitCounter(s32);
void ResultPanel_ClearFiveTiles(void);
void Hud_UpdateTransientStatGauge(u8 *p)
{
    s32 draw = 0, full = 0, partial = 0, width = 0, foreground = 0, background = 0, tile = 0, n;
    u16 *cell;
    if (*(u16 *)(p + 0xa74)) {
        background = 0x39ce;
        if (p[0xa77]) {
            foreground = 0x7fff;
            p[0xa77]--;
        } else
            foreground = 0x1f;
        width = p[0xa76];
        partial = width * 8;
        if (*(u16 *)(p + 0xa70) == 0) {
            full = 0;
            partial = 0;
        } else {
            s32 scale = partial - 1;
            partial = Div((*(u16 *)(p + 0xa70) - 1) * scale, *(u16 *)(p + 0xa72) - 1) + 1;
            full = partial >> 3;
            partial -= full << 3;
        }
        tile = 0x8c;
        --*(u16 *)(p + 0xa74);
        p[0xa7f] = 1;
        draw = 1;
        if (*(u16 *)(p + 0xa7c))
            --*(u16 *)(p + 0xa7c);
    } else if (*(u16 *)(p + 0xa7c)) {
        background = 0x1f;
        if (p[0xa7e]) {
            foreground = 0x7fff;
            p[0xa7e]--;
        } else
            foreground = 0x3e0;
        width = 10;
        if (*(u16 *)(p + 0xa78) == 0) {
            full = 0;
            partial = 0;
        } else {
            s32 scale = 79;
            partial = Div((*(u16 *)(p + 0xa78) - 1) * scale, *(u16 *)(p + 0xa7a) - 1) + 1;
            full = partial >> 3;
            partial -= full << 3;
        }
        tile = 0x42;
        p[0xa7f] = 1;
        --*(u16 *)(p + 0xa7c);
        draw = 1;
    }
    if (draw) {
        if (gBgPaletteBank13Color14 != background)
            gBgPaletteBank13Color14 = background;
        if ((&gBgPaletteBank13Color14)[-1] != foreground)
            (&gBgPaletteBank13Color14)[-1] = foreground;
        cell = Hud_GetTilemapCell(0, 29, 1);
        {
            s32 mk;
            {
                register s32 t asm("r0") = -0x3000;
                asm("" : "+r"(t));
                mk = t;
            }
            n = 2;
            do {
                *cell-- = tile | mk;
                tile--;
            } while (--n >= 0);
        }
        for (n = 0; n < full; n++)
            *cell-- = 0xd089;
        if (partial) {
            *cell-- = (partial + 0x81) | -0x3000;
            n++;
        }
        for (; n < width; n++)
            *cell-- = 0xd04b;
        *cell-- = 0xd44c;
        n++;
        for (; n <= 10; n++)
            *cell-- = 0xd000;
        StatDisplay_DrawTwoDigitCounter(*(s16 *)(p + 0xa6e));
    } else if (p[0xa7f]) {
        cell = Hud_GetTilemapCell(0, 29, 1);
        {
            u32 z;
            {
                register u32 t asm("r1") = 0xd000;
                asm("" : "+r"(t));
                z = t;
            }
            asm("" : "+r"(z));
            n = 13;
            do {
                *cell-- = z;
            } while (--n >= 0);
        }
        ResultPanel_ClearFiveTiles();
        {
            register u32 off asm("r2") = 0xa7f;
            register u8 *q asm("r1");
            asm("" : "+r"(off));
            q = p + off;
            *q = 0;
        }
    }
}
