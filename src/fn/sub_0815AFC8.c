#include "global.h"

struct P { u8 filler[0x3A4]; s8 b; u8 filler2[0x58C - 0x3A5]; void *fn; u8 filler3[0x5A4 - 0x590]; u32 c; };

void sub_0813ADD0(struct P *);
void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_0814F90C(void);

void sub_0815AFC8(struct P *p, s32 v, u32 c)
{
    if (v >= 0)
        p->b = v;
    sub_0813ADD0(p);
    p->c = c;
    sub_0815AD78(p);
    sub_0813AFEC(p, 0, 2);
    p->fn = sub_0814F90C;
}
