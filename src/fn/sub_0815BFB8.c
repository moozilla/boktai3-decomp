#include "global.h"

struct P {
    u8 f00[0x1c]; s32 st;
    u8 f20[0x3A2 - 0x20]; u8 a2; u8 a3; s8 a4;
    u8 f3a5[0x418 - 0x3A5]; u8 d18;
    u8 f419[0x457 - 0x419]; u8 c57; u8 c58;
    u8 f459[0x58C - 0x459]; void *fn;
    struct Q { u32 a, b; } v590; u8 f598[2]; u8 b9a; u8 b9b;
    u8 f59c; u8 b9d;
    u8 f59e[0x5A4 - 0x59E]; u32 w5a4;
};
void sub_0813AE18(struct P *, s32, s32, s32);
s32 sub_081419D4(s32, s32);
void sub_0815AD78(struct P *);
void sub_0813ADD0(struct P *);
void sub_0815B858(struct P *, s32);
void sub_0815AF14(struct P *, s32);
void sub_081502AC(void);
void sub_081507F4(void);
void sub_0822B2F8(s32);
void sub_08151988(void);
void sub_0813AFEC();

void sub_0815BFB8(struct P *p)
{
    sub_0815AD78(p);
    p->a4 = 1;
    p->a2 = 0;
    p->a3 = 0;
    sub_0813AFEC(p, 0x11);
    p->fn = sub_08151988;
}
