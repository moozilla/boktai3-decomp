// COMPILER: old_agbcc
#include "global.h"
/* TimedSpriteEffect_ResetManager: Clears the singleton pointer for the four-slot effect manager */
#define TimedSpriteEffect_ResetManager sub_08138ABC

extern u32 gUnk_020004C0;

void sub_08138ABC(void)
{
    gUnk_020004C0 = 0;
}
