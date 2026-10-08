#include "global.h"

struct S0813B3E8 { u8 filler[0x52]; s16 idx; u8 f54[0x34]; s16 t[1]; };
extern struct S0813B3E8 *gUnk_02000710;

s32 sub_0813B3E8(void)
{
    s32 r;
    s32 v = *(s16 *)((u8 *)gUnk_02000710 + gUnk_02000710->idx * 2 + 0x88);
    r = 4;
    if (v > 8) {
        r = 0xc;
        if (v <= 0xa)
            r = 6;
    }
    return r;
}
