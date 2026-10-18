// COMPILER: old_agbcc
#include "global.h"
/* SceneActorEffects_ResetManager: Clears the singleton manager pointer shared by actor effect groups */
#define SceneActorEffects_ResetManager sub_08231438

extern u32 gUnk_0200005C;

void sub_08231438(void)
{
    gUnk_0200005C = 0;
}
