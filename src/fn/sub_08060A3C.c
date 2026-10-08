#include "global.h"

struct S { u8 f[0x58]; s16 v[4]; };
extern struct S *gUnk_02000710;

s32 sub_08060A3C(s32 a)
{
    s32 i = 0;
    s16 *p = gUnk_02000710->v;
    for (; i < 4; p++, i++) {
        if (*p == a) return i;
    }
    return -1;
}
