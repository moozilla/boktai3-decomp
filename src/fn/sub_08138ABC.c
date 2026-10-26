// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* TimedSpriteEffect_ResetManager: Clears the singleton pointer for the four-slot effect manager */
#define TimedSpriteEffect_ResetManager sub_08138ABC

void sub_08138ABC(void)
{
    gSpriteSlots = 0;
}
