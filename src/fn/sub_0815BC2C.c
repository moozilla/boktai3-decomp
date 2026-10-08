#include "global.h"

struct P {
    u8 f00[0x1c]; s32 st;
    u8 f20[0x3A2 - 0x20]; u8 a2; u8 a3; s8 a4;
    u8 f3a5[0x418 - 0x3A5]; u8 d18;
    u8 f419[0x457 - 0x419]; u8 c57; u8 c58;
    u8 f459[0x58C - 0x459]; void *fn;
    u8 f590[0x59A - 0x590]; u8 b9a;
    u8 f59b[0x59D - 0x59B]; u8 b9d;
    u8 f59e[0x5A4 - 0x59E]; u32 w5a4;
};
void sub_0815AD78(struct P *);
void sub_0813ADD0(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_0815B858(struct P *, s32);
void sub_0815AF14(struct P *, s32);
void sub_081502AC(void);
void sub_081507F4(void);
void sub_0822B2F8(s32);
void sub_0815130C(void);

void sub_0815BC2C(struct P *p, s32 v, u32 w)
{
    if (v >= 0)
        p->a4 = v;
    sub_0813ADD0(p);
    p->w5a4 = w;
    sub_0815AD78(p);
    sub_0813AFEC(p, 3, 2);
    p->fn = sub_0815130C;
}
