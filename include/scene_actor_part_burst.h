#ifndef GUARD_SCENE_ACTOR_PART_BURST_H
#define GUARD_SCENE_ACTOR_PART_BURST_H

#include "scene_actor_registry.h"
#include "ram.h"
#include "effect_group.h"

static inline u32 SceneActorEffects_NextSample(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

static inline void SceneActorEffects_SpawnContactParts(struct SceneActorRegistryActor *actor)
{
    union SceneActorRegistryPackedPosition velocity, bounds, position;
    s32 i;
    for (i = 2; i >= 0; i--) {
        const u16 *table = gRandomTable;
        const struct ContactPosition *origin;
        velocity.packed.x = (SceneActorEffects_NextSample(table) & 15) - 7;
        velocity.packed.y = (SceneActorEffects_NextSample(table) & 31) - 16;
        velocity.packed.z = (SceneActorEffects_NextSample(table) & 15) - 7;
        bounds.packed.x = 5;
        bounds.packed.y = 10;
        bounds.packed.z = 5;
        origin = actor->effectOrigin;
        position.packed.x = origin->x;
        position.packed.y = origin->y + 128;
        position.packed.z = origin->z;
        EffectGroup_SpawnPart((void *)actor->effectConfig, 3, &position.position,
                              &velocity.position, &bounds.position, 40, 40);
    }
    actor->fieldAA = 0;
    actor->field2B8 = 0;
}
#endif
