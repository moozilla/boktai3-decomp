#include "global.h"
#include "render.h"
u16 *sub_081BDC14(u32 bg, u32 x, u32 y)
{
    struct BackgroundLayer *b = &gBgRecords[bg];
    u16 *m = b->map;
    return m + (x & 31) + (y & 31) * 32;
}