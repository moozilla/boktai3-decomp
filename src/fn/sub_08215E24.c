#include "global.h"
#include "render.h"

void sub_08215E24(s32 layer, s32 x, s32 y, s32 w, s32 h, s32 *src, s32 palette, s32 priority)
{
    //BODY
    struct BackgroundLayer *l = &gBgRecords[layer];
    s32 stride;
    u16 *map;
    s32 xEnd, yEnd, x0;
    map = l->map;
    stride = l->width * 2;
    xEnd = x + w;
    yEnd = h + y;
    x0 = x;
    for (; y < yEnd; y++) {
        for (x = x0; x < xEnd; x++) {
            map[stride * y + x] = *src++ | (palette << 10) | (priority << 12);
        }
    }
}
