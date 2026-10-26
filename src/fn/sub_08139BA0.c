#include "global.h"
#include "ram.h"

void CallbackPool_RemoveEntry(s32);

s32 ParticleBurstEffect_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 5; i++)
        CallbackPool_RemoveEntry(i);
    return gSceneProcParticles = 0;
}
