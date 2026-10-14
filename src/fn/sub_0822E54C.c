// COMPILER: old_agbcc
#include "global.h"

extern u8 *gUnk_02000710;

void sub_0822E54C(s32 i)
{
    *(s16 *)(gUnk_02000710 + i * 2 + 0x88) = -1;
}
