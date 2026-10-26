// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* PlayerParticleCloud_InitModule: Clears the singleton pointer for the following player-following particle effect */
#define PlayerParticleCloud_InitModule sub_081D31E0

void sub_081D31E0(void)
{
    gCloudTask = 0;
}
