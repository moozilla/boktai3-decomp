#include "global.h"
#include "render.h"

u16 *Hud_GetTilemapCell(s32 i, u32 x, u32 y)
{
    struct BackgroundLayer *e;
    u16 *p;
    u32 m;
    e = &gBgRecords[i];
    p = e->map;
    m = 0x1f;
    p += x & m;
    p = (u16 *)((u8 *)p + ((y & m) << 6));
    return p;
}
