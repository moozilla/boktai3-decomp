#include "global.h"
struct P { u8 f0[0xA40]; u8 *o; };
void sub_0813ED14(u8 *, s32);
void sub_08168DB0(struct P *p)
{
    u8 *o = p->o;
    s8 i = *((s8 *)p + 0xF4A);
    s8 *q = (s8 *)p;
    q += i * 4;
    q += 0xF51;
    sub_0813ED14(o, *q);
}
