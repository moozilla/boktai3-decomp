// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ScriptedParticle_ResetSingleton: Clears the global pool pointer without removing sprites */
#define ScriptedParticle_ResetSingleton sub_08126650

void sub_08126650(void)
{
    gScriptedParticlePool = 0;
}
