#ifndef GUARD_SCENE_ACTOR_PART_BURST_H
#define GUARD_SCENE_ACTOR_PART_BURST_H

#include "scene/actor/scene_actor_registry.h"
#include "engine/ram.h"
#include "effects/effect_group.h"
#include "effects/particles/spark_particle.h"
#include "scene/actor/scene_actor_state.h"
#include "engine/math/monster_particle_random.h"

static inline u32 SceneActorEffects_NextSample(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

static inline void SceneActorEffects_SpawnContactParts(struct ChaseActor *actor)
{
    union SceneActorRegistryPackedPosition velocity, bounds, position;
    s32 i;
    for (i = 0; i < 3; i++) {
        union RandomTableView table = {gRandomTable};
        const struct ContactPosition *origin;
        velocity.packed.x = (SceneActorEffects_NextSample(table.samples) & 15) - 7;
        velocity.packed.y = (SceneActorEffects_NextSample(table.samples) & 31) - 16;
        velocity.packed.z = (SceneActorEffects_NextSample(table.samples) & 15) - 7;
        bounds.packed.x = 5;
        bounds.packed.y = 10;
        bounds.packed.z = 5;
        origin = actor->group.effectOrigin;
        position.packed.x = origin->x;
        position.packed.y = origin->y + 128;
        position.packed.z = origin->z;
        EffectGroup_SpawnPart((void *)actor->group.effectConfig, 3, &position.position,
                              &velocity.position, &bounds.position, 40, 40);
    }
    actor->base.attachmentTimer = 0;
    actor->base.hitLockTimer = 0;
}

static inline u8 SceneActorEffects_NextSparkCount(const u16 *table)
{
    return (SceneActorEffects_NextSample(table) & 3) + 1;
}

/* Each ring draws its particle count from the shared random sample cursor. */
#define SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(actor, range, table, radius) \
    do { \
        u8 count = (ParticleRandom_ReadSample((table)) & 3) + 1; \
        SparkParticle_Spawn(&(actor)->base.body.position, count, (range), (radius), 4, 20, 2, 1); \
    } while (0)

static inline void SceneActorEffects_SpawnDefeatSparks(struct ChaseActor *actor,
                                                       const struct ContactPosition *range,
                                                       const u16 *table, u8 firstCount)
{
    s32 verticalRange, kind;
    u32 priority;
    SparkParticle_Spawn(&actor->base.body.position, firstCount, range, 32, 4, verticalRange = 20,
                        kind = 2, priority = 1);
    SparkParticle_Spawn(&actor->base.body.position, SceneActorEffects_NextSparkCount(table), range,
                        64, 4, verticalRange, kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, SceneActorEffects_NextSparkCount(table), range,
                        94, 4, verticalRange, kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, SceneActorEffects_NextSparkCount(table), range,
                        128, 4, verticalRange, kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, SceneActorEffects_NextSparkCount(table), range,
                        196, 4, verticalRange, kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, SceneActorEffects_NextSparkCount(table), range,
                        240, 4, verticalRange, kind, priority);
}

#endif
