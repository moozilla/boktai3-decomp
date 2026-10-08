#include "global.h"

struct P { u8 filler[0x58C]; void *fn; u8 filler2[0x59D - 0x590]; u8 b; u8 filler3[0x5A4 - 0x59E]; u32 c; };

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_0814FA10(void);

void sub_0815AF50(struct P *p, u8 b, u32 c)
{
    p->b = b;
    p->c = c;
    p->fn = sub_0814FA10;
    sub_0815AD78(p);
    sub_0813AFEC(p, 0, 0);
}
