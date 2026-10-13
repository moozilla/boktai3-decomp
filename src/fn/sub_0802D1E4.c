#include "global.h"
struct Object {
    u8 pad0[0x128];
    u16 state;
    u8 pad12a[6];
    int ticks;
};
extern void (*gUnk_0860504C[])(struct Object *, int);
void sub_0802CF40(struct Object *);
void sub_0802D1E4(struct Object *p)
{
    int tick;
    sub_0802CF40(p);
    tick = p->ticks;
    p->ticks = tick + 1;
    gUnk_0860504C[p->state](p, tick);
}
