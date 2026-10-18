// COMPILER: old_agbcc
#include "global.h"
/* ScriptedParticle_ResetSingleton: Clears the global pool pointer without removing sprites */
#define ScriptedParticle_ResetSingleton sub_08126650

extern u32 gUnk_02000154;

void sub_08126650(void)
{
    gUnk_02000154 = 0;
}
