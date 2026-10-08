#include "global.h"

struct S { u8 f[0x160]; s16 v[16]; };
extern struct S *gUnk_02000710;

s32 sub_08060928(void)
{
    s32 i = 0;
    s16 *p = gUnk_02000710->v;
    for (i = 0; i < 16; p++, i++) {
        if (*p < 0) return i;
    }
    return -1;
}
