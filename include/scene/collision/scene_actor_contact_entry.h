#ifndef GUARD_SCENE_ACTOR_CONTACT_ENTRY_H
#define GUARD_SCENE_ACTOR_CONTACT_ENTRY_H

#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/actor/scene_actor_placement.h"

/* Entering a contact wait drops the pending-contact, route-hold and blocked-destination bits and
   restores the default animation speed. */
static inline void SceneActorContact_ResetEntry(struct ChaseActor *actor)
{
    SceneActorChase_AndWordFlags(&actor->base.behaviorFlags, ~0x80);
    SceneActorChase_AndWordFlags(&actor->base.statusFlags, ~0x1000000);
    SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x200);
    actor->base.field2C2 = 0x40;
}

#endif
