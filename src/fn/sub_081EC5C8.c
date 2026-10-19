#include "global.h"
void ActorEffect_SpawnRandomizedBurst(u8 *, u32);
void SceneActors_EmitPeriodicParticles(u8 *a, u32 b)
{
    u32 m = 0xf;
    if ((b & m) == 0)
        ActorEffect_SpawnRandomizedBurst(a, 0);
    if (((b + 3) & m) == 0)
        ActorEffect_SpawnRandomizedBurst(a, 1);
}
