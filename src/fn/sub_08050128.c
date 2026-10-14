#include "global.h"
struct Act {
    u8 pad[0x30];
    s16 x;
    s16 padx;
    s16 y;
};
struct Map {
    u8 pad[0x24];
    u16 cells[1];
};
extern struct Act *gUnk_02000580;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
struct Obj {
    u8 pad[0x1e];
    u16 v;
};
int sub_08050128(struct Obj *o)
{
    s32 cx = (s16)gUnk_02000580->x >> 8;
    s32 cy = (s16)gUnk_02000580->y >> 8;
    u32 r;
    if (cx < 0 || cy < 0 || cx >= gUnk_030052F8 || cy >= gUnk_030052FC)
        r = 0;
    else
        r = gUnk_030052F4->cells[cy] + cx;
    if (r == o->v)
        return 1;
    return 0;
}
