#include "global.h"
#include "scene_object.h"
#include "scene_actor_reset.h"
#include "monster_particle_random.h"
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);
static inline s32 ActorParticles_FirstRandom(void)
{
    const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
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
    u32 sampleOrigin;
    velocity.x = ActorParticles_Half(actor->body.motion.x);
    PARTICLE_RANDOM_CAPTURE_ORIGIN(sampleOrigin);
    velocity.y = ActorParticles_FirstRandom() & 7;
    velocity.z = ActorParticles_Half(actor->body.motion.z);
    spread.x = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 3) + 15;
    spread.y = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 7) + 15;
    spread.z = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 3) + 15;
    count = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 3) + 4;
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->body.position, &velocity, &spread, 45, 31);
}
