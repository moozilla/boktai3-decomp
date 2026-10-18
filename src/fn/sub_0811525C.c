// COMPILER: old_agbcc
#include "global.h"
/* EffectPool_ClearManager: Clears the singleton effect manager pointer */
#define EffectPool_ClearManager sub_0811525C

extern u32 gUnk_020001DC;

void sub_0811525C(void)
{
    gUnk_020001DC = 0;
}
