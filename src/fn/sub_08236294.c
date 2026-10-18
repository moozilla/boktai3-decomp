#include "global.h"

struct P {
    u32 flags;
    u8 u4[0x14];
    u16 x, y, z;
    u8 u1e[0xa];
    u8 a28, a29;
    u8 u2a[2];
    u16 frame;
    u8 u2e[0xa];
    u16 vx, vy, vz;
};

void sub_08217EEC(struct P *, void *, u32);

void sub_08236294(struct P *p)
{
    void *unused;
    u32 n = p->frame + 1;
    if ((p->frame = n) > 13) {
        p->flags |= 1;
        p->a28 = 0;
    } else {
        p->x += p->vx;
        p->y += p->vy;
        p->z += p->vz;
        if ((u16)n == 10)
            sub_08217EEC(p, unused, p->a29 + 1);
    }
}
