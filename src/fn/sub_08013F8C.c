// COMPILER: old_agbcc
#include "global.h"
/* ParticleManager_ClearTaskPtr: zeroes the ParticleManager singleton pointer gUnk_02000060, same global ParticleManager_Kill clears on destroy */
#define ParticleManager_ClearTaskPtr sub_08013F8C

extern u32 gUnk_02000060;

void sub_08013F8C(void)
{
    gUnk_02000060 = 0;
}
