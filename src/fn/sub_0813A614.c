// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* FollowingSpriteEffect_ResetManager: Clears the two-slot following effect manager singleton pointer */
#define FollowingSpriteEffect_ResetManager sub_0813A614

void sub_0813A614(void)
{
    gSpritePairPool = 0;
}
