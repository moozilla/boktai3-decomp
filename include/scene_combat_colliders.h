#ifndef GUARD_SCENE_COMBAT_COLLIDERS_H
#define GUARD_SCENE_COMBAT_COLLIDERS_H

#include "scene_combat_actor.h"

static inline void ActorControllerMotion_ClearFlags(struct ActorControllerMotion *motion, u32 bits)
{
    u32 mask = ~bits;
    motion->flags &= mask;
}

/* Make the first collider of the group the active one: the other three get the disable bit
 * (collider flag 4) and the first loses it, and the group's motion stops (bit 0). The walk and
 * layered animations of the combat actor start this way. */
static inline void SceneCombatActor_SelectFirstCollider(struct SceneCombatActor *actor)
{
    ContactCollider_ClearFlags(&actor->colliderGroup.colliders[2], 4);
    ContactCollider_ClearFlags(&actor->colliderGroup.colliders[1], 4);
    ContactCollider_ClearFlags(&actor->colliderGroup.colliders[3], 4);
    ContactCollider_SetFlags(&actor->colliderGroup.colliders[0], 4);
    ActorControllerMotion_ClearFlags(&actor->colliderGroup.motion, 1);
}

#endif
