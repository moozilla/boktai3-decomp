// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* PlayerParticleCloud_InitModule: Clears the singleton pointer for the following player-following particle effect */

void PlayerParticleCloud_InitModule(void)
{
    gCloudTask = 0;
}
