#ifndef GUARD_SKELETON_FLAG_ACCESSORS_H
#define GUARD_SKELETON_FLAG_ACCESSORS_H

#include "monsters/chase_actor.h"
#include "scene/actor/scene_actor_components.h"

static inline void ChaseActor_ClearMotionFlags(struct ChaseActor *p, u32 mask)
{
    p->base.motionFlags &= ~mask;
}

static inline u32 SceneActorChaseTracking_TestFlags(struct SceneActorChaseTracking *p, u32 mask)
{
    return p->flags & mask;
}

#endif
