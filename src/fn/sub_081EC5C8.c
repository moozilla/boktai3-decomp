#include "departure_burst.h"
void SceneActors_EmitPeriodicParticles(void *actor, s32 frame)
{
    u32 b = frame;
    u32 m = 0xf;
    if ((b & m) == 0)
        ActorEffect_SpawnRandomizedBurst(actor, 0);
    if (((b + 3) & m) == 0)
        ActorEffect_SpawnRandomizedBurst(actor, 1);
}
