#include "global.h"
struct S { u8 p[0x3a4]; s32 pos; s32 vel; };
void sub_081FE12C(struct S *p)
{
    p->pos += p->vel;
    if (p->pos > 0x30000) {
        p->pos = 0x30000;
        p->vel = -p->vel;
    }
    if (p->pos <= 0x17fff) {
        p->pos = 0x18000;
        p->vel = -p->vel;
    }
}
