#include "bosses/vanargand/departure_burst.h"
void VanargandHead_EmitPeriodicParticles(void *actor, s32 frame)
{
    u32 b = frame;
    if ((b & 0xf) == 0)
        ActorEffect_SpawnRandomizedBurst(actor, 0);
    if (((b + 3) & 0xf) == 0)
        ActorEffect_SpawnRandomizedBurst(actor, 1);
}
