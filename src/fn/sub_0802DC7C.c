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
int sub_0802DC7C(struct Actor *p, int a, int b, int stride, int c, int d)
{
    int direction, flip;
    if (p->drawable == 1) {
        u32 facing = p->facing8;
        direction = 0;
        if (facing == 4)
            direction = stride;
        flip = 0;
        if (facing > 4)
            flip = 1;
    } else {
        u32 facing = p->facing8;
        direction = 0;
        if (facing == 4)
            direction = stride;
        flip = 0;
        if (facing > 4)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(&p->drawable, a, direction, flip, b, c, d);
}
