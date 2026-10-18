// COMPILER: old_agbcc
#include "global.h"
/* AreaEffect_ResetSingleton: Clears the global area effect manager pointer. */
#define AreaEffect_ResetSingleton sub_0804DF40

extern u32 gUnk_0200010C;

void sub_0804DF40(void)
{
    gUnk_0200010C = 0;
}
