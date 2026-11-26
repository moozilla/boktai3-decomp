#include "global.h"

u16 *Hud_GetTilemapCell(s32, u32, u32);
void StatusPanel_ClearLowerRows(void)
{
    s32 row;
    for (row = 0; row <= 1; row++) {
        u16 *p = Hud_GetTilemapCell(0, 9, row + 17);
        s32 col;
        for (col = 0; col < 12; col++)
            *p++ = 0xD000;
    }
}
