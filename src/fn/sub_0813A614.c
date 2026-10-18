// COMPILER: old_agbcc
#include "global.h"
/* FollowingSpriteEffect_ResetManager: Clears the two-slot following effect manager singleton pointer */
#define FollowingSpriteEffect_ResetManager sub_0813A614

extern u32 gUnk_020004D4;

void sub_0813A614(void)
{
    gUnk_020004D4 = 0;
}
