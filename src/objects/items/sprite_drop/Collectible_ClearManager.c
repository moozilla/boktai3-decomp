// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* Collectible_ClearManager: Clears the singleton collectible manager pointer */

void Collectible_ClearManager(void)
{
    gActorParticlePool = 0;
}
