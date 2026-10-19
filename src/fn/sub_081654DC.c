#include "global.h"

#ifdef NONMATCHING
// fuzzy: 88.9%
// foundry draft (worker best 86.8%); what differs is mostly register allocation and instruction order
#define ResultPanel_FillSignedIndicator SceneHud_FillDigitCells
u16 *Hud_GetTilemapCell(u32, u32, u32);
void SceneHud_FillDigitCells(s32 count, u32 x, u32 y, s32 sign)
{
    u32 tile = 1;
    u16 *p;
    if (sign >= 0) {
        tile = 0x30;
        if (sign == 0)
            tile = 0x20;
    }
    p = Hud_GetTilemapCell(0, x, y);
    if (count > 0) {
        s32 i;
        u32 mask = 0xFFFFF000;
        u32 value = tile;
        value |= mask;
        for (i = count; i != 0; i--)
            *p++ = value;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneHud_FillDigitCells);
#endif
