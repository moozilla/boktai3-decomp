#include "global.h"

struct S {
    u8 pad[0x1c];
    s16 x;
    s16 y;
};
void BgGfx_LoadTilemapAtPosition(u32, s32, s32);

void Registry6A37_UpdateBackgroundScroll(struct S *p)
{
    BgGfx_LoadTilemapAtPosition(3, p->x, p->y);
}
