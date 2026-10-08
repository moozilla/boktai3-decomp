#include "global.h"

struct P { u8 filler[0x3A4]; s8 b; u8 filler2[0x58C - 0x3A5]; void *fn; };

void sub_0813ADD0(struct P *);
void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_0814F90C(void);

void sub_0815AF8C(struct P *p, s32 v)
{
    if (v >= 0)
        p->b = v;
    sub_0813ADD0(p);
    sub_0815AD78(p);
    sub_0813AFEC(p, 0, 1);
    p->fn = sub_0814F90C;
}
