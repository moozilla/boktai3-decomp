#define SelectionPanel_GetTileAddress Hud_GetTilemapCell

#include "global.h"
u16 *Hud_GetTilemapCell(s32, s32, s32);
void SelectionPanel_ClearTileColumns(void)
{
    s32 row, i, next;
    u16 *a, *b, *c;
    for (row = 0; row <= 3; row = next) {
        a = Hud_GetTilemapCell(0, 0, row + 16);
        b = Hud_GetTilemapCell(0, 22, row + 16);
        c = Hud_GetTilemapCell(0, 26, row + 16);
        next = row + 1;
        for (i = 3; i >= 0; i--) {
            *a++ = 0xD000;
            *b++ = 0xD000;
            *c++ = 0xD000;
        }
    }
}
