#include "global.h"
struct Object {
    u8 pad0[0xc];
    s16 pos[4];
    u8 pad14[0x3c];
    u32 flags, transient;
    u8 pad58[0xc4];
    void (*callback)(void *, int);
    u8 pad120[0x14];
    int ticks;
    u8 pad138[0x49];
    u8 facing;
    u8 pad182[2];
    u32 drawable;
};
void sub_0802CB94(struct Object *);
void sub_080426B0(void *, void *);
void sub_080423C8(void *);
int sub_08042320(void *, int, int, int, int, int, int);
int sub_0802D080(struct Object *p, int a, int b, int stride, int c, int d)
{
    int direction, flip;
    if (p->drawable == 1) {
        if (p->facing > 4) {
            direction = 8 - p->facing;
            flip = 1;
        } else {
            direction = p->facing;
            flip = 0;
        }
    } else {
        if (p->facing > 4) {
            direction = 8 - p->facing;
            flip = 1;
        } else {
            direction = p->facing;
            flip = 0;
        }
        direction *= stride;
    }
    return sub_08042320(&p->drawable, a, direction, flip, b, c, d);
}
