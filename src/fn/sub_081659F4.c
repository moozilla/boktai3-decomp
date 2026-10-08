#include "global.h"
struct P { u8 f[0x43E9]; u8 v; };
void sub_08165B94(struct P *, s32, s32);
void sub_08165B5C(struct P *, s32, s32, s32);
void sub_08165B28(struct P *, s32, s32);
void sub_081659F4(struct P *p, s32 b)
{
    s32 r;
    sub_08165B94(p, 7, 1);
    sub_08165B5C(p, 7, 0, 0);
    p->v = b;
    if ((u8)b)
        r = 0x4b;
    else
        r = 0x4c;
    sub_08165B28(p, 7, r);
}
