#include "global.h"
struct Actor {
    u8 pad0[0xb2];
    u8 active;
    u8 padb3[3];
    u8 mode, speed;
    u8 padb8[0x7fc];
    u8 animation[16];
    void *resource;
};
void sub_08220D20(void *, void *, int, int, int);
void sub_08220C8C(void *, void *, int, int, int);
void sub_080297FC(struct Actor *p, int sequence, int start, int speed)
{
    p->mode = 0;
    p->speed = speed;
    if (start) {
        sub_08220D20(p->animation, p->resource, (u16)sequence, p->mode, p->speed);
        p->active = 0;
    } else
        sub_08220C8C(p->animation, p->resource, (u16)sequence, p->mode, p->speed);
}
