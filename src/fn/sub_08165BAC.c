#define MenuTilemap_ClearRect sub_08165BAC
#define MenuTilemap_GetCell Hud_GetTilemapCell
#include "global.h"
extern u16 *MenuTilemap_GetCell(u32, s32, s32);
void sub_08165BAC(s32 x, s32 y, s32 width, s32 height)
{
    s32 row;
    for (row = 0; row < height; row++) {
        u16 *cell = MenuTilemap_GetCell(0, x, y + row);
        s32 col;
        for (col = 0; col < width; col++) {
            *cell++ = 0xf001;
        }
    }
}
