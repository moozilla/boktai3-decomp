#include "global.h"
#include "render.h"

void sub_08215EB4(s32 layer, s32 x, s32 y, s32 w, s32 h, s32 tile, s32 palette, s32 priority)
{
    //BODY
    struct BackgroundLayer *l = &gBgRecords[layer];
    u16 *map = l->map;
    s32 stride = l->width * 2;
    s32 xEnd = x + w;
    s32 yEnd = h + y;
    s32 x0 = x;
    for (; y < yEnd; y++) {
        for (x = x0; x < xEnd; x++) {
            map[stride * y + x] = tile | (palette << 10) | (priority << 12);
        }
    }
}
