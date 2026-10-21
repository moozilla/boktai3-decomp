#include "global.h"
#include "player.h"
#define StatDisplay_GetTileAddress Hud_GetTilemapCell
struct StatDisplay {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[0x28];
    u8 enabled;
};
u16 *StatDisplay_GetTileAddress(u32, s32, s32);
s32 Div(s32, s32);
void StatDisplay_DrawGauge(struct StatDisplay *p, s32 x, s32 y)
{
    s32 filled, i;
    u16 *tiles;
    if (!p->enabled) {
        tiles = StatDisplay_GetTileAddress(0, x - 3, y);
        i = 0;
        do {
            *tiles++ = 0xD000;
        } while (++i < 14);
    } else {
        if ((u8)(p->player->actionMode - 1) <= 1) {
            if (p->player->statGaugeFull == 1)
                filled = 10;
            else
                filled = Div(p->player->statusValue, 400);
        } else
            filled = p->player->statWhole;
        tiles = StatDisplay_GetTileAddress(0, x, y);
        for (i = 0; i < filled; i++)
            *tiles++ = 0xD05C;
        for (; i < 10; i++)
            *tiles++ = 0xD05E;
    }
}
