#include "global.h"
void sub_0815AD78(u8 *);
void sub_0813AFEC(u8 *, u32, u32);
void sub_0814FAE8(void);
void sub_0815C0BC(u8 *p, s32 a, u32 b)
{
    if (a != 0) {
        u8 *q = p + 0x3a4;
        u8 z;
        z = 0;
        *q = 5;
        p[0x3a2] = z;
        p[0x3a3] = 1;
    } else {
        p[0x3a4] = 3;
        p[0x3a2] = a;
        p[0x3a3] = a;
    }
    *(u32 *)(p + 0x5a4) = b;
    sub_0815AD78(p);
    sub_0813AFEC(p, 0, 3);
    *(u32 *)(p + 0x58c) = (u32)sub_0814FAE8;
}
