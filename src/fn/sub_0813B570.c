#include "global.h"

struct P0813B570 { u8 f0[0x332]; s16 a; u8 f334[0x41c - 0x334]; u16 b; };
struct G0813B570 { u8 f0[0x2a]; s16 v; };
extern u32 gUnk_030053F4;
extern struct G0813B570 *gUnk_02000710;

s32 sub_0813B570(struct P0813B570 *p)
{
    s32 s;
    if (gUnk_030053F4 & 0x1000)
        return gUnk_02000710->v;
    { u8 *q = (u8 *)p + 0x32C; s = *(s16 *)(q + 6) + *(u16 *)(q + 0xF0); }
    if (s > 0x78)
        s = 0x78;
    return s * 10;
}
