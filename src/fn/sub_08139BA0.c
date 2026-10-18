#include "global.h"

extern u32 gUnk_020004D0;
void CallbackPool_RemoveEntry(s32);

s32 ParticleBurstEffect_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 5; i++)
        CallbackPool_RemoveEntry(i);
    return gUnk_020004D0 = 0;
}
