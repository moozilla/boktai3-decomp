#include "global.h"

extern u8 *gUnk_0200059C;

void PlayerAttackEffect_SetElementMode(u32 v)
{
    if (gUnk_0200059C != 0)
        *(u32 *)(gUnk_0200059C + 0x5A0) = v;
}
