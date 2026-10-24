// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_object.h"
#include "scene_actor_reset.h"
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);
static inline s32 ActorParticles_NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}
static inline s32 ActorParticles_FirstRandom(void)
{
    const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}
static inline s32 ActorParticles_Half(s16 n)
{
    if (n >= 0)
        return n >> 1;
    return (s16) - ((-n) >> 1);
}
void ActorParticles_SpawnBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneExtent velocity, spread;
    u32 count;
    u32 observed;
    velocity.x = ActorParticles_Half(actor->motion.x);
    /* Preserve the unused setup constant retained by the original RNG expansion. */
    observed = 0x0403B714;
    asm volatile("" : "+l"(observed));
    velocity.y = ActorParticles_FirstRandom() & 7;
    asm volatile("" : "+r"(observed));
    velocity.z = ActorParticles_Half(actor->motion.z);
    spread.x = (ActorParticles_NextRandom() & 3) + 15;
    spread.y = (ActorParticles_NextRandom() & 7) + 15;
    spread.z = (ActorParticles_NextRandom() & 3) + 15;
    count = (ActorParticles_NextRandom() & 3) + 4;
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->position, &velocity, &spread, 45, 31);
}
