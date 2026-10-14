#include "global.h"
struct Actor {
    u8 pad0[0xe];
    u16 flags;
    u8 pad10[9];
    u8 facing4, facing8;
    u8 pad1b[0xb9];
    u32 drawable;
};
int DisplayAnimation_SetEntry(void *, int, int, int, int, int, int);
int sub_0802DB78(struct Actor *p, int a, int b, int stride, int c, int d)
{
    void *draw = &p->drawable;
    int direction, flip;
    if (p->drawable == 1) {
        if (p->facing4 > 1) {
            direction = 3 - p->facing4;
            flip = 1;
        } else {
            direction = p->facing4;
            flip = 0;
        }
    } else {
        u32 facing = p->facing4;
        u8 range = facing - 1;
        direction = 0;
        if (range <= 1)
            direction = stride;
        flip = 0;
        if (facing > 1)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(draw, a, direction, flip, b, c, d);
}
