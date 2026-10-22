#include "global.h"

struct TextLayer {
    u8 pad0[0x18];
    s16 stride;
    u8 pad1a[0x12];
    u16 *map;
};
extern struct TextLayer gUnk_03004C30[];

void sub_08215EB4(s32 layer, s32 x, s32 y, s32 w, s32 h, s32 tile, s32 palette, s32 priority)
{
    //BODY
    struct TextLayer *l = &gUnk_03004C30[layer];
    u16 *map = l->map;
    s32 stride = l->stride * 2;
    s32 xEnd = x + w;
    s32 yEnd = h + y;
    s32 x0 = x;
    for (; y < yEnd; y++) {
        for (x = x0; x < xEnd; x++) {
            map[stride * y + x] = tile | (palette << 10) | (priority << 12);
        }
    }
}
