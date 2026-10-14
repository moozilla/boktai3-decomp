// COMPILER: old_agbcc
#include "global.h"

extern u8 *gUnk_02000580;

s32 sub_0815AD3C(void)
{
    u8 *p = gUnk_02000580;
    p[0x45d] = 1;
    p[0x45f] = 1;
    return 0;
}
