#include "global.h"
#include "render.h"

void Bg_SetTilemapEntry(u32 n, s32 x, s32 y, u32 a, u32 b, u32 c)
{
    struct BackgroundLayer *bg = &gBgRecords[n];
    bg->map[(bg->width << 1) * y + x] = a | (b << 10) | (c << 12);
}
