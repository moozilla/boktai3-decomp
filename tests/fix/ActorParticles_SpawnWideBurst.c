#include "global.h"
#include "scene_object.h"
#include "scene_actor_reset.h"
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);
static inline u16 ActorParticles_NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}
void ActorParticles_SpawnWideBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneExtent velocity, spread;
    u32 count;
    u32 mask;
    velocity.x = -12;
    {
        u16 value = ActorParticles_NextRandom();
        mask = 7;
        velocity.y = value & mask;
    }
    velocity.z = -12;
    spread.x = 24;
    mask = 7;
    spread.y = (ActorParticles_NextRandom() & mask) + 3;
    spread.z = 24;
    count = (ActorParticles_NextRandom() & 3) + 4;
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->position, &velocity, &spread, 60, 63);
}
