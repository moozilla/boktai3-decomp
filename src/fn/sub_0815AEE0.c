#include "global.h"

struct P { u8 filler[0x94]; u32 f; u8 filler2[0x58C - 0x98]; void *fn; };

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_0814F8C8(void);

void sub_0815AEE0(struct P *p)
{
    sub_0815AD78(p);
    sub_0813AFEC(p, 0, 0);
    p->f |= 1;
    p->fn = sub_0814F8C8;
}
