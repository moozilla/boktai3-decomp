// COMPILER: old_agbcc
#include "global.h"
/* PlayerParticleCloud_InitModule: Clears the singleton pointer for the following player-following particle effect */
#define PlayerParticleCloud_InitModule sub_081D31E0

extern u32 gUnk_02000298;

void sub_081D31E0(void)
{
    gUnk_02000298 = 0;
}
