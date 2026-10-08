#include "global.h"

extern u8 *gUnk_02000710;
void sub_0813EDDC(u32, void *, s32);

s32 sub_0815A384(s32 i)
{
    u8 buf[16];
    s32 r;
    u8 *q;
    u8 *g = gUnk_02000710;
    i <<= 1;
    q = (u8 *)i;
    q += (s32)g;
    r = *(s16 *)(q + 0x18) + *(s16 *)(q + 0x20);
    sub_0813EDDC(0, buf, -1);
    {
        u8 *e = buf + 6;
        e += i;
        r += *(s16 *)e;
    }
    return r;
}
