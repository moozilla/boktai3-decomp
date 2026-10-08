#include "global.h"

struct R { u8 f[0xbe8]; u8 *q[1]; };
struct P { u8 f[0x6b0]; struct R *r; };
void sub_0805CB88(u8 *, s32);

void sub_08059704(struct P *p, s32 i, s32 a)
{
    sub_0805CB88(p->r->q[i], a);
}
