// COMPILER: old_agbcc
#include "global.h"

extern u8 *gUnk_02000710;

void sub_0811D234(void)
{
    u32 m = 0x100;
    *(u32 *)(gUnk_02000710 + 0x868) |= m;
}
