// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* Collectible_ClearManager: Clears the singleton collectible manager pointer */
#define Collectible_ClearManager sub_08115D4C

void sub_08115D4C(void)
{
    gActorParticlePool = 0;
}
