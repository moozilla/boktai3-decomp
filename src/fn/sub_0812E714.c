#include "global.h"
#include "scene_object.h"
#include "scene_actor_reset.h"

u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

static inline s32 ActorParticles_NextRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

void ActorParticles_SpawnWideBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneExtent velocity, spread;
    u32 count;
    velocity.x = -12;
    velocity.y = ActorParticles_NextRandom() & 7;
    velocity.z = -12;
    spread.x = 24;
    spread.y = (ActorParticles_NextRandom() & 7) + 3;
    spread.z = 24;
    count = (ActorParticles_NextRandom() & 3) + 4;
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->body.position, &velocity, &spread, 60, 63);
}
