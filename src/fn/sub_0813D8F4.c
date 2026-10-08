#include "global.h"

extern u8 *gUnk_02000710;

void sub_0813D8F4(u8 *p, s32 idx)
{
    s32 off = idx << 1;
    u8 *base = p + 0x530;
    *(u16 *)(base + off) = 0;
    if (idx == 1) {
        u16 v = *(u16 *)(gUnk_02000710 + 0x10);
        *(u8 *)(p + 0x54A) = v;
    }
}
