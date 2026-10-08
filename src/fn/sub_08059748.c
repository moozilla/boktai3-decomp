#include "global.h"

struct Q { u8 f[0xe6]; u16 v; };
struct R { u8 f[0xbe8]; struct Q *q[1]; };
struct P { u8 f[0x6b0]; struct R *r; };

void sub_08059748(struct P *p, s32 i)
{
    p->r->q[i]->v = 1;
}
