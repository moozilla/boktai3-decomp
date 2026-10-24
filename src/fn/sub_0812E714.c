#include "global.h"
#include "scene_object.h"
#include "scene_actor_reset.h"
// MATCHING_CONCESSION: register-pinning
#define gRandomTableIndex gUnk_03005308
#define gRandomTable      gUnk_0203B400
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);
static inline u16 ActorParticles_NextRandom(u32 *index, const u16 *table)
{
    *index = (*index + 1) & 1023;
    return table[*index];
}
void ActorParticles_SpawnWideBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneExtent velocity, spread;
    u32 count;
    u32 mask;
    const u16 *table;
    u32 *indexPtr;
    u32 index;
    velocity.x = -12;
    table = gUnk_0203B400;
    indexPtr = &gUnk_03005308;
    index = *indexPtr;
    {
        u16 value = ActorParticles_NextRandom(&index, table);
        mask = 7;
        // Keep the mask reload separate from its initialization.
        asm("" ::: "r0");
        velocity.y = value & mask;
    }
    velocity.z = -12;
    spread.x = 24;
    mask = 7;
    spread.y = (ActorParticles_NextRandom(&index, table) & mask) + 3;
    spread.z = 24;
    index = (index + 1) & 1023;
    *indexPtr = index;
    {
        u16 value = table[index];
        count = (value & 3) + 4;
    }
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->position, &velocity, &spread, 60, 63);
}
