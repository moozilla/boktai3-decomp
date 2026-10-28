#include "global.h"
#include "render.h"

// confidence=low: health meter drawn as a 10-cell bar (8 steps per cell).
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Div(s32, s32);

void Hud_DrawHealthMeterCells(u8 *p, s32 mode, s32 row, s32 col, s32 cap, s32 value, s32 max)
{
    s32 units, full, partial, n;
    s32 fullTile, emptyTile, endTile;
    s32 color;
    u16 *cell;
    if (value == 0) {
        units = 0;
        full = 0;
        partial = 0;
    } else {
        units = Div((value - 1) * 79, max - 1) + 1;
        full = units >> 3;
        partial = units - (full << 3);
    }
    if (mode == 0) {
        fullTile = 0x43;
        emptyTile = 0x4b;
        endTile = 0x4c;
        if (units > 39) {
            color = 0x3e0;
        } else if (units > 19) {
            color = 0x29f;
        } else {
            u16 *timer = (u16 *)(p + 0xa60);
            u16 t = *timer;
            s32 bit = (t >> 2) & 1;
            color = 0x12;
            if (bit)
                color = 0x109f;
            *timer = t + 1;
        }
        if (cap == 0)
            cell = Hud_GetTilemapCell(0, row, col);
        else
            cell = Hud_GetTilemapCell(0, row - 1, col);
    } else {
        fullTile = 0x63;
        emptyTile = 0x6b;
        endTile = 0x6c;
        color = 0x3e0;
        if (units <= 39) {
            color = 0x109f;
            if (units > 19)
                color = 0x29f;
        }
        if (cap != 0)
            cell = Hud_GetTilemapCell(0, row - 2, col);
        else
            cell = Hud_GetTilemapCell(0, row - 1, col);
    }
    if (gBgPaletteBank13Color11 != color)
        gBgPaletteBank13Color11 = color;
    if (mode != 0 && cap != 0)
        *cell++ = endTile | 0xd400;
    for (n = 0; n < full; n++)
        *cell++ = fullTile | 0xd000;
    if (partial) {
        s32 r = partial - 8;
        *cell++ = (fullTile - r) | 0xd000;
        n++;
    }
    for (; n < 10; n++)
        *cell++ = emptyTile | 0xd000;
    if (cap != 0)
        *cell = endTile | 0xd000;
}
