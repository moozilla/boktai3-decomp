#include "global.h"
struct S { u8 p[0x3a4]; s32 pos; s32 vel; };
void sub_081FE0EC(struct S *p)
{
    p->pos += p->vel;
    if (p->pos > 0x60000) {
        p->pos = 0x60000;
        p->vel = -p->vel;
    }
    if (p->pos <= 0x2ffff) {
        p->pos = 0x30000;
        p->vel = -p->vel;
    }
}
