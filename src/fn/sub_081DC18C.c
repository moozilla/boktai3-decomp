#include "global.h"

void sub_082164AC(u32, u32, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void sub_081DC18C(u32 p, u32 b)
{
    sub_082164AC(3, p, b);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
}
